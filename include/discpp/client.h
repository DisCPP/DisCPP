#ifndef DISCPP_BOT_H
#define DISCPP_BOT_H

#include <memory>
#include <string>
#include <future>
#include <string_view>
#include <vector>

#include <ixwebsocket/IXWebSocket.h>

#include "user.h"
#include "settings.h"
#include "channel.h"

namespace discpp {
	class Role;
	class Presence;
	class ClientConfig;
	class Member;
	class Guild;
	class Message;
	class Logger;
	class Image;
	class EventHandler;
	class Cache;

	class ClientUser : public User {
	public:
		ClientUser() = default;
		ClientUser(discpp::Client* client, const Snowflake& id) : User(client, id) {}
		ClientUser(discpp::Client* client, rapidjson::Document & json);

        /**
         * @brief Get all connections of this user.
         *
         * ```cpp
         *      std::vector<discpp::User::Connection> connections = client->GetUserConnections();
         * ```
         *
         * @return std::vector<discpp::User::Connection>
         */
        std::vector<Connection> GetUserConnections();
		ClientUserSettings GetSettings();
		void ModifySettings(ClientUserSettings& user_settings);

		ClientUserSettings settings;
		bool mfa_enabled;
		std::string locale;
		bool verified;
		std::string email;
        discpp::specials::NitroSubscription premium_type; /**< The type of Nitro subscription on a user's account. */
	};

	class UserRelationship {
	private:
	    int type;
	public:
        UserRelationship() = default;
        UserRelationship(discpp::Client* client, rapidjson::Document& json);

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

        discpp::Snowflake id;
        std::string nickname;
        discpp::User user;
	};

	class Shard;
    class CommandHandler;

	class Client {
	    friend class Shard;
	public:
		std::string token; /**< Token for the current client. */
        ClientConfig& config; /**< Configuration for the current client. */

        discpp::ClientUser client_user; /**< discpp::User object representing current user. */
        std::unique_ptr<discpp::Logger> logger; /**< discpp::Logger object representing current logger. */

        std::unique_ptr<discpp::Cache> cache; /**< Bot cache. Stores members, channels, guilds, etc. */

        std::unique_ptr<EventHandler> event_handler; /**< Event handler. For registering event listeners, and dispatching them. */
		std::unique_ptr<CommandHandler> command_handler; /**< Command handler. For registering commands. */
        std::vector<std::unique_ptr<Shard>> shards;

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
		Client(const std::string& token, ClientConfig& config);

		~Client();

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
         * @brief Change the command handler.
         *
         * This is used in case you wanted to add functionality to the command handler.
         *
         * ```cpp
         *      client->SetCommandHandler([](discpp::Shard& shard, discpp::Message& message) { my_discpp_bot::command_handler::FireCommand(shard, message); });
         * ```
         *
         * @param[in] command_handler The method that will handle commands from a user.
         *
         * @return void
         */
		void SetCommandHandler(const std::function<void(discpp::Shard&, discpp::Message&)>& command_handler);

        /**
         * @brief Stop the client.
         *
         * This will cause the thread being blocked by discpp::Client::Run() to no longer be blocked.
         *
         * @return void
         */
		void StopClient();

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
         * @return std::unordered_map<discpp::Snowflake, discpp::UserRelationship>
         */
        std::unordered_map<discpp::Snowflake, discpp::UserRelationship> GetRelationships();

        /**
         * @brief Modify the client's user.
         *
         * ```cpp
         *      discpp::User user = client.ModifyCurrent("New bot name!", new_avatar);
         * ```
         *
         * @param[in] username The new username.
         * @param[in] avatar The new avatar.
         *
         * @return discpp::User
         */
        discpp::User ModifyCurrentUser(const std::string& username, discpp::Image& avatar);

        /**
         * @brief Leave the guild.
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
         * @brief Get all DM's for this user. Only supports user tokens!
         *
         * @return std::vector<discpp::User::Connection>
         */
        std::unordered_map<discpp::Snowflake, discpp::Channel> GetUserDMs();

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

