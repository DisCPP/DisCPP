#include "bot.h"
#include "utils.h"

#include <iostream>

namespace discord {
	Bot::Bot(std::string token, std::string prefix) : token(token), prefix(prefix) {
		
	}

	int Bot::Run() {
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

			gateway_endpoint = gateway_request["url"];
			
			//std::thread bindthread{ &Bot::BindEvents, this };
			BindEvents();

			utility::string_t stringt = utility::conversions::to_string_t(gateway_endpoint);
			websocket_client.connect(web::uri(stringt)).then([]() { std::cout << "Connected" << std::endl; });
			websocket_client.set_message_handler(std::bind(&Bot::OnWebSocketPacket, this, std::placeholders::_1));
			websocket_client.set_close_handler([](websocket_close_status close_status, utility::string_t reason, std::error_code error_code) {
				std::cout << "Websocket was closed with error: " << error_code << " - " << reason.c_str() << std::endl;
			});

			//bindthread.join();
		} else {
			throw std::runtime_error{ "Improper token, failed to connect to discord gateway!" };
		}
	}

	void Bot::OnWebSocketPacket(websocket_incoming_message msg) {
		std::cout << "Packet recieved: " << msg.extract_string().get() << std::endl;

		try {
			nlohmann::json result = nlohmann::json::parse(msg.extract_string().get());

			switch (result["op"].get<int>()) {
			case (hello): {
				hello_packet = result;
				websocket_outgoing_message identify_msg;
				identify_msg.set_utf8_message(GetIdentifyPacket().dump());
				websocket_client.send(identify_msg);
				break;
			} case (heartbeat_ack):
				heartbeat_acked = true;
				break;
			default:
				HandleDiscordEvent(result, result["t"]);
				break;
			}
		}
		catch (nlohmann::json::parse_error& e) {
			std::cout << "FAILED TO DO SOMETHING: " << e.what() << std::endl;
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

			//websocket_connection->send(data.dump());
			websocket_outgoing_message msg;
			msg.set_utf8_message(data.dump());
			websocket_client.send(msg);

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
		//heartbeat_thread = std::thread{ &Bot::HandleHeartbeat, this };
		ready = true;
	}

	void Bot::HelloEvent(nlohmann::json) {
		std::cout << "HELLO EVENT" << std::endl;
	}
}