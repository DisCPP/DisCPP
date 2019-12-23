#include "bot.h"
#include "utils.h"

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
		
		//WebSocketStart();
		//websocket_thread = std::thread{ &Bot::WebSocketStart, this };
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
	struct PerSocketData {
	};

	void Bot::WebSocketStart() {
		nlohmann::json gateway_request = SendGetRequest(Endpoint("/gateway/bot"), { {"Authorization", Format("Bot %", token) }, { "User-Agent", "DiscordBot (https://github.com/seanomik/discordpp, v0.0.0)" } }, { });
		
		if (gateway_request.contains("url")) {
			if (gateway_request["session_start_limit"]["remaining"].get<int>() == 0) {
				throw std::runtime_error{ "GATEWAY ERROR: Maximum start limit reached" };
			}

			gateway_endpoint = gateway_request["url"];
			
			BindEvents();

			try {
				websocket.set_access_channels(websocketpp::log::alevel::all);
				websocket.clear_access_channels(websocketpp::log::alevel::frame_payload);
				websocket.set_error_channels(websocketpp::log::elevel::all);

				websocket.init_asio();

				websocket.set_message_handler(std::bind(&Bot::OnWebSocketPacket, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
				websocket.set_tls_init_handler([this](websocketpp::connection_hdl) {
					return websocketpp::lib::make_shared<websocketpp::lib::asio::ssl::context>(websocketpp::lib::asio::ssl::context::tlsv1);
				});

				websocketpp::lib::error_code ec;
				client::connection_ptr websocket_connection = websocket.get_connection(gateway_endpoint, ec);
				if (ec) {
					std::cout << "could not create connection because: " << ec.message() << std::endl;
					return;
				}

				websocket.connect(websocket_connection);

				websocket.run();
				disconnected = false;
			} catch (websocketpp::exception const& e) {
				std::cout << e.what() << std::endl;
			}
		} else {
			throw std::runtime_error{ "Improper token, failed to connect to discord gateway!" };
		}
	}

	void Bot::OnWebSocketPacket(websocketpp::connection_hdl hdl, client::message_ptr msg) {
		std::cout << "Packet recieved: " << msg->get_payload() << std::endl;

		nlohmann::json result = nlohmann::json::parse(msg->get_payload());
		// Switch on the op code of the packet, else send it to the event handler
		switch (result["op"].get<int>()) {
		case (hello): {
			hello_packet = result;
			websocket_connection->send(GetIdentifyPacket().dump()); // Crashes the application with a "read access exception"
			break;
		} case (heartbeat_ack):
			heartbeat_acked = true;
			break;
		default:
			HandleDiscordEvent(result, result["t"]);
			break;
		}
		packet_counter++;
	}

	void Bot::HandleDiscordEvent(nlohmann::json const j, std::string event_name) {
		std::cout << "Handling discord event (" << event_name << ")" << std::endl;

		const nlohmann::json data = j["d"];
		last_sequence = (j.contains("s") && j["s"].is_number()) ? j["s"].get<int>() : -1;

		if (internal_event_map.find(event_name) != internal_event_map.end()) {

			if (ready) {
				internal_event_map[event_name](data);
			} else {
				futures.push_back(std::async(std::launch::async, internal_event_map[event_name], data));
			}
		}
	}

	void Bot::HandleHeartbeat() {
		while (!disconnected) {
			nlohmann::json data = { { "op", 3 }, { "d", nullptr } };
			if (last_sequence != -1) {
				data["d"] = last_sequence;
			}

			websocket_connection->send(data.dump());
			heartbeat_acked = false;

			// Wait for the required heartbeat interval and while waiting it should be acked.
			std::this_thread::sleep_for(std::chrono::milliseconds(hello_packet["d"]["heartbeat_interval"].get<int>() - 5));
			if (!heartbeat_acked) {
				std::cout << "HEARTBEAT WASN'T ACKED!" << std::endl;
				disconnected = true;
			}
		}
	}

	nlohmann::json Bot::GetIdentifyPacket() {
		nlohmann::json obj = { { "op", 2 },
							   { "d",
								 { { "token", token },
								   { "properties",
									 { { "$os", GetOsName() },
									   { "$browser", "discordpp" },
									   { "$device", "discordpp" } } },
								   { "compress", false },
								   { "large_threshold", 250 } } } };
		return obj;
	}

	void Bot::ReadyEvent(nlohmann::json) {
		std::cout << "READY EVENT" << std::endl;
		heartbeat_thread = std::thread{ &Bot::HandleHeartbeat, this };
		ready = true;
	}

	void Bot::HelloEvent(nlohmann::json) {
		std::cout << "HELLO EVENT" << std::endl;
	}
}