#include "bot.h"
#include "utils.h"
#include "command_handler.h"
#include "guild.h"
#include "channel.h"
#include "message.h"
#include "member.h"
#include "role.h"
#include "activity.h"
#include "event_handler.h"
#include "event_dispatcher.h"

namespace discpp {
    Bot::Bot(std::string token, BotConfig config) : token(
            token), config(config) {
        /**
         * @brief Constructs a discpp::Bot object.
         *
         * ```cpp
         *      discpp::Bot bot(TOKEN, {"+", "bot "}, discpp::logger_flags::ERROR_SEVERITY | discpp::logger_flags::WARNING_SEVERITY, "log.txt");
         * ```
         *
         * @param[in] token The discpp token the bot needs to run.
         * @param[in] prefixes The bot's prefixes that will be used for command handling.
         * @param[in] logger_flags The flags that will be passed to the logger.
         * @param[in] logger_path The file path for the logger, doesn't need one as it can just log to console/terminal instead.
         *
         * @return discpp::Bot, this is a constructor.
         */

        fire_command_method = std::bind(&discpp::FireCommand, std::placeholders::_1, std::placeholders::_2);

        discpp::globals::bot_instance = this;

        if (config.logger_path.empty()) {
            logger = discpp::Logger(config.logger_flags);
        } else {
            logger = discpp::Logger(config.logger_path, config.logger_flags);
        }
    }

