#ifndef DISCORDPP_BOT_H
#define DISCORDPP_BOT_H

#include <string>
#include <future>
#include <string_view>

#include <nlohmann/json.hpp>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace discord {
	typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
	typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

	class Bot {
	public:
		std::string prefix;

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
	private:
		bool ready = false;
		bool disconnected = true;
		std::string token;
		std::string gateway_endpoint;

		nlohmann::json hello_packet;

		std::thread heartbeat_thread;

		client websocket;
		client::connection_ptr websocket_connection;

		bool heartbeat_acked;
		int last_sequence;
		long long packet_counter;

		std::vector<std::future<void>> futures;
		std::unordered_map<std::string, std::function<void(nlohmann::json)>> internal_event_map;

		// WEBSOCKET METHODS
		void WebSocketStart();
		void OnWebSocketPacket(websocketpp::connection_hdl hdl, client::message_ptr msg);
		void HandleDiscordEvent(nlohmann::json const j, std::string event_name);
		void HandleHeartbeat();
		//void OnWebSocketCallback(auto* ws, std::string_view message, uWS::OpCode opCode);
		nlohmann::json GetIdentifyPacket();

		void BindEvents();

		// EVENTS
		void ReadyEvent(nlohmann::json);
		void HelloEvent(nlohmann::json);
	};
}

#endif