#include "bot.h"
#include "utils.h"
#include "command_handler.h"
#include "guild.h"
#include "channel.h"
#include "message.h"
#include "member.h"
#include "role.h"
#include "activity.h"
#include "event_handler.h"

// Events includes: 
#include "events/channel_create_event.h"
#include "events/channel_delete_event.h"
#include "events/channel_pins_update_event.h"
#include "events/channel_update_event.h"
#include "events/guild_ban_add_event.h"
#include "events/guild_ban_remove_event.h"
#include "events/guild_create_event.h"
#include "events/guild_delete_event.h"
#include "events/guild_emojis_update_event.h"
#include "events/guild_integrations_update_event.h"
#include "events/guild_member_add_event.h"
#include "events/guild_member_remove_event.h"
#include "events/guild_member_update_event.h"
#include "events/guild_members_chunk_event.h"
#include "events/guild_role_create_event.h"
#include "events/guild_role_delete_event.h"
#include "events/guild_role_update_event.h"
#include "events/guild_update_event.h"
#include "events/invalid_session_event.h"
#include "events/message_bulk_delete_event.h"
#include "events/message_create_event.h"
#include "events/message_delete_event.h"
#include "events/message_reaction_add_event.h"
#include "events/message_reaction_remove_all_event.h"
#include "events/message_reaction_remove_event.h"
#include "events/message_update_event.h"
#include "events/presense_update_event.h"
#include "events/ready_event.h"
#include "events/reconnect_event.h"
#include "events/resumed_event.h"
#include "events/typing_start_event.h"
#include "events/user_update_event.h"
#include "events/voice_server_update_event.h"
#include "events/voice_state_update_event.h"
#include "events/webhooks_update_event.h"

#include <iostream>
#include <algorithm>
#include <vector>

namespace discord {
	Bot::Bot(std::string token, std::vector<std::string> prefixes, int logger_flags, std::string logger_path) : token(token), prefixes(prefixes) {
		/**
		 * @brief Constructs a discord::Bot object.
		 *
		 * ```cpp
		 *      discord::Bot bot(TOKEN, {"+", "bot "}, discord::logger_flags::ERROR_SEVERITY | discord::logger_flags::WARNING_SEVERITY, "log.txt");
		 * ```
		 *
		 * @param[in] token The discord token the bot needs to run.
		 * @param[in] prefixes The bot's prefixes that will be used for command handling.
		 * @param[in] logger_flags The flags that will be passed to the logger.
		 * @param[in] logger_path The file path for the logger, doesn't need one as it can just log to console/terminal instead.
		 *
		 * @return discord::Bot, this is a constructor.
		 */

		fire_command_method = std::bind(&discord::FireCommand, std::placeholders::_1, std::placeholders::_2);

		discord::globals::bot_instance = this;

		if (logger_path.empty()) {
			logger = discord::Logger(logger_flags);
		}
		else {
			logger = discord::Logger(logger_path, logger_flags);
		}
	}

	int Bot::Run() {
		/**
		 * @brief Executes the discord bot.
		 *
		 * ```cpp
		 *      discord::Bot bot(TOKEN, {"+", "bot ", discord::logger_flags::ERROR_SEVERITY | discord::logger_flags::WARNING_SEVERITY, "log.txt");
		 *		bot.Run();
		 * ```
		 *
		 * @return int, currently only returns zero.
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

	discord::Guild Bot::GetGuild(snowflake guild_id) {
		/**
		 * @brief Gets a discord::Guild from a guild id.
		 *
		 * This will throw a runtime exception if the guild is not found.
		 *
		 * ```cpp
		 *      discord::Guild guild = bot.GetGuild(583251190591258624);
		 * ```
		 *
		 * @param[in] guild_id The guild id of the guild you want to get.
		 *
		 * @return discord::Guild
		 */
		auto guild = std::find_if(discord::globals::bot_instance->guilds.begin(), discord::globals::bot_instance->guilds.end(), [guild_id](discord::Guild a) { return guild_id == a.id; });

		if (guild != discord::globals::bot_instance->guilds.end()) {
			return *guild;
		}
		throw std::runtime_error("Guild not found!");
	}