    int Bot::Run() {
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

    discpp::Guild Bot::GetGuild(snowflake guild_id) {
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

        std::unordered_map<snowflake, Guild>::iterator it = discpp::globals::bot_instance->guilds.find(guild_id);
        if (it != discpp::globals::bot_instance->guilds.end()) {
            return it->second;
        }
        throw std::runtime_error("Guild not found!");
    }

    discpp::User Bot::ModifyCurrentUser(std::string username) {
        /**
         * @brief Modify the bot's username.
         *
         * ```cpp
         *      discpp::User user = bot.ModifyCurrent("New bot name!");
         * ```
         *
         * @param[in] username The new username.
         *
         * @return discpp::User
         */

        nlohmann::json result = SendPatchRequest(Endpoint("/users/@me"), DefaultHeaders(), 0,
                                                 discpp::RateLimitBucketType::GLOBAL);

        bot_user = discpp::User(result);

        return bot_user;
    }

    void Bot::LeaveGuild(discpp::Guild guild) {
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

    void Bot::UpdatePresence(discpp::Activity activity) {
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

        nlohmann::json payload = nlohmann::json({
                                                        {"op", 3},
                                                        {"d",  activity.ToJson()}
                                                });

        CreateWebsocketRequest(payload);
    }

    void discpp::Bot::CreateWebsocketRequest(nlohmann::json json) {
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
         *
         * @return void
         */

        logger.Log(LogSeverity::SEV_DEBUG, "Sending gateway payload: " + json.dump());
        WaitForRateLimits(bot_user.id, RateLimitBucketType::GLOBAL);


        websocket_outgoing_message msg;
        msg.set_utf8_message(json.dump());

        auto threadlock = std::lock_guard(websocket_client_mutex);
        websocket_client.send(msg);
    }

    void Bot::SetCommandHandler(std::function<void(discpp::Bot *, discpp::Message)> command_handler) {
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

    void Bot::DisconnectWebsocket() {
        logger.Log(LogSeverity::SEV_DEBUG, LogTextColor::YELLOW + "Locking Mutex before disconnect...");
        std::lock_guard<std::mutex> lck(websocket_client_mutex);
        logger.Log(LogSeverity::SEV_DEBUG, LogTextColor::YELLOW + "Closing websocket connection...");
        websocket_client.close(web::websockets::client::websocket_close_status::server_terminate).wait();
    }

    void Bot::WebSocketStart() {
        nlohmann::json gateway_request = SendGetRequest(Endpoint("/gateway/bot"), {{"Authorization", "Bot " + token},
                                                                                   {"User-Agent",    "discppBot (https://github.com/seanomik/DISCPP, v0.0.0)"}},
                                                        {}, {});

        if (gateway_request.contains("url")) {
            logger.Log(LogSeverity::SEV_DEBUG, LogTextColor::YELLOW + "Connecting to gateway...");

            if (gateway_request["session_start_limit"]["remaining"].get<int>() == 0) {
                logger.Log(LogSeverity::SEV_ERROR, LogTextColor::RED + "GATEWAY ERROR: Maximum start limit reached");
                throw std::runtime_error{"GATEWAY ERROR: Maximum start limit reached"};
            }

            // Specify version and encoding just ot be safe
            gateway_endpoint = gateway_request["url"].get<std::string>() + "?v=6&encoding=json";

            std::thread bindthread {&EventDispatcher::BindEvents, EventDispatcher()};
            utility::string_t stringt = utility::conversions::to_string_t(gateway_endpoint);

            {
                std::lock_guard<std::mutex> lck(websocket_client_mutex);

                // Recreate a websocket client just incase we're trying to reconnect.
                if (reconnecting) {
                    websocket_client = websocket_callback_client();
                }

                websocket_client.connect(web::uri(stringt));
                websocket_client.set_message_handler(std::bind(&Bot::OnWebSocketPacket, this, std::placeholders::_1));
                websocket_client.set_close_handler(
                        std::bind(&Bot::HandleDiscordDisconnect, this, std::placeholders::_1, std::placeholders::_2,
                                  std::placeholders::_3));
            }

            disconnected = false;

            logger.Log(LogSeverity::SEV_INFO, LogTextColor::GREEN + "Connected to gateway!");
            bindthread.join();
        } else {
            logger.Log(LogSeverity::SEV_ERROR, LogTextColor::RED + "Improper token, failed to connect to discpp gateway!");
            throw std::runtime_error("Improper token, failed to connect to discpp gateway!");
        }
    }

    void Bot::HandleDiscordDisconnect(websocket_close_status close_status, utility::string_t reason, std::error_code error_code) {
        logger.Log(LogSeverity::SEV_ERROR, LogTextColor::RED + "Websocket was closed with error: 400" + std::to_string(error_code.value()) + "! Attemping reconnect in 10 seconds...");
        heartbeat_acked = false;
        disconnected = true;
        reconnecting = true;

        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        if (disconnected && !reconnecting) {
            ReconnectToWebsocket();
        }
    }

    void Bot::OnWebSocketPacket(websocket_incoming_message msg) {
        std::string packet_raw = msg.extract_string().get();

        nlohmann::json result = nlohmann::json::parse(packet_raw);
        logger.Log(LogSeverity::SEV_DEBUG, "Received payload: " + result.dump());

        switch (result["op"].get<int>()) {
            case (hello): {
                if (reconnecting) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
                    logger.Log(LogSeverity::SEV_INFO, LogTextColor::GREEN + "Reconnected!");

                    std::string resume = "{ \"op\": 6, \"d\": { \"token\": \"" + token + "\", \"session_id\": \"" + session_id + "\", \"seq\": " + std::to_string(last_sequence_number) + "} }";
                    CreateWebsocketRequest(nlohmann::json::parse(resume));

                    // Heartbeat just to be safe
                    nlohmann::json data = {{"op", packet_opcode::heartbeat}, {"d",  nullptr}};
                    if (last_sequence_number != -1) {
                        data["d"] = last_sequence_number;
                    }

                    reconnecting = false;
                } else {
                    logger.Log(LogSeverity::SEV_DEBUG, "Sending gateway payload: " + GetIdentifyPacket().dump());

                    hello_packet = result;
                    websocket_outgoing_message identify_msg;
                    identify_msg.set_utf8_message(GetIdentifyPacket().dump());

                    {
                        std::lock_guard<std::mutex> lck(websocket_client_mutex);
                        websocket_client.send(identify_msg);
                    }
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
                EventDispatcher::HandleDiscordEvent(result, result["t"]);
                break;
        }

        packet_counter++;
    }

    void Bot::HandleHeartbeat() {
        try {
            while (true) {
                // Make sure that it doesn't try to do anything while its trying to reconnect.
                while (reconnecting) {}

                nlohmann::json data = {{"op", packet_opcode::heartbeat},
                                       {"d",  nullptr}};
                if (last_sequence_number != -1) {
                    data["d"] = last_sequence_number;
                }

                logger.Log(LogSeverity::SEV_DEBUG, "Sending heartbeat payload: " + data.dump());

                {
                    std::lock_guard<std::mutex> lck(websocket_client_mutex);
                    websocket_outgoing_message msg;
                    msg.set_utf8_message(data.dump());
                    websocket_client.send(msg);
                }

                heartbeat_acked = false;

                logger.Log(LogSeverity::SEV_DEBUG, "Waiting for next heartbeat (" + std::to_string(hello_packet["d"]["heartbeat_interval"].get<int>() / 1000.0 - 10) + " seconds)...");
                // Wait for the required heartbeat interval, while waiting it should be acked from another thread.
                std::this_thread::sleep_for(std::chrono::milliseconds(hello_packet["d"]["heartbeat_interval"].get<int>() - 10));

                if (!heartbeat_acked) {
                    logger.Log(LogSeverity::SEV_WARNING, LogTextColor::YELLOW + "Heartbeat wasn't acked, trying to reconnect...");
                    disconnected = true;

                    ReconnectToWebsocket();
                }
            }
        } catch (std::exception &e) {
            logger.Log(LogSeverity::SEV_ERROR, LogTextColor::RED + "ERROR: " + e.what());
        }
    }

    nlohmann::json Bot::GetIdentifyPacket() {
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

    void Bot::ReconnectToWebsocket() {
        logger.Log(LogSeverity::SEV_INFO, LogTextColor::YELLOW + "Reconnecting to Discord gateway!");

        reconnecting = true;

        DisconnectWebsocket();
        WebSocketStart();
    }
}