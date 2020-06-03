#ifndef DISCPP_BOT_H
#define DISCPP_BOT_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

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
		ClientUser(const snowflake& id) : User(id) {}
		ClientUser(rapidjson::Document & json);


        /**
         * @brief Get all connections of this user.
         *
         * ```cpp
         *      std::vector<discpp::User::Connection> conntections = client->GetUserConnections();
         * ```
         *
         * @return std::vector<discpp::User::Connection>
         */
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

        /**
         * @brief Returns if this relation is a friend.
         *
         * @return bool
         */
        bool IsFriend();

        /**
         * @brief Returns if this relation is a block.
         *
         * @return bool
         */
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
        std::unordered_map<discpp::snowflake, discpp::Channel> private_channels; /**< List of dm channels the current client can access. */

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


        /**
         * @brief Constructs a discpp::Bot object.
         *
         * ```cpp
         *      discpp::Bot bot(TOKEN, {"+", "bot "}, discpp::logger_flags::ERROR_SEVERITY | discpp::logger_flags::WARNING_SEVERITY, "log.txt");
         * ```
         *
         * @param[in] token The discpp token the bot needs to run.
         * @param[in] prefixes The bot's prefixes that will be used for command handling.
         * @param[in] logger_flags The flags that will be passed to the logger->
         * @param[in] logger_path The file path for the logger, doesn't need one as it can just log to console/terminal instead.
         *
         * @return discpp::Bot, this is a constructor.
         */
		Client(const std::string& token, ClientConfig* config);

        /**
         * @brief Executes the discpp bot.
         *
         * ```cpp
         *      discpp::Bot bot(TOKEN, {"+", "bot ", discpp::logger_flags::ERROR_SEVERITY | discpp::logger_flags::WARNING_SEVERITY, "log.txt");
         *		bot.Run();
         * ```
         *
         * @return int, currently only returns zero.
         */
		int Run();

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
         * @param[in] message The message to print to the debug log. If this is empty it will be set to default. (Default: "Sending gateway payload" + payload)
         *
         * @return void
         */
		void CreateWebsocketRequest(rapidjson::Document& json, const std::string& message = "");

        /**
         * @brief Change the command handler.
         *
         * This is used in case you wanted to add functionality to the command handler.
         *
         * ```cpp
         *      bot.SetCommandHandler(std::bind(&my_discpp_bot::command_handler::HandleCommands, std::placeholders::_1, std::placeholders::_2));
         * ```
         *
         * @param[in] command_handler The method that will handle commands from a user.
         *
         * @return void
         */
		void SetCommandHandler(const std::function<void(discpp::Client*, discpp::Message)>& command_handler);
		void DisconnectWebsocket();
		void StopClient();
		void ReconnectToWebsocket();

		// Discord based methods.

        /**
         * @brief Add a friend. Only supports user tokens!
         *
         * @return void
         */
		void AddFriend(const discpp::User& user);

        /**
         * @brief Remove a friend. Only supports user tokens!
         *
         * @return void
         */
		void RemoveFriend(const discpp::User& user);

        /**
         * @brief Get all friends. Only supports user tokens!
         *
         * @return std::unordered_map<discpp::snowflake, discpp::UserRelationship>
         */
        std::unordered_map<discpp::snowflake, discpp::UserRelationship> GetRelationships();

        /**
         * @brief Gets a discpp::Guild from a guild id.
         *
         * This will throw a runtime exception if the guild is not found.
         *
         * ```cpp
         *      std::shared_ptr<discpp::Guild> guild = bot.GetGuild(583251190591258624);
         * ```
         *
         * @param[in] guild_id The guild id of the guild you want to get.
         *
         * @return std::shared_ptr<discpp::Guild>
         */
        std::shared_ptr<discpp::Guild> GetGuild(const snowflake& guild_id);


        /**
         * @brief Modify the bot's username.
         *
         * ```cpp
         *      discpp::User user = bot.ModifyCurrent("New bot name!", new_avatar);
         * ```
         *
         * @param[in] username The new username.
         * @param[in] avatar The new avatar.
         *
         * @return discpp::User
         */
        discpp::User ModifyCurrentUser(const std::string& username, discpp::Image& avatar);

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
        void LeaveGuild(const discpp::Guild& guild);

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
        void UpdatePresence(discpp::Presence& activity);

        /**
         * @brief Get a user.
         *
         * ```cpp
         *      bot.GetUser("150312037426135041");
         * ```
         *
         * @param[in] id The user to get with this id.
         *
         * @return discpp::User
         */
		discpp::User ReqestUserIfNotCached(const discpp::snowflake& id);

        /**
         * @brief Get the bot's user connections.
         *
         * ```cpp
         *      bot.GetBotUserConnections();
         * ```
         *
         * @return std::vector<discpp::User::Connection>
         */
        std::vector<discpp::User::Connection> GetBotUserConnections();

        /**
         * @brief Gets a channel from guild cache and private caches.
         *
         * @return discpp::Channel
         */
        discpp::Channel GetChannel(const discpp::snowflake& id);

        /**
         * @brief Get a DM channel with id
         *
         * @return discpp::Channel
         */
        discpp::Channel GetDMChannel(const discpp::snowflake& id);

        /**
         * @brief Get all DM's for this user. Only supports user tokens!
         *
         * @return std::vector<discpp::User::Connection>
         */
        std::unordered_map<discpp::snowflake, discpp::Channel> GetUserDMs();

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