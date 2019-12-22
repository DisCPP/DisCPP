#include "bot.h"
#include "utils.h"
#include "ixwebsocket/IXNetSystem.h"

#include <iostream>

namespace discord {
	Bot::Bot(std::string token, std::string prefix) : token(token), prefix(prefix) {
		
	}

	int Bot::Run() {
		/**
		 * @brief Runs your discord bot
		 *
		 * Runs your discord bot, handles your event loop and launches your gateway handling thread.
		 *
		 * Keep in mind that this call is blocking indefinitely, meaning that you can only call this once.
		 * This function should be ran in your main() function, and should probably be `return`'ed from main.
		 *
		 * @return int, always 0 for now.
		 */
		
		WebSocketStart();
		while (true) {
			for (size_t i = 0; i < futures.size(); i++) {
				if (!futures[i].valid() || !(futures[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready)) {
					continue;
				}
				futures.erase(futures.begin() + i);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		return 0;
	}
	
	void Bot::BindEvents() {
		internal_event_map["READY"] = std::bind(&Bot::ReadyEvent, this, std::placeholders::_1);
		internal_event_map["HELLO"] = std::bind(&Bot::HelloEvent, this, std::placeholders::_1);
	}

	void Bot::WebSocketStart() {
		nlohmann::json gateway_request = SendGetRequest(Endpoint("/gateway/bot"), { {"Authorization", Format("Bot %", token) }, { "User-Agent", "DiscordBot (https://github.com/seanomik/discordpp, v0.0.0)" } }, { });
		
		if (gateway_request.contains("url")) {
			if (gateway_request["session_start_limit"]["remaining"].get<int>() == 0) {
				throw std::runtime_error{ "GATEWAY ERROR: Maximum start limit reached" };
			}

			websocket_endpoint = gateway_request["url"];
			
			// Bind events
			std::thread bind_event_thread{ &Bot::BindEvents, this };

			ix::initNetSystem();

			web_socket.setUrl(websocket_endpoint);
			web_socket.disableAutomaticReconnection();

			// Set on_websocket_callback for all packets received from the websocket
			web_socket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) { OnWebsocketCallback(msg); });

			// Set certificate
			ix::SocketTLSOptions tls_options = ix::SocketTLSOptions();
			tls_options.certFile = "ca-chain.cert.pem";
			web_socket.setTLSOptions(tls_options);


			// Wait for the events to get binded
			bind_event_thread.join();

			// Connect to discord server
			web_socket.start();
		} else {
			throw std::runtime_error{ "Improper token, failed to connect to discord gateway!" };
		}
	}

	void Bot::OnWebsocketCallback(const ix::WebSocketMessagePtr& msg) {
		if (msg->type == ix::WebSocketMessageType::Open) {
			std::cout << "Websocket connected" << std::endl;
			// Send an identify packet to the websocket so we can actually do things
			web_socket.sendText(GetIdentifyPacket());
			websocket_disconnected = false;
		}
		else if (msg->type == ix::WebSocketMessageType::Close) {
			std::cout << "Client disconnected (" << msg->closeInfo.code << ", " << msg->closeInfo.reason << ")" << std::endl;
			websocket_disconnected = true;
		}
		else if (msg->type == ix::WebSocketMessageType::Error) {
			std::cout << "Error ! " << msg->errorInfo.reason << std::endl;
		}
		else if (msg->type == ix::WebSocketMessageType::Message) {
			nlohmann::json result = nlohmann::json::parse(msg->str);
			OnWebsocketPacket(result);
		}
		else {
			std::cout << "Known message sent" << std::endl;
		}
	}

	void Bot::OnWebsocketPacket(nlohmann::json result) {
		// Switch on the op code of the packet, else send it to the event handler
		switch (result["op"].get<int>()) {
		case (hello):
			hello_packet = result;
			websocket_ready = true;
			heartbeat_thread = std::thread{ &Bot::HandleHeartbeat, this };
			break;
		case (heartbeat_ack):
			heartbeat_acked = true;
			break;
		default:
			std::string event_name = result["t"];

			// If the event doesn't have its name specified, find it in the event map and send that found name to the event handler
			/*auto possible_event = std::find_if(internal_event_map.begin(), internal_event_map.end(), [&result](auto&& p) { return p.second == result["op"].get<int>(); });
			if (possible_event != internal_event_map.end()) {
				event_name = possible_event->first;
			}*/

			HandleDiscordEvent(result, event_name);
			break;
		}
		packet_counter++;
	}

	void Bot::HandleDiscordEvent(nlohmann::json const j, std::string event_name) {
		std::cout << "Handling discord event (" << event_name << ")" << std::endl;

		const nlohmann::json data = j["d"];
		websocket_last_sequence = (j.contains("s") && j["s"].is_number()) ? j["s"].get<int>() : -1;

		if (internal_event_map.find(event_name) != internal_event_map.end()) {
			if (websocket_ready) {
				internal_event_map[event_name](data);
			} else {
				futures.push_back(std::async(std::launch::async, internal_event_map[event_name], data));
			}
		}
	}

	void Bot::HandleHeartbeat() {
		while (!websocket_disconnected) {
			nlohmann::json data = { { "op", 3 }, { "d", nullptr } };
			if (websocket_last_sequence != -1) {
				data["d"] = websocket_last_sequence;
			}

			web_socket.sendText(data.dump());
			heartbeat_acked = false;

			// Wait for the required heartbeat interval and while waiting it should be acked.
			std::this_thread::sleep_for(std::chrono::milliseconds(hello_packet["d"]["heartbeat_interval"].get<int>() - 5));
			if (!heartbeat_acked) {
				std::cout << "HEARTBEAT WASN'T ACKED!" << std::endl;
				disconnected = true;
			}
		}
	}

	std::string Bot::GetIdentifyPacket() {
		nlohmann::json obj = { { "op", 2 },
							   { "d",
								 { { "token", token },
								   { "properties",
									 { { "$os", GetOsName() },
									   { "$browser", "DiscordPP" },
									   { "$device", "DiscordPP" } } },
								   { "compress", false },
								   { "large_threshold", 250 } } } };
		return obj.dump();
	}

	void Bot::ReadyEvent(nlohmann::json) {

	}

	void Bot::HelloEvent(nlohmann::json) {

	}
}