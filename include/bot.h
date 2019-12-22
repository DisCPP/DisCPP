#ifndef DISCORDPP_BOT_H
#define DISCORDPP_BOT_H

#include <string>
#include <future>

#include <nlohmann/json.hpp>

#include <ixwebsocket/IXWebSocket.h>

namespace discord {
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
		std::string token;
		std::string websocket_endpoint;

		std::string external_ip;
		int external_port;

		nlohmann::json hello_packet;

		ix::WebSocket web_socket;

		std::thread heartbeat_thread;

		bool heartbeat_acked;
		bool disconnected;
		bool websocket_disconnected;
		bool websocket_ready;
		int websocket_last_sequence;
		long long packet_counter;

		std::vector<std::future<void>> futures;
		std::unordered_map<std::string, std::function<void(nlohmann::json)>> internal_event_map;

		// WEBSOCKET METHODS
		void WebSocketStart();
		std::string GetIdentifyPacket();
		void OnWebsocketCallback(const ix::WebSocketMessagePtr& msg);
		void OnWebsocketPacket(nlohmann::json result);
		void HandleDiscordEvent(nlohmann::json const j, std::string event_name);
		void HandleHeartbeat();

		void BindEvents();

		// EVENTS
		void ReadyEvent(nlohmann::json);
		void HelloEvent(nlohmann::json);
	};
}

#endif