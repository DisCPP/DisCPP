#include "client.h"
#include "utils.h"
#include "command_handler.h"
#include "guild.h"
#include "channel.h"
#include "message.h"
#include "member.h"
#include "activity.h"
#include "event_handler.h"
#include "event_dispatcher.h"

#include <ixwebsocket/IXNetSystem.h>

namespace discpp {
    Client::Client(std::string token, ClientConfig config) : token(token), config(config) {
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

        fire_command_method = std::bind(discpp::FireCommand, std::placeholders::_1, std::placeholders::_2);

        discpp::globals::client_instance = this;

        if (config.logger_path.empty()) {
            logger = new discpp::Logger(config.logger_flags);
        } else {
            logger = new discpp::Logger(config.logger_path, config.logger_flags);
        }
    }

    int Client::Run() {
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

        WebSocketStart();
        while (true) {
            for (size_t i = 0; i < futures.size(); i++) {
                if (!futures[i].valid() ||
                    !(futures[i].wait_for(std::chrono::seconds(0)) == std::future_status::ready)) {
                    continue;
                }
                futures.erase(futures.begin() + i);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return 0;
    }

    discpp::Guild Client::GetGuild(snowflake guild_id) {
        /**
         * @brief Gets a discpp::Guild from a guild id.
         *
         * This will throw a runtime exception if the guild is not found.
         *
         * ```cpp
         *      discpp::Guild guild = bot.GetGuild(583251190591258624);
         * ```
         *
         * @param[in] guild_id The guild id of the guild you want to get.
         *
         * @return discpp::Guild
         */

        std::unordered_map<snowflake, Guild>::iterator it = discpp::globals::client_instance->guilds.find(guild_id);
        if (it != discpp::globals::client_instance->guilds.end()) {
            return it->second;
        }
        throw std::runtime_error("Guild not found!");
    }

    discpp::User Client::ModifyCurrentUser(std::string username, discpp::Image avatar) {
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

        cpr::Body body("{\"username\": \"" + username + "\", \"avatar\": " + avatar.ToDataURI() + "}");
        nlohmann::json result = SendPatchRequest(Endpoint("/users/@me"), DefaultHeaders(), 0, discpp::RateLimitBucketType::GLOBAL, body);

        client_user = discpp::User(result);

        return client_user;
    }

    void Client::LeaveGuild(discpp::Guild guild) {
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

        SendDeleteRequest(Endpoint("/users/@me/guilds/" + guild.id), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
    }

    discpp::User Client::GetUser(discpp::snowflake id) {
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

        nlohmann::json result = SendGetRequest(Endpoint("/users/" + id), DefaultHeaders(), "", RateLimitBucketType::GLOBAL);

        return discpp::User(result);
    }

    std::vector<discpp::Connection> Client::GetBotUserConnections() {
        /**
         * @brief Get the bot user connections.
         *
         * ```cpp
         *      bot.GetBotUserConnections();
         * ```
         *
         * @return std::vector<discpp::Connection>
         */

        nlohmann::json result = SendGetRequest(Endpoint("/users/@me/connections"), DefaultHeaders(), "", RateLimitBucketType::GLOBAL);

        std::vector<discpp::Connection> connections;
        for (auto const& connection : result) {
            connections.push_back(discpp::Connection(connection));
        }

        return connections;
    }

    void Client::UpdatePresence(discpp::Activity activity) {
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

        nlohmann::json payload = nlohmann::json({ {"op", 3}, {"d", activity.ToJson()} });

        CreateWebsocketRequest(payload);
    }

    void discpp::Client::CreateWebsocketRequest(nlohmann::json json, std::string message) {
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

        std::string json_payload = json.dump();

        if (message.empty()) {
            logger->Debug("Sending gateway payload: " + json_payload);
        } else {
            logger->Debug(message);
        }

        WaitForRateLimits(client_user.id, RateLimitBucketType::GLOBAL);

        std::lock_guard<std::mutex> lock = std::lock_guard(websocket_client_mutex);
        websocket.sendText(json_payload);
    }

    void Client::SetCommandHandler(std::function<void(discpp::Client *, discpp::Message)> command_handler) {
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

        fire_command_method = command_handler;
    }

    void Client::DisconnectWebsocket() {
        logger->Debug(LogTextColor::YELLOW + "Locking Mutex before disconnect...");
        std::lock_guard<std::mutex> lock(websocket_client_mutex);
        logger->Debug(LogTextColor::YELLOW + "Closing websocket connection...");

        websocket.close(ix::WebSocketCloseConstants::kNormalClosureCode);
        websocket.stop(ix::WebSocketCloseConstants::kNormalClosureCode);
    }

    void Client::WebSocketStart() {
        nlohmann::json gateway_request;
        switch (config.type) {
        case TokenType::USER:
            gateway_request = SendGetRequest(Endpoint("/gateway"), { {"Authorization", token}, {"User-Agent", "discpp (https://github.com/DisCPP/DisCPP, v0.0.0)"} }, {}, {});
            break;
        case TokenType::BOT:
            gateway_request = SendGetRequest(Endpoint("/gateway/bot"), { {"Authorization", "Bot " + token}, {"User-Agent", "discpp (https://github.com/DisCPP/DisCPP, v0.0.0)"} }, {}, {});
            break;
        }

        if (gateway_request.contains("url")) {
            logger->Debug(LogTextColor::YELLOW + "Connecting to gateway...");

            if (gateway_request["session_start_limit"]["remaining"].get<int>() == 0) {
                logger->Debug(LogTextColor::RED + "GATEWAY ERROR: Maximum start limit reached");
                throw std::runtime_error{"GATEWAY ERROR: Maximum start limit reached"};
            }

            // Specify version and encoding just ot be safe
            gateway_endpoint = gateway_request["url"].get<std::string>() + "/?v=6&encoding=json";

            std::thread bindthread {&EventDispatcher::BindEvents, EventDispatcher()};

#ifdef _WIN32
            if (!reconnecting) {
                ix::initNetSystem();
            }
#endif

            {
                std::lock_guard<std::mutex> lock(websocket_client_mutex);
                if (reconnecting) {

                }

                websocket.setUrl(gateway_endpoint);
                websocket.disableAutomaticReconnection();

                websocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
                    OnWebSocketListen(msg);
                });

                /*ix::SocketTLSOptions tls_options;
                tls_options.certFile = "cacert.pem";
                websocket.setTLSOptions(tls_options);*/

                websocket.start();
            }

            disconnected = false;

            bindthread.join();
        } else {
            logger->Error(LogTextColor::RED + "Improper token, failed to connect to discord gateway!");
            throw std::runtime_error("Improper token, failed to connect to discord gateway!");
        }
    }

    void Client::HandleDiscordDisconnect(const ix::WebSocketMessagePtr& msg) {
        // if we're reconnecting this just stop here.
        if (reconnecting) {
            logger->Debug("Websocket was closed for reconnecting...");
        } else {
            logger->Error(LogTextColor::RED + "Websocket was closed with error: " + std::to_string(msg->closeInfo.code) + ", " + msg->closeInfo.reason + "! Attempting reconnect in 10 seconds...");
        }

        heartbeat_acked = false;
        disconnected = true;

        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        if (disconnected && !reconnecting) {
            reconnecting = true;
            ReconnectToWebsocket();
        }
    }

    void Client::OnWebSocketListen(const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            logger->Info(LogTextColor::GREEN + "Connected to gateway!");

            disconnected = false;
            reconnecting = false;
        } else if (msg->type == ix::WebSocketMessageType::Close) {
            HandleDiscordDisconnect(msg);
        } else if (msg->type == ix::WebSocketMessageType::Error) {
            logger->Info(LogTextColor::RED + "Error: " + msg->errorInfo.reason);
        } else if (msg->type == ix::WebSocketMessageType::Message) {
            nlohmann::json result;
            try {
                result = nlohmann::json::parse(msg->str);
            } catch(const nlohmann::json::exception& e) {
                logger->Debug(LogTextColor::YELLOW + "A non json payload was received and ignored: \"" + msg->str + "\" (Error: " + e.what() + ")");
            }

            if (!result.empty()) {
                OnWebSocketPacket(result);
            }
        } else {
            logger->Info(LogTextColor::RED + "Known message sent");
        }
    }