            {
                std::lock_guard<std::mutex> futures_guard(futures_mutex);
                futures.push_back(std::async(std::launch::async, func, std::forward<T>(args)...));
            }
		}

        /**
         * @brief Get an instance of Client. Mainly used internally.
         *
         * @param[in] id The instance id of the client you're looking for.
         *
         * @return Client*
         */
		static Client* GetInstance(uint8_t id);

        /**
         * @brief Get the instance id of the client. Mainly used internally.
         *
         * @return uint8_t
         */
		uint8_t GetInstanceID();
	private:
		friend class Shard;
        friend class EventDispatcher;
		bool stay_disconnected = false;
		bool run = true;

        std::vector<std::future<void>> futures;

		std::mutex futures_mutex;

		int message_cache_count;

		static uint8_t next_instance_id;
		static std::map<uint8_t, Client*> client_instances;

		uint8_t my_instance_id;

		// The method to run to fire commands.
		std::function<void(discpp::Shard&, discpp::Message&)> fire_command_method;

        class HeartbeatWaiter { // For explanation, go to https://stackoverflow.com/a/29775639
        public:
            // returns false if killed:
            template<class R, class P>
            bool WaitFor( std::chrono::duration<R,P> const& time ) const {
                std::unique_lock<std::mutex> lock(m);
                return !cv.wait_for(lock, time, [&]{ return terminate; });
            }

            void Kill() {
                std::unique_lock<std::mutex> lock(m);
                terminate = true; // should be modified inside mutex lock
                cv.notify_all(); // it is safe, and *sometimes* optimal, to do this outside the lock
            }

            // I like to explicitly delete/default special member functions:
            HeartbeatWaiter() = default;
            HeartbeatWaiter(HeartbeatWaiter&&) = delete;
            HeartbeatWaiter(HeartbeatWaiter const&) = delete;
            HeartbeatWaiter& operator=(HeartbeatWaiter&&) = delete;
            HeartbeatWaiter& operator=(HeartbeatWaiter const&) = delete;
        private:
            mutable std::condition_variable cv;
            mutable std::mutex m;
            bool terminate = false;
        };
    };

    class Shard {
    public:
        /**
         * @brief Send a request to the websocket.
         *
         * Be cautious with this as it will close the websocket connection if the packet is invalid.
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

        enum Opcode : int {
            DISPATCH = 0,				// Receive
            HEARTBEAT = 1,				// Send/Receive
            IDENTIFY = 2,				// Send
            STATUS_UPDATE = 3,			// Send
            VOICE_STATE_UPDATE = 4,		// Send
            RESUME = 6,					// Send
            RECONNECT = 7,				// Receive
            REQUEST_GUILD_MEMBERS = 8,	// Send
            INVALID_SESSION = 9,		// Receive
            HELLO = 10,					// Receive
            HEARTBEAT_ACK = 11			// Receive
        };

        const int id;
        Client& client;
    private:
        friend class Client;
        friend class EventDispatcher;

        Shard(Client& client, int id, std::string endpoint) : client(client), id(id), gateway_endpoint(endpoint) {}

        std::string session_id;
        std::string gateway_endpoint;

        rapidjson::Document hello_packet;

        std::thread heartbeat_thread;

        ix::WebSocket websocket;

        discpp::Client::HeartbeatWaiter heartbeat_waiter;

        std::atomic<bool> ready = false;
        std::atomic<bool> disconnected = true;
        std::atomic<bool> reconnecting = false;
        std::atomic<bool> heartbeat_acked;
        std::atomic<int> last_sequence_number = 0;

        void ReconnectToWebsocket();
        void DisconnectWebsocket();
        void WebSocketStart();
        void OnWebSocketListen(ix::WebSocketMessagePtr& msg);
        void OnWebSocketPacket(rapidjson::Document& result);
        void HandleDiscordDisconnect(const ix::WebSocketMessagePtr& msg);
        void HandleHeartbeat();
        std::unique_ptr<rapidjson::Document> GetIdentifyPacket();
    };
}

#endif