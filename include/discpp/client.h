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

namespace discpp {
	class Role;
	class User;
	class Activity;
	class ClientConfig;

	class InvalidGuildException : public std::runtime_error {
	public: 
		InvalidGuildException() : std::runtime_error("Guild not found") {}
	};

	class StartLimitException : public std::runtime_error {
	public:
		StartLimitException() : std::runtime_error("Maximum start limit reached") {}
	};

    class ProhibitedEndpointException : public std::runtime_error {
    public:
        ProhibitedEndpointException(std::string msg) : std::runtime_error(msg) {}
    };

	class AuthenticationException : public std::runtime_error {
	public:
        AuthenticationException() : std::runtime_error("Invalid token, failed to connect to gateway") {}
	};

	class ClientUser : public User {
	public:
		ClientUser() = default;
		ClientUser(snowflake id) : User(id) {}
		ClientUser(rapidjson::Document & json);

		std::vector<Connection> GetUserConnections();

		bool mfa_enabled;
		std::string locale;
		bool verified;
		std::string email;
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

		std::vector<std::future<void>> futures; /**< List of events. */

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
		void GetFriends();
        std::shared_ptr<discpp::Guild> GetGuild(snowflake guild_id);
        discpp::User ModifyCurrentUser(std::string username, discpp::Image avatar);
        void LeaveGuild(discpp::Guild& guild);
        void UpdatePresence(discpp::Activity& activity);
		discpp::User ReqestUserIfNotCached(discpp::snowflake id);
        std::vector<discpp::Connection> GetBotUserConnections();
        discpp::Channel GetChannel(discpp::snowflake id);
        discpp::DMChannel GetDMChannel(discpp::snowflake id);
		std::vector<Connection> GetUserConnections();

		bool user_mfa_enabled;
		std::string user_locale;
		bool user_verified;
        // std::vector<discpp::Channel> GetUserDMs(); // Not supported by bots.
        // discpp::Channel CreateGroupDM(std::vector<discpp::User> users); // Deprecated and will not be shown in the discord client.

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
		friend class EventDispatcher;
		bool ready = false;
		bool disconnected = true;
		bool reconnecting = false;
		bool stay_disconnected = false;
		bool run = true;

		std::string session_id;
		std::string gateway_endpoint;

		rapidjson::Document hello_packet;

		std::thread heartbeat_thread;
		std::thread future_loop_thread;

		std::mutex websocket_client_mutex;

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