    void Client::OnWebSocketPacket(const nlohmann::json& result) {
        logger->Debug("Received payload: " + result.dump());

        switch (result["op"].get<int>()) {
            case (hello): {
                if (reconnecting) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
                    logger->Info(LogTextColor::GREEN + "Reconnected!");

                    std::string resume = "{ \"op\": 6, \"d\": { \"token\": \"" + token + "\", \"session_id\": \"" + session_id + "\", \"seq\": " + std::to_string(last_sequence_number) + "} }";
                    CreateWebsocketRequest(nlohmann::json::parse(resume));

                    // Heartbeat just to be safe
                    nlohmann::json data = {{"op", packet_opcode::heartbeat}, {"d",  nullptr}};
                    if (last_sequence_number != -1) {
                        data["d"] = last_sequence_number;
                    }

                    heartbeat_acked = true;
                    reconnecting = false;
                } else {
                    hello_packet = result;

                    CreateWebsocketRequest(GetIdentifyPacket());
                }
                break;
            }
            case heartbeat_ack:
                heartbeat_acked = true;
                break;
            case reconnect:
                ReconnectToWebsocket();
                break;
            case invalid_session:
                // Check if the session is resumable
                if (result["d"].get<bool>()) {
                    std::string resume = "{ \"op\": 6, \"d\": { \"token\": \"" + token + "\", \"session_id\": \"" + session_id + "\", \"seq\": " + std::to_string(last_sequence_number) + "} }";
                    CreateWebsocketRequest(nlohmann::json::parse(resume));
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    CreateWebsocketRequest(GetIdentifyPacket());
                }

                break;
            default:
                EventDispatcher::HandleDiscordEvent(const_cast<nlohmann::json &>(result), result["t"].get<std::string>());
                break;
        }

        packet_counter++;
    }

    void Client::HandleHeartbeat() {
        try {
            while (true) {
                // Make sure that it doesn't try to do anything while its trying to reconnect.
                while (reconnecting) {}

                nlohmann::json data = {{"op", packet_opcode::heartbeat},
                                       {"d",  nullptr}};
                if (last_sequence_number != -1) {
                    data["d"] = last_sequence_number;
                }

                CreateWebsocketRequest(data, "Sending heartbeat payload: " + data.dump());

                heartbeat_acked = false;

                logger->Debug("Waiting for next heartbeat (" + std::to_string(hello_packet["d"]["heartbeat_interval"].get<int>() / 1000.0 - 10) + " seconds)...");
                // Wait for the required heartbeat interval, while waiting it should be acked from another thread.
                std::this_thread::sleep_for(std::chrono::milliseconds(hello_packet["d"]["heartbeat_interval"].get<int>() - 10));

                if (!heartbeat_acked) {
                    logger->Warn(LogTextColor::YELLOW + "Heartbeat wasn't acked, trying to reconnect...");
                    disconnected = true;

                    ReconnectToWebsocket();
                }
            }
        } catch (std::exception &e) {
            logger->Error(LogTextColor::RED + "ERROR: " + e.what());
        }
    }

    nlohmann::json Client::GetIdentifyPacket() {
        nlohmann::json obj = {{"op", packet_opcode::identify},
                              {"d",
                                     {{"token", token},
                                             {"properties",
                                                     {{"$os", GetOsName()},
                                                             {"$browser", "DISCPP"},
                                                             {"$device", "DISCPP"}}},
                                             {"compress", false},
                                             {"large_threshold", 250}}}};
        return obj;
    }

    void Client::ReconnectToWebsocket() {
        if (!reconnecting) {
            logger->Info(LogTextColor::YELLOW + "Reconnecting to Discord gateway!");

            reconnecting = true;

            DisconnectWebsocket();
            // Connect with a 20 second timeout.
            websocket.connect(20);
            //WebSocketStart();
        }
    }
}