	discord::User Bot::ModifyCurrentUser(std::string username) {
		/**
		 * @brief Modify the bot's username.
		 *
		 * ```cpp
		 *      discord::User user = bot.ModifyCurrent("New bot name!");
		 * ```
		 *
		 * @param[in] username The new username.
		 *
		 * @return discord::User
		 */

		nlohmann::json result = SendPatchRequest(Endpoint("/users/@me"), DefaultHeaders(), 0, discord::RateLimitBucketType::GLOBAL);

		bot_user = discord::User(result);

		return bot_user;
	}

	void Bot::LeaveGuild(discord::Guild guild) {
		/**
		 * @brief Leave the guild
		 *
		 * ```cpp
		 *      bot.LeaveGuild(guild);
		 * ```
		 *
		 * @param[in] guild The guild the bot will be leaving.
		 *
		 * @return void
		 */

		SendDeleteRequest(Endpoint("/users/@me/guilds/%", guild.id), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
	}

	void Bot::UpdatePresence(discord::Activity activity) {
		/**
		 * @brief Updates the bot's presence.
		 *
		 * ```cpp
		 *      bot.UpdatePresence(activity);
		 * ```
		 *
		 * @param[in] activity The presence the bot will update to.
		 *
		 * @return void
		 */

		nlohmann::json payload = nlohmann::json({
			{"op", 3},
			{"d", activity.ToJson()}
			});

		CreateWebsocketRequest(payload);
	}

	void discord::Bot::CreateWebsocketRequest(nlohmann::json json) {
		/**
		 * @brief Send a request to the websocket.
		 *
		 * Be cautious with this as it may close the websocket connection if it is invalid.
		 *
		 * ```cpp
		 *      bot.CreateWebsocketRequest(request_json);
		 * ```
		 *
		 * @param[in] json The request to send to the websocket.
		 *
		 * @return void
		 */

		logger.Log(LogSeverity::SEV_DEBUG, "Sending gateway payload: " + json.dump());

		websocket_outgoing_message msg;
		msg.set_utf8_message(json.dump());
		websocket_client.send(msg);
	}

	void Bot::SetCommandHandler(std::function<void(discord::Bot*, discord::Message)> command_handler) {
		/**
		 * @brief Change the command handler.
		 *
		 * This is used in case you wanted to add functionality to the command handler.
		 *
		 * ```cpp
		 *      bot.SetCommandHandler(std::bind(&my_discord_bot::command_handler::HandleCommands, std::placeholders::_1, std::placeholders::_2));
		 * ```
		 *
		 * @param[in] command_handler The method that will handle commands from a user.
		 *
		 * @return void
		 */

		fire_command_method = command_handler;
	}

	void Bot::BindEvents() {
		internal_event_map["READY"] = std::bind(&Bot::ReadyEvent, this, std::placeholders::_1);
		internal_event_map["RESUMED"] = std::bind(&discord::Bot::ResumedEvent, this, std::placeholders::_1);
		internal_event_map["INVALID_SESSION"] = std::bind(&discord::Bot::InvalidSesionEvent, this, std::placeholders::_1);
		internal_event_map["CHANNEL_CREATE"] = std::bind(&discord::Bot::ChannelCreateEvent, this, std::placeholders::_1);
		internal_event_map["CHANNEL_UPDATE"] = std::bind(&discord::Bot::ChannelUpdateEvent, this, std::placeholders::_1);
		internal_event_map["CHANNEL_DELETE"] = std::bind(&discord::Bot::ChannelDeleteEvent, this, std::placeholders::_1);
		internal_event_map["CHANNEL_PINS_UPDATE"] = std::bind(&discord::Bot::ChannelPinsUpdateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_CREATE"] = std::bind(&discord::Bot::GuildCreateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_UPDATE"] = std::bind(&discord::Bot::GuildUpdateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_DELETE"] = std::bind(&discord::Bot::GuildDeleteEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_BAN_ADD"] = std::bind(&discord::Bot::GuildBanAddEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_BAN_REMOVE"] = std::bind(&discord::Bot::GuildBanRemoveEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_EMOJIS_UPDATE"] = std::bind(&discord::Bot::GuildEmojisUpdateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_INTEGRATIONS_UPDATE"] = std::bind(&discord::Bot::GuildIntegrationsUpdateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_MEMBER_ADD"] = std::bind(&discord::Bot::GuildMemberAddEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_MEMBER_REMOVE"] = std::bind(&discord::Bot::GuildMemberRemoveEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_MEMBER_UPDATE"] = std::bind(&discord::Bot::GuildMemberUpdateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_MEMBERS_CHUNK"] = std::bind(&discord::Bot::GuildMembersChunkEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_ROLE_CREATE"] = std::bind(&discord::Bot::GuildRoleCreateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_ROLE_UPDATE"] = std::bind(&discord::Bot::GuildRoleUpdateEvent, this, std::placeholders::_1);
		internal_event_map["GUILD_ROLE_DELETE"] = std::bind(&discord::Bot::GuildRoleDeleteEvent, this, std::placeholders::_1);
		internal_event_map["MESSAGE_CREATE"] = std::bind(&discord::Bot::MessageCreateEvent, this, std::placeholders::_1);
		internal_event_map["MESSAGE_UPDATE"] = std::bind(&discord::Bot::MessageUpdateEvent, this, std::placeholders::_1);
		internal_event_map["MESSAGE_DELETE"] = std::bind(&discord::Bot::MessageDeleteEvent, this, std::placeholders::_1);
		internal_event_map["MESSAGE_DELETE_BULK"] = std::bind(&discord::Bot::MessageDeleteBulkEvent, this, std::placeholders::_1);
		internal_event_map["MESSAGE_REACTION_ADD"] = std::bind(&discord::Bot::MessageReactionAddEvent, this, std::placeholders::_1);
		internal_event_map["MESSAGE_REACTION_REMOVE"] = std::bind(&discord::Bot::MessageReactionRemoveEvent, this, std::placeholders::_1);
		internal_event_map["MESSAGE_REACTION_REMOVE_ALL"] = std::bind(&discord::Bot::MessageReactionRemoveAllEvent, this, std::placeholders::_1);
		internal_event_map["PRESENCE_UPDATE"] = std::bind(&discord::Bot::PresenceUpdateEvent, this, std::placeholders::_1);
		internal_event_map["TYPING_START"] = std::bind(&discord::Bot::TypingStartEvent, this, std::placeholders::_1);
		internal_event_map["USER_UPDATE"] = std::bind(&discord::Bot::UserUpdateEvent, this, std::placeholders::_1);
		internal_event_map["VOICE_STATE_UPDATE"] = std::bind(&discord::Bot::VoiceStateUpdateEvent, this, std::placeholders::_1);
		internal_event_map["VOICE_SERVER_UPDATE"] = std::bind(&discord::Bot::VoiceServerUpdateEvent, this, std::placeholders::_1);
		internal_event_map["WEBHOOKS_UPDATE"] = std::bind(&discord::Bot::WebhooksUpdateEvent, this, std::placeholders::_1);
	}

	void Bot::WebSocketStart() {
		nlohmann::json gateway_request = SendGetRequest(Endpoint("/gateway/bot"), { {"Authorization", Format("Bot %", token) }, { "User-Agent", "DiscordBot (https://github.com/seanomik/discordpp, v0.0.0)" } }, {}, {});

		if (gateway_request.contains("url")) {
			logger.Log(LogSeverity::SEV_DEBUG, LogTextColor::WHITE + "Connecting to gateway...");

			if (gateway_request["session_start_limit"]["remaining"].get<int>() == 0) {
				logger.Log(LogSeverity::SEV_ERROR, LogTextColor::RED + "GATEWAY ERROR: Maximum start limit reached");
				throw std::runtime_error{ "GATEWAY ERROR: Maximum start limit reached" };
			}

			gateway_endpoint = gateway_request["url"];

			std::thread bindthread{ &Bot::BindEvents, this };

			utility::string_t stringt = utility::conversions::to_string_t(gateway_endpoint);
			websocket_client.connect(web::uri(stringt));
			websocket_client.set_message_handler(std::bind(&Bot::OnWebSocketPacket, this, std::placeholders::_1));
			websocket_client.set_close_handler(std::bind(&Bot::HandleDiscordDisconnect, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			disconnected = false;

			logger.Log(LogSeverity::SEV_INFO, LogTextColor::GREEN + "Connected to gateway!");
			bindthread.join();
		}
		else {
			logger.Log(LogSeverity::SEV_ERROR, LogTextColor::RED + "Improper token, failed to connect to discord gateway!");
			throw std::runtime_error("Improper token, failed to connect to discord gateway!");
		}
	}

	void Bot::HandleDiscordDisconnect(websocket_close_status close_status, utility::string_t reason, std::error_code error_code) {
		//std::cout << "Websocket was closed with error: 400" << error_code.value() << "!" << std::endl;
		logger.Log(LogSeverity::SEV_ERROR, LogTextColor::RED + "Websocket was closed with error: 400" + std::to_string(error_code.value()) + "!");
		throw std::runtime_error("Websocket was closed with error: 400" + std::to_string(error_code.value()) + "!");
	}

	void Bot::OnWebSocketPacket(websocket_incoming_message msg) {
		std::string packet_raw = msg.extract_string().get();

		nlohmann::json result = nlohmann::json::parse(packet_raw);
		logger.Log(LogSeverity::SEV_DEBUG, "Received payload: " + result.dump());

		switch (result["op"].get<int>()) {
		case (hello): {
			logger.Log(LogSeverity::SEV_DEBUG, "Sending gateway payload: " + GetIdentifyPacket().dump());

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
		const nlohmann::json data = j["d"];
		last_sequence_number = (j.contains("s") && j["s"].is_number()) ? j["s"].get<int>() : -1;

		if (internal_event_map.find(event_name) != internal_event_map.end()) {
			if (ready) {
				internal_event_map[event_name](data);
			}
			else {
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

			logger.Log(LogSeverity::SEV_DEBUG, "Sending heartbeat payload: " + data.dump());
			websocket_outgoing_message msg;
			msg.set_utf8_message(data.dump());
			websocket_client.send(msg);

			heartbeat_acked = false;

			// Wait for the required heartbeat interval, while waiting it should be acked from another thread.
			std::this_thread::sleep_for(std::chrono::milliseconds(hello_packet["d"]["heartbeat_interval"].get<int>() - 5));
			if (!heartbeat_acked) {
				logger.Log(LogSeverity::SEV_WARNING, LogTextColor::YELLOW + "Heartbeat wasn't acked, trying to reconnect...");
				disconnected = true;

				logger.Log(LogSeverity::SEV_DEBUG, "Sending gateway payload: " + discord::Format("{\"token\": \"%\", \"session_id\": \"%\", \"seq\": %}", token, session_id, last_sequence_number));
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

		nlohmann::json user_json = SendGetRequest(Endpoint("/users/@me"), DefaultHeaders(), {}, {});
		bot_user = discord::User(user_json);

		discord::DispatchEvent(discord::ReadyEvent());
	}

	void Bot::ResumedEvent(nlohmann::json result) {
		discord::DispatchEvent(discord::ResumedEvent());
	}

	void Bot::ReconectEvent(nlohmann::json result) {
		discord::DispatchEvent(discord::ReconnectEvent());
	}

	void Bot::InvalidSesionEvent(nlohmann::json result) {
		discord::DispatchEvent(discord::InvalidSessionEvent());
	}

	void Bot::ChannelCreateEvent(nlohmann::json result) {
		discord::Channel new_channel = discord::Channel(result, result["id"].get<snowflake>());
		channels.push_back(new_channel);

		discord::DispatchEvent(discord::ChannelCreateEvent(new_channel));
	}

	void Bot::ChannelUpdateEvent(nlohmann::json result) {
		discord::Channel new_channel = discord::Channel(result);
		std::replace_if(channels.begin(), channels.end(), [new_channel](discord::Channel a) { return new_channel.id == a.id; }, new_channel);

		discord::DispatchEvent(discord::ChannelUpdateEvent(new_channel));
	}

	void Bot::ChannelDeleteEvent(nlohmann::json result) {
		std::remove_if(channels.begin(), channels.end(), [&](discord::Channel& channel) { return channel.id == result["id"].get<snowflake>(); });

		discord::DispatchEvent(discord::ChannelDeleteEvent(discord::Channel(result)));
	}

	void Bot::ChannelPinsUpdateEvent(nlohmann::json result) {
		discord::Channel new_channel = discord::Channel(result["channel_id"].get<snowflake>());
		new_channel.last_pin_timestamp = GetDataSafely<std::string>(result, "last_pin_timestamp");
		new_channel.guild_id = result["guild_id"].get<snowflake>();

		std::replace_if(channels.begin(), channels.end(), [new_channel](discord::Channel a) { return new_channel.id == a.id; }, new_channel);

		discord::DispatchEvent(discord::ChannelPinsUpdateEvent(new_channel));
	}

	void Bot::GuildCreateEvent(nlohmann::json result) {
		snowflake guild_id = result["id"].get<snowflake>();
		discord::Guild guild(result);
		logger.LogToConsole(LogSeverity::SEV_INFO, LogTextColor::GREEN + "Connected to %", guild.name);
		guilds.push_back(guild);

		members.insert(members.end(), guild.members.begin(), guild.members.end());

		for (auto& channel : result["channels"]) {
			channels.push_back(discord::Channel(channel, guild_id));
		}

		discord::DispatchEvent(discord::GuildCreateEvent(guild));
	}

	void Bot::GuildUpdateEvent(nlohmann::json result) {
		discord::Guild guild(result);
		std::replace_if(guilds.begin(), guilds.end(), [guild](discord::Guild gild) { return gild.id == guild.id; }, guild);

		discord::DispatchEvent(discord::GuildUpdateEvent(guild));
	}

	void Bot::GuildDeleteEvent(nlohmann::json result) {
		discord::Guild guild;
		guild.id = result["id"].get<snowflake>();
		guild.unavailable = true;
		std::remove_if(guilds.begin(), guilds.end(), [guild](discord::Guild gild) {return gild.id == guild.id; });

		discord::DispatchEvent(discord::GuildDeleteEvent(guild));
	}

	void Bot::GuildBanAddEvent(nlohmann::json result) {
		discord::Guild guild(result["guild_id"].get<snowflake>());
		discord::User user(result["user"]);

		discord::DispatchEvent(discord::GuildBanAddEvent(guild, user));
	}

	void Bot::GuildBanRemoveEvent(nlohmann::json result) {
		discord::Guild guild(result["guild_id"].get<snowflake>());
		discord::User user(result["user"]);

		discord::DispatchEvent(discord::GuildBanRemoveEvent(guild, user));
	}

	void Bot::GuildEmojisUpdateEvent(nlohmann::json result) {
		discord::Guild guild(result["guild_id"].get<snowflake>());
		std::vector<discord::Emoji> emojis;
		for (auto emoji : result["emojis"]) {
			emojis.push_back({ emoji });
		}
		guild.emojis = emojis;
		std::replace_if(guilds.begin(), guilds.end(), [guild](discord::Guild gild) { return gild.id == guild.id; }, guild);

		discord::DispatchEvent(discord::GuildEmojisUpdateEvent(guild));
	}

	void Bot::GuildIntegrationsUpdateEvent(nlohmann::json result) {
		discord::DispatchEvent(discord::GuildIntegrationsUpdateEvent(discord::Guild(result["guild_id"].get<snowflake>())));
	}

	void Bot::GuildMemberAddEvent(nlohmann::json result) {
		discord::Guild guild(result["guild_id"].get<snowflake>());
		discord::Member member(result, guild.id);

		discord::DispatchEvent(discord::GuildMemberAddEvent(guild, member));
	}

	void Bot::GuildMemberRemoveEvent(nlohmann::json result) {
		discord::Guild guild(result["guild_id"].get<snowflake>());
		discord::Member member(result["user"]["id"].get<snowflake>());
		std::remove_if(members.begin(), members.end(), [member](discord::Member member) { return member.user.id == member.user.id; });

		discord::DispatchEvent(discord::GuildMemberRemoveEvent(guild, member));
	}

	void Bot::GuildMemberUpdateEvent(nlohmann::json result) {
		discord::Guild guild(result["guild_id"].get<snowflake>());
		discord::Member member = GetIf(guild.members, [result](discord::Member& member) { return member.user.id == result["user"]["id"]; });
		member.roles.clear();
		for (auto role : result["roles"]) {
			member.roles.push_back(discord::Role(role, guild));
		}
		if (result.contains("nick") && !result["nick"].is_null()) {
			member.nick = result["nick"];
		}

		discord::DispatchEvent(discord::GuildMemberUpdateEvent(guild, member));
	}

	void Bot::GuildMembersChunkEvent(nlohmann::json result) {
		discord::DispatchEvent(discord::GuildMembersChunkEvent());
	}

	void Bot::GuildRoleCreateEvent(nlohmann::json result) {
		discord::Role role(result["role"]);

		discord::DispatchEvent(discord::GuildRoleCreateEvent(role));
	}

	void Bot::GuildRoleUpdateEvent(nlohmann::json result) {
		discord::Role role(result["role"]);

		discord::DispatchEvent(discord::GuildRoleUpdateEvent(role));
	}

	void Bot::GuildRoleDeleteEvent(nlohmann::json result) {
		discord::Guild guild(result["guild_id"].get<snowflake>());
		discord::Role role = GetIf(guild.roles, [result](discord::Role& role) {return role.id == result["role_id"].get<snowflake>(); });
		std::replace_if(guild.roles.begin(), guild.roles.end(), [role](discord::Role& r) { return r.id == role.id; }, role);

		discord::DispatchEvent(discord::GuildRoleDeleteEvent(role));
	}

	void Bot::MessageCreateEvent(nlohmann::json result) {
		discord::Message message(result);
		if (messages.size() >= message_cache_count) {
			messages.erase(messages.begin());
		}
		messages.push_back(message);

		DoFunctionLater(fire_command_method, this, message);
		DispatchEvent(discord::MessageCreateEvent(message));
	}

	void Bot::MessageUpdateEvent(nlohmann::json result) {
		discord::Message message(result);
		if (messages.size() >= message_cache_count) {
			messages.erase(messages.begin());
		}
		std::replace_if(messages.begin(), messages.end(), [message](discord::Message& msg) { return msg.id == message.id; }, message);

		discord::DispatchEvent(discord::MessageUpdateEvent(message));
	}

	void Bot::MessageDeleteEvent(nlohmann::json result) {
		discord::Message message = GetIf(messages, [result](discord::Message& msg) { return msg.id == result["id"].get<snowflake>(); });
		std::remove_if(messages.begin(), messages.end(), [message](discord::Message& msg) { return msg.id == message.id; });

		discord::DispatchEvent(discord::MessageDeleteEvent(message));
	}

	void Bot::MessageDeleteBulkEvent(nlohmann::json result) {
		std::vector<discord::Message> msgs;
		for (auto id : result["ids"]) {
			discord::Message message = GetIf(messages, [id](discord::Message& msg) { return msg.id == id.get<snowflake>(); });
			message.channel = discord::Channel(result["channel_id"].get<snowflake>());
			if (result.contains("guild_id")) message.guild = discord::Guild(result["guild_id"].get<snowflake>());

			msgs.push_back(message);
		}

		auto ib = std::begin(msgs);
		auto iter = std::remove_if(std::begin(messages), std::end(messages),
			[&ib, &msgs](discord::Message x) -> bool {
				while (ib != std::end(msgs) && ib->id < x.id) ++ib;
				return (ib != std::end(msgs) && ib->id == x.id);
			});

		discord::DispatchEvent(discord::MessageBulkDeleteEvent(msgs));
	}

	void Bot::MessageReactionAddEvent(nlohmann::json result) {
		discord::Message message = GetIf(messages, [result](discord::Message& msg) { return msg.id == result["message_id"].get<snowflake>(); });
		discord::Channel channel = GetIf(channels, [result](discord::Channel& channel) { return channel.id == result["channel_id"].get<snowflake>(); });
		message.channel = channel;

		if (result.contains("guild_id")) {
			message.guild = discord::Guild(result["guild_id"].get<snowflake>());
			channel.guild_id = result["guild_id"].get<snowflake>();
		}

		discord::Emoji emoji(result["emoji"]);
		discord::User user(result["user_id"].get<snowflake>());

		discord::DispatchEvent(discord::MessageReactionAddEvent(message, emoji, user));
	}

	void Bot::MessageReactionRemoveEvent(nlohmann::json result) {
		discord::Message message = GetIf(messages, [result](discord::Message& msg) { return msg.id == result["message_id"].get<snowflake>(); });
		discord::Channel channel = GetIf(channels, [result](discord::Channel& channel) { return channel.id == result["channel_id"].get<snowflake>(); });
		message.channel = channel;

		if (result.contains("guild_id")) {
			channel.guild_id = result["guild_id"].get<snowflake>();
		}

		discord::Emoji emoji(result["emoji"]);
		discord::User user(result["user_id"].get<snowflake>());

		discord::DispatchEvent(discord::MessageReactionRemoveEvent(message, emoji, user));
	}

	void Bot::MessageReactionRemoveAllEvent(nlohmann::json result) {
		discord::Message message = GetIf(messages, [result](discord::Message& msg) { return msg.id == result["message_id"].get<snowflake>(); });
		discord::Channel channel = GetIf(channels, [result](discord::Channel& channel) { return channel.id == result["channel_id"].get<snowflake>(); });
		message.channel = channel;

		if (result.contains("guild_id")) {
			channel.guild_id = result["guild_id"].get<snowflake>();
		}

		discord::DispatchEvent(discord::MessageReactionRemoveAllEvent(message));
	}

	void Bot::PresenceUpdateEvent(nlohmann::json result) {
		discord::DispatchEvent(discord::PresenseUpdateEvent(discord::User(result["user"])));
	}

	void Bot::TypingStartEvent(nlohmann::json result) {
		discord::User user(result["user_id"].get<snowflake>());
		discord::Channel channel(result["channel_id"].get<snowflake>());
		if (result.contains("guild_id")) channel.guild_id = result["guild_id"].get<snowflake>();
		int timestamp = result["timestamp"].get<int>();

		discord::DispatchEvent(discord::TypingStartEvent(user, channel, timestamp));
	}

	void Bot::UserUpdateEvent(nlohmann::json result) {
		discord::User user(result);

		discord::DispatchEvent(discord::UserUpdateEvent(user));
	}

	void Bot::VoiceStateUpdateEvent(nlohmann::json result) {
		discord::DispatchEvent(discord::VoiceStateUpdateEvent(result));
	}

	void Bot::VoiceServerUpdateEvent(nlohmann::json result) {
		discord::DispatchEvent(discord::VoiceServerUpdateEvent(result));
	}

	void Bot::WebhooksUpdateEvent(nlohmann::json result) {
		discord::Channel channel(result["channel_id"].get<snowflake>());
		channel.guild_id = result["guild_id"].get<snowflake>();

		discord::DispatchEvent(discord::WebhooksUpdateEvent(channel));
	}
}