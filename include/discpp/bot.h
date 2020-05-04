#ifndef DISCPP_BOT_H
#define DISCPP_BOT_H

#include <string>
#include <future>
#include <string_view>
#include <optional>
#include <vector>

#include <nlohmann/json.hpp>

#include <ixwebsocket/IXWebSocket.h>

#include "channel.h"
#include "message.h"
#include "member.h"
#include "guild.h"
#include "log.h"
#include "bot_config.h"

namespace discpp {
	class Role;
	class User;
	class Activity;
	class BotConfig;

	class Bot {
	public:
		std::string token; /**< Token for the current bot */
		BotConfig config; /**< Configuration for the current bot */

		discpp::User bot_user; /**< discpp::User object representing current bot */
		discpp::Logger* logger; /**< discpp::Logger object representing current logger */

		std::unordered_map<snowflake, Channel> channels; /**< List of channels the current bot can access */
		std::unordered_map<snowflake, Member> members; /**< List of members the current bot can access */
		std::unordered_map<snowflake, Guild> guilds; /**< List of guilds the current bot can access */ 
		std::unordered_map<snowflake, Message> messages; /**< List of messages the current bot can access */

		std::vector<std::future<void>> futures; /**< List of events */

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

		Bot(std::string token, BotConfig config);
		int Run();
		void CreateWebsocketRequest(nlohmann::json json, std::string message = "");
		void SetCommandHandler(std::function<void(discpp::Bot*, discpp::Message)> command_handler);
		void DisconnectWebsocket();
		void ReconnectToWebsocket();

		// Discord based methods.
        discpp::Guild GetGuild(snowflake guild_id);
        discpp::User ModifyCurrentUser(std::string username, discpp::Image avatar);
        void LeaveGuild(discpp::Guild guild);
        void UpdatePresence(discpp::Activity activity);
		discpp::User GetUser(discpp::snowflake id);
        std::vector<discpp::Connection> GetBotUserConnections();
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

		std::string session_id;
		std::string gateway_endpoint;

		nlohmann::json hello_packet;

		std::thread heartbeat_thread;

		std::mutex websocket_client_mutex;
		//websocket_callback_client websocket_client;

		ix::WebSocket websocket;

		bool heartbeat_acked;
		int last_sequence_number;
		long long packet_counter;

		int message_cache_count = config.messageCacheSize;

		// Websocket Methods
		void WebSocketStart();
		void OnWebSocketListen(const ix::WebSocketMessagePtr& msg);
		void OnWebSocketPacket(const nlohmann::json& result);
		void HandleDiscordDisconnect(const ix::WebSocketMessagePtr& msg);
		void HandleHeartbeat();
		nlohmann::json GetIdentifyPacket();

		// Commands
		std::function<void(discpp::Bot*, discpp::Message)> fire_command_method;
	};
}

#endif