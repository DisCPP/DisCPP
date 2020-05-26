#ifndef DISCPP_BOT_H
#define DISCPP_BOT_H

#define RAPIDJSON_HAS_STDSTRING 1

#include <string>
#include <future>
#include <string_view>
#include <optional>
#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <ixwebsocket/IXWebSocket.h>

#include "channel.h"
#include "message.h"
#include "member.h"
#include "guild.h"
#include "log.h"
#include "settings.h"

namespace discpp {
	class Role;
	class User;
	class Presence;
	class ClientConfig;

	class ClientUser : public User {
	public:
		ClientUser() = default;
		ClientUser(snowflake id) : User(id) {}
		ClientUser(rapidjson::Document & json);

		std::vector<Connection> GetUserConnections();
		ClientUserSettings GetSettings();
		void ModifySettings(ClientUserSettings& settings);

		ClientUserSettings settings;
		bool mfa_enabled;
		std::string locale;
		bool verified;
		std::string email;
	};

	class UserRelationship {
	private:
	    int type;
	public:
        UserRelationship() = default;
        UserRelationship(rapidjson::Document& json);

        bool IsFriend();
        bool IsBlocked();

        discpp::snowflake id;
        std::string nickname;
        discpp::User user;
	};

	class Client {
	public:
		std::string token; /**< Token for the current client. */
		ClientConfig* config; /**< Configuration for the current bot. */

		discpp::ClientUser client_user; /**< discpp::User object representing current user. */
		discpp::Logger* logger; /**< discpp::Logger object representing current logger. */

		//std::unordered_map<snowflake, std::shared_ptr<Channel>> channels; /**< List of channels the current bot can access. */
		std::unordered_map<snowflake, std::shared_ptr<Member>> members; /**< List of members the current bot can access. */
		std::unordered_map<snowflake, std::shared_ptr<Guild>> guilds; /**< List of guilds the current bot can access. */
		std::unordered_map<snowflake, std::shared_ptr<Message>> messages; /**< List of messages the current bot can access. */
        std::unordered_map<discpp::snowflake, discpp::DMChannel> private_channels; /**< List of dm channels the current client can access. */

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

		Client(std::string token, ClientConfig* config);
		int Run();
		void CreateWebsocketRequest(rapidjson::Document& json, std::string message = "");
		void SetCommandHandler(std::function<void(discpp::Client*, discpp::Message)> command_handler);
		void DisconnectWebsocket();
		void StopClient();
		void ReconnectToWebsocket();

		// Discord based methods.
		void AddFriend(discpp::User user);
		void RemoveFriend(discpp::User user);
        std::unordered_map<discpp::snowflake, discpp::UserRelationship> GetRelationships();
        std::shared_ptr<discpp::Guild> GetGuild(snowflake guild_id);
        discpp::User ModifyCurrentUser(std::string username, discpp::Image avatar);
        void LeaveGuild(discpp::Guild& guild);
        void UpdatePresence(discpp::Presence& activity);
		discpp::User ReqestUserIfNotCached(discpp::snowflake id);
        std::vector<discpp::User::Connection> GetBotUserConnections();
        discpp::Channel GetChannel(discpp::snowflake id);
        discpp::DMChannel GetDMChannel(discpp::snowflake id);
        std::unordered_map<discpp::snowflake, discpp::DMChannel> GetUserDMs();
        // discpp::Channel CreateGroupDM(std::vector<discpp::User> users); // Deprecated and will not be shown in the discord client.

		bool user_mfa_enabled;
		std::string user_locale;
		bool user_verified;

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

            {
                std::lock_guard<std::mutex> futures_guard(futures_mutex);
                futures.push_back(std::async(std::launch::async, func, std::forward<T>(args)...));
            }
		}
	private:
		friend class EventDispatcher;
		bool ready = false;
		bool disconnected = true;
		bool reconnecting = false;
		bool stay_disconnected = false;
		bool run = true;

        std::vector<std::future<void>> futures;

		std::string session_id;
		std::string gateway_endpoint;

		rapidjson::Document hello_packet;

		std::thread heartbeat_thread;
		std::thread future_loop_thread;

		std::mutex websocket_client_mutex;
		std::mutex futures_mutex;

		ix::WebSocket websocket;

		bool heartbeat_acked;
		int last_sequence_number;
		long long packet_counter;

		int message_cache_count;

		// Websocket Methods
		void WebSocketStart();
		void OnWebSocketListen(const ix::WebSocketMessagePtr& msg);
		void OnWebSocketPacket(rapidjson::Document& result);
		void HandleDiscordDisconnect(const ix::WebSocketMessagePtr& msg);
		void HandleHeartbeat();
		rapidjson::Document GetIdentifyPacket();

		// Commands
		std::function<void(discpp::Client*, discpp::Message)> fire_command_method;
    };
}

#endif