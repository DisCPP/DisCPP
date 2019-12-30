#include "..\include\bot.h"
#include "..\include\bot.h"
#include "..\include\bot.h"
#include "..\include\bot.h"
#include "bot.h"
#include "utils.h"
#include "command_handler.h"

#include <iostream>

namespace discord {
	Bot::Bot(std::string token, std::string prefix) : token(token), prefix(prefix) {
		fire_command_method = std::bind(&discord::FireCommand, std::placeholders::_1, std::placeholders::_2);

		discord::globals::bot_instance = this;
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

	discord::Guild Bot::GetGuild(snowflake guild_id) {
		auto guild = std::find_if(discord::globals::bot_instance->guilds.begin(), discord::globals::bot_instance->guilds.end(), [guild_id](discord::Guild a) { return guild_id == a.id; });

		if (guild != discord::globals::bot_instance->guilds.end()) {
			return *guild;
		}
		throw std::runtime_error("Guild not found!");
	}

	discord::User Bot::GetCurrentUser() {
		nlohmann::json result = SendGetRequest(Endpoint("/users/@me"), DefaultHeaders(), {}, {});
		return discord::User(result);
	}

	void Bot::LeaveGuild(discord::Guild guild) {
		SendDeleteRequest(Endpoint("/users/@me/guilds/%", guild.id), DefaultHeaders());
	}

	void Bot::SetCommandHandler(std::function<void(discord::Bot*, discord::Message)> command_handler) {
		fire_command_method = command_handler;
	}
	
	void Bot::BindEvents() {
		internal_event_map["READY"] = std::bind(&Bot::ReadyEvent, this, std::placeholders::_1);
		internal_event_map["CHANNEL_CREATE"] = std::bind(&Bot::ChannelCreateEvent, this, std::placeholders::_1);
		internal_event_map["MESSAGE_CREATE"] = std::bind(&Bot::MessageCreateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_CREATE"] = std::bind(&Bot::GuildCreateEvent, this, std::placeholders::_1);
	}

	void Bot::WebSocketStart() {
		nlohmann::json gateway_request = SendGetRequest(Endpoint("/gateway/bot"), { {"Authorization", Format("Bot %", token) }, { "User-Agent", "DiscordBot (https://github.com/seanomik/discordpp, v0.0.0)" } }, {},{});
		
		if (gateway_request.contains("url")) {
			if (gateway_request["session_start_limit"]["remaining"].get<int>() == 0) {
				throw std::runtime_error{ "GATEWAY ERROR: Maximum start limit reached" };
			}

			gateway_endpoint = gateway_request["url"];
			
			std::thread bindthread{ &Bot::BindEvents, this };

			utility::string_t stringt = utility::conversions::to_string_t(gateway_endpoint);
			websocket_client.connect(web::uri(stringt)).then([]() { std::cout << "Connected" << std::endl; });
			websocket_client.set_message_handler(std::bind(&Bot::OnWebSocketPacket, this, std::placeholders::_1));
			websocket_client.set_close_handler(std::bind(&Bot::HandleDiscordDisconnect, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			disconnected = false;

			bindthread.join();
		} else {
			//throw std::runtime_error{ "Improper token, failed to connect to discord gateway!" };
			std::cout << "Improper token, failed to connect to discord gateway!" << std::endl;
		}
	}

	void Bot::HandleDiscordDisconnect(websocket_close_status close_status, utility::string_t reason, std::error_code error_code) {
		std::cout << "Websocket was closed with error: " << error_code << " - " << reason.c_str() << std::endl;
	}

	void Bot::OnWebSocketPacket(websocket_incoming_message msg) {
		std::string packet_raw = msg.extract_string().get();
		//std::cout << "Packet recieved: " << packet_raw << std::endl;

		nlohmann::json result = nlohmann::json::parse(packet_raw);

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
		packet_counter++;
	}

	void Bot::HandleDiscordEvent(nlohmann::json const j, std::string event_name) {
		std::cout << "Handling discord event (" << event_name << ")" << std::endl;

		const nlohmann::json data = j["d"];
		last_sequence_number = (j.contains("s") && j["s"].is_number()) ? j["s"].get<int>() : -1;

		if (internal_event_map.find(event_name) != internal_event_map.end()) {
			if (ready) {
				internal_event_map[event_name](data);
			} else {
				futures.push_back(std::async(std::launch::async, internal_event_map[event_name], data));
			}
		}
	}

	void Bot::HandleHeartbeat() {
		while (true) {
			nlohmann::json data = { { "op", packet_opcode::heartbeat }, { "d", nullptr } };
			if (last_sequence_number != -1) {
				data["d"] = last_sequence_number;
			}

			websocket_outgoing_message msg;
			msg.set_utf8_message(data.dump());
			websocket_client.send(msg);

			heartbeat_acked = false;

			// Wait for the required heartbeat interval, while waiting it should be acked from another thread.
			std::this_thread::sleep_for(std::chrono::milliseconds(hello_packet["d"]["heartbeat_interval"].get<int>() - 5));
			if (!heartbeat_acked) {
				std::cout << "Heartbeat wasn't acked, trying to reconnect..." << std::endl;
				disconnected = true;

				websocket_outgoing_message resume_msg;
				resume_msg.set_utf8_message(discord::Format("{\"token\": \"%\", \"session_id\": \"%\", \"seq\": %}", token, session_id, last_sequence_number));
				websocket_client.send(resume_msg);
			}
		}
	}

	nlohmann::json Bot::GetIdentifyPacket() {
		nlohmann::json obj = { { "op", packet_opcode::identify },
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

	void Bot::ReadyEvent(nlohmann::json result) {
		heartbeat_thread = std::thread{ &Bot::HandleHeartbeat, this };
		ready = true;
		session_id = result["session_id"];
	}

	void Bot::ChannelCreateEvent(nlohmann::json result) {
		discord::Channel new_channel = discord::Channel(result, result["id"].get<snowflake>());
		channels.push_back(new_channel);
	}

	void Bot::MessageCreateEvent(nlohmann::json result) {
		discord::Message message(result);
		futures.push_back(std::async(std::launch::async, fire_command_method, this, message));
	}

	void Bot::GuildCreateEvent(nlohmann::json result) {
		snowflake guild_id = result["id"].get<snowflake>();

		for (auto& member : result["members"]) {
			members.push_back(discord::Member(member));
		}

		for (auto& channel : result["channels"]) {
			channels.push_back(discord::Channel(channel, guild_id));
		}

		guilds.push_back(discord::Guild(result));
	}
}