#ifndef DISCORDPP_BOT_H
#define DISCORDPP_BOT_H

#include <string>
#include <future>
#include <string_view>
#include <optional>

#include <nlohmann/json.hpp>

#include <cpprest/ws_client.h>

#include "channel.h"
#include "message.h"
#include "member.h"
#include "guild.h"

namespace discord {
	using namespace web::websockets::client;

	class Bot {
	public:
		std::string prefix;
		std::string token;

		std::vector<discord::Channel> channels;
		std::vector<discord::Member> members;
		std::vector<discord::Guild> guilds;

		enum packet_opcode : int {
			dispatch = 0,				// Receive
			heartbeat = 1,				// Send/Receive
			identify = 2,				// Send
			status_update = 3,			// Send
			voice_state_update = 4,		// Send
			resume = 6,					// Send
			reconnect = 7,				// Receive
			request_guild_members = 8,	// Send
			invalid_session = 9,		// Receive
			hello = 10,					// Receive
			heartbeat_ack = 11			// Receive
		};

		Bot(std::string token, std::string prefix);
		int Run();
		discord::Guild GetGuild(snowflake guild_id);

		void SetCommandHandler(std::function<void(discord::Bot*, discord::Message)> command_handler);

		template <size_t discord_event, typename FType>
		void HandleEvent(FType&& func) {
			//std::get<discord_event>(func_hol)
		}

		template <typename FType, typename... T>
		void DoFunctionLater(FType&& func, T&&... args) {
			futures.push_back(std::async(std::launch::async, func, std::forward<T>(args)...));
		}

		//void RegisterCommand(std::string const& command_name, std::string const& command_desc, std::vector<std::string> params, std::function<void(discord::Context)> function, std::vector<std::function<bool(discord::Context)>> requirements);
	private:
		bool ready = false;
		bool disconnected =true;
		std::string session_id;
		std::string gateway_endpoint;

		nlohmann::json hello_packet;

		std::thread heartbeat_thread;

		websocket_callback_client websocket_client;

		bool heartbeat_acked;
		int last_sequence_number;
		long long packet_counter;

		std::vector<std::future<void>> futures;
		std::unordered_map<std::string, std::function<void(nlohmann::json)>> internal_event_map;

		// Websocket Methods
		void WebSocketStart();
		void OnWebSocketPacket(websocket_incoming_message msg);
		void HandleDiscordEvent(nlohmann::json const j, std::string event_name);
		void HandleDiscordDisconnect(websocket_close_status close_status, utility::string_t reason, std::error_code error_code);
		void HandleHeartbeat();
		nlohmann::json GetIdentifyPacket();

		void BindEvents();

		// Commands
		std::function<void(discord::Bot*, discord::Message)> fire_command_method;

		// Events
		void ReadyEvent(nlohmann::json result);
		void ResumedEvent(nlohmann::json result);
		void ReconectEvent(nlohmann::json result);
		void InvalidSesionEvent(nlohmann::json result);
		void ChannelCreateEvent(nlohmann::json result);
		void ChannelUpdateEvent(nlohmann::json result);
		void ChannelDeleteEvent(nlohmann::json result);
		void ChannelPinsUpdateEvent(nlohmann::json result);
		void GuildCreateEvent(nlohmann::json result);
		void GuildUpdateEvent(nlohmann::json result);
		void GuildDeleteEvent(nlohmann::json result);
		void GuildBanAddEvent(nlohmann::json result);
		void GuildBanRemoveEvent(nlohmann::json result);
		void GuildEmojisUpdateEvent(nlohmann::json result);
		void GuildIntegrationsUpdateEvent(nlohmann::json result);
		void GuildMemberAddEvent(nlohmann::json result);
		void GuildMemberRemoveEvent(nlohmann::json result);
		void GuildMemberUpdateEvent(nlohmann::json result);
		void GuildMembersChunkEvent(nlohmann::json result);
		void GuildRoleCreateEvent(nlohmann::json result);
		void GuildRoleUpdateEvent(nlohmann::json result);
		void GuildRoleDeleteEvent(nlohmann::json result);
		void MessageCreateEvent(nlohmann::json result);
		void MessageUpdateEvent(nlohmann::json result);
		void MessageDeleteEvent(nlohmann::json result);
		void MessageDeleteBulkEvent(nlohmann::json result);
		void MessageReactionAddEvent(nlohmann::json result);
		void MessageReactionRemoveEvent(nlohmann::json result);
		void MessageReactionRemoveAllEvent(nlohmann::json result);
		void PresenceUpdateEvent(nlohmann::json result);
		void TypingStartEvent(nlohmann::json result);
		void UserUpdateEvent(nlohmann::json result);
		void VoiceStateUpdateEvent(nlohmann::json result);
		void VoiceServerUpdateEvent(nlohmann::json result);
		void WebhooksUpdateEvent(nlohmann::json result);
	};
}

#endif