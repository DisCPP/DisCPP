#ifndef DISCORDPP_BOT_H
#define DISCORDPP_BOT_H

#include <string>
#include <future>
#include <string_view>
#include <optional>
#include <vector>

#include <nlohmann/json.hpp>

#include <cpprest/ws_client.h>

#include "channel.h"
#include "message.h"
#include "member.h"
#include "guild.h"
#include "event_func_type.h"


namespace discord {
	class Role;
	class User;
	class Activity;

	using namespace web::websockets::client;

	class Bot {
	public:
		std::string prefix;
		std::string token;

		discord::User bot_user;

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
		discord::User ModifyCurrentUser(std::string username);
		void LeaveGuild(discord::Guild guild);
		void UpdatePresence(discord::Activity activity);
		void CreateWebsocketRequest(nlohmann::json json);

		void SetCommandHandler(std::function<void(discord::Bot*, discord::Message)> command_handler);

		template <size_t discord_event, typename Func>
		void HandleEvent(Func&& func, int run_amount = -1) {
			/**
			 * @brief Register an event listener
			 *
			 * ```cpp
			 *      bot.HandleEvent<discord::events::ready>([&bot]() {
			 *  		std::cout << "Ready!" << std::endl
			 *  			<< "Logged in as: " << bot.bot_user.username << "#" << bot.bot_user.discriminator << std::endl
			 *  			<< "ID: " << bot.bot_user.id << std::endl
			 *  			<< "-----------------------------" << std::endl;
			 *		});
			 * ```
			 *
			 * @param[in] func The method that will run when the event is triggered.
			 * @param[in] run_amount The amount of times the method will run before it is removed from the internal event map so it wont run again.
			 *
			 * @return void
			 */

			std::get<discord_event>(discord_event_func_holder.tuple).push_back({ std::forward<Func>(func), { run_amount, 0 } });
		}

		template <typename FType, typename... T>
		void DoFunctionLater(FType&& func, T&&... args) {
			/**
			 * @brief Do a function async so it wont hold the bot up.
			 *
			 * ```cpp
			 *      bot.DoFunctionLater(method, this, message);
			 * ```
			 *
			 * @param[in] func The method that will run when the event is triggered.
			 * @param[in] args The arguments for the method.
			 *
			 * @return void
			 */

			futures.push_back(std::async(std::launch::async, func, std::forward<T>(args)...));
		}
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

		int message_cache_count = 10000;
		std::vector<discord::Message> messages;

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

		// This is here due to some issues with intellisense thinking there was something wrong with all of this.
#ifdef __INTELLISENSE__ 
		Events<> discord_event_func_holder;
#else
		Events<
			void(),																	   // READY
			void(),																	   // RESUMED
			void(),																	   // RECONNECT
			void(),																	   // INVALID_SESSION
			void(discord::Channel const),											   // CHANNEL_CREATE
			void(discord::Channel const),											   // CHANNEL_UPDATE
			void(discord::Channel const),											   // CHANNEL_DELETE
			void(discord::Channel const),											   // CHANNEL_PINS_UPDATE
			void(discord::Guild const),												   // GUILD_CREATE
			void(discord::Guild const),												   // GUILD_UPDATE
			void(discord::Guild const),												   // GUILD_DELETE
			void(discord::Guild const, discord::User const),						   // GUILD_BAN_ADD
			void(discord::Guild const, discord::User const),						   // GUILD_BAN_REMOVE
			void(discord::Guild const),												   // GUILD_EMOJIS_UPDATE
			void(discord::Guild const),												   // GUILD_INTEGRATIONS_UPDATE
			void(discord::Guild const, discord::Member const),						   // GUILD_MEMBER_ADD
			void(discord::Guild const, discord::User const),						   // GUILD_MEMBER_REMOVE
			void(discord::Member const),											   // GUILD_MEMBER_UPDATE
			void(),																	   // GUILD_MEMBERS_CHUNK
			void(discord::Role const),												   // GUILD_ROLE_CREATE
			void(discord::Role const),												   // GUILD_ROLE_UPDATE
			void(discord::Role const),												   // GUILD_ROLE_DELETE
			void(discord::Message const),											   // MESSAGE_CREATE
			void(discord::Message const),											   // MESSAGE_UPDATE
			void(discord::Message const),											   // MESSAGE_DELETE
			void(std::vector<discord::Message> const),							       // MESSAGE_DELETE_BULK
			void(discord::Message const, discord::Emoji const, discord::User const),   // MESSAGE_REACTION_ADD
			void(discord::Message const, discord::Emoji const, discord::User const),   // MESSAGE_REACTION_REMOVE
			void(discord::Message const),											   // MESSAGE_REACTION_REMOVE_ALL
			void(discord::User const),												   // PRECENSE_UPDATE
			void(discord::User const, discord::Channel const, int const),			   // TYPING_START (user, channel, timestamp)
			void(discord::User const),												   // USER_UPDATE
			void(nlohmann::json const),												   // VOICE_STATE_UPDATE (raw_json)
			void(nlohmann::json const),												   // VOICE_SERVER_UPDATE (raw_json)
			void(discord::Channel const)											   // WEBHOOKS_UPDATE
		> discord_event_func_holder;
#endif

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