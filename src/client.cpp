#include "client.h"
#include "utils.h"
#include "command_handler.h"
#include "guild.h"
#include "channel.h"
#include "message.h"
#include "member.h"
#include "presence.h"
#include "event_handler.h"
#include "event_dispatcher.h"
#include "client_config.h"
#include "exceptions.h"
#include "settings.h"

#include <ixwebsocket/IXNetSystem.h>

namespace discpp {
    Client::Client(std::string token, ClientConfig* config) : token(token), config(config) {
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

        message_cache_count = config->message_cache_size;

        if (config->logger_path.empty()) {
            logger = new discpp::Logger(config->logger_flags);
        } else {
            logger = new discpp::Logger(config->logger_path, config->logger_flags);
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

        while (run) {
            {
                std::lock_guard<std::mutex> futures_guard(futures_mutex);
                futures.erase(std::remove_if(futures.begin(), futures.end(), [](const std::future<void>& future) {
                    return future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
                }), futures.end());
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return 0;
    }

    void Client::UpdatePresence(discpp::Presence& presence) {
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

        rapidjson::Document payload(rapidjson::kObjectType);
        rapidjson::Document activity_json = presence.ToJson();

        payload.AddMember("op", status_update, payload.GetAllocator());
        payload.AddMember("d", activity_json, payload.GetAllocator());

        CreateWebsocketRequest(payload);
    }

    void discpp::Client::CreateWebsocketRequest(rapidjson::Document& json, std::string message) {
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
        std::string json_payload = DumpJson(json);

        if (message.empty()) {
            logger->Debug("Sending gateway payload: " + json_payload);
        } else {
            logger->Debug(message);
        }

        WaitForRateLimits(client_user.id, RateLimitBucketType::GLOBAL);

        std::lock_guard<std::mutex> lock = std::lock_guard(websocket_client_mutex);
        websocket.sendText(json_payload);
    }

    void Client::SetCommandHandler(std::function<void(discpp::Client*, discpp::Message)> command_handler) {
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
        rapidjson::Document gateway_request(rapidjson::kObjectType);
        switch (config->type) {
        case TokenType::USER: {
            rapidjson::Document user_doc = SendGetRequest(Endpoint("/gateway"), {{"Authorization", token}, {"User-Agent", "discpp (https://github.com/DisCPP/DisCPP, v0.0.0)"}}, {}, {});
            gateway_request.CopyFrom(user_doc, gateway_request.GetAllocator());

            break;
        } case TokenType::BOT:
            rapidjson::Document bot_doc = SendGetRequest(Endpoint("/gateway/bot"), { {"Authorization", "Bot " + token}, {"User-Agent", "discpp (https://github.com/DisCPP/DisCPP, v0.0.0)"} }, {}, {});
            gateway_request.CopyFrom(bot_doc, gateway_request.GetAllocator());

            break;
        }


        rapidjson::Value::ConstMemberIterator itr = gateway_request.FindMember("url");

        if (itr != gateway_request.MemberEnd()) {
            logger->Debug(LogTextColor::YELLOW + "Connecting to gateway...");

            itr = gateway_request.FindMember("session_start_limit");
            if (itr != gateway_request.MemberEnd() && gateway_request["session_start_limit"]["remaining"].GetInt() == 0) {
                logger->Debug(LogTextColor::RED + "GATEWAY ERROR: Maximum start limit reached");
                throw new StartLimitException();
            }

            // Specify version and encoding just ot be safe
            std::string url = gateway_request["url"].GetString();
            gateway_endpoint = url + "/?v=6&encoding=json";

            std::thread bindthread{ &EventDispatcher::BindEvents };

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

                websocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
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
            throw AuthenticationException();
        }
    }

    void Client::HandleDiscordDisconnect(const ix::WebSocketMessagePtr& msg) {
        // if we're reconnecting this just stop here.
        if (reconnecting) {
            logger->Debug("Websocket was closed for reconnecting...");
        } else if (stay_disconnected) {
            logger->Warn(LogTextColor::YELLOW + "Websocket was closed.");
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
            rapidjson::Document result;
            result.Parse(msg->str);
            if (result.HasParseError()) {
                logger->Debug(LogTextColor::YELLOW + "A non-json payload was received and ignored: \"" + msg->str);
            }

            if (!result.IsNull()) {
                OnWebSocketPacket(result);
            }
        } else {
            logger->Warn(LogTextColor::YELLOW + "Unknown message sent");
        }
    }

    void Client::OnWebSocketPacket(rapidjson::Document& result) {
        logger->Debug("Received payload: " + DumpJson(result));

        switch (result["op"].GetInt()) {
        case (hello): {
            if (reconnecting) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1200));
                logger->Info(LogTextColor::GREEN + "Reconnected!");

                rapidjson::Document resume(rapidjson::kObjectType);
                resume.AddMember("op", 6, resume.GetAllocator());

                rapidjson::Value resume_d(rapidjson::kObjectType);
                resume_d.AddMember("token", token, resume.GetAllocator());
                resume_d.AddMember("session_id", session_id, resume.GetAllocator());
                resume_d.AddMember("seq", std::to_string(last_sequence_number), resume.GetAllocator());

                resume.AddMember("d", resume_d, resume.GetAllocator());

                CreateWebsocketRequest(resume);

                // Heartbeat just to be safe
                rapidjson::Document data;
                data.SetObject();
                rapidjson::Document::AllocatorType& data_allocator = data.GetAllocator();
                data.AddMember("op", packet_opcode::heartbeat, data_allocator);
                data.AddMember("d", NULL, data_allocator);
                if (last_sequence_number != -1) {
                    data["d"] = last_sequence_number;
                }

                heartbeat_acked = true;
                reconnecting = false;
            } else {
                hello_packet = std::move(result);

                rapidjson::Document identify = GetIdentifyPacket();
                CreateWebsocketRequest(identify);
            }
            break;
        }
        case heartbeat_ack:
            heartbeat_acked = true;
            break;
        case reconnect:
            DoFunctionLater([&] {ReconnectToWebsocket();});
            break;
        case invalid_session:
            // Check if the session is resumable
            if (result["d"].GetBool()) {
                rapidjson::Document resume(rapidjson::kObjectType);

                rapidjson::Document::AllocatorType& allocator = resume.GetAllocator();
                resume.AddMember("op", 6, allocator);

                rapidjson::Value d(rapidjson::kObjectType);
                d.AddMember("token", token, allocator);
                d.AddMember("session_id", session_id, allocator);
                d.AddMember("seq", std::to_string(last_sequence_number), allocator);

                resume.AddMember("d", d, allocator);

                CreateWebsocketRequest(resume);
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                rapidjson::Document identify = GetIdentifyPacket();
                CreateWebsocketRequest(identify);
            }

            break;
        default:
            EventDispatcher::HandleDiscordEvent(result, result["t"].GetString());
            break;
        }

        packet_counter++;
    }

    void Client::HandleHeartbeat() {
        try {
            while (run) {
                // Make sure that it doesn't try to do anything while its trying to reconnect.
                while (reconnecting && !run) {}

                rapidjson::Document data(rapidjson::kObjectType);
                rapidjson::Document::AllocatorType& data_allocator = data.GetAllocator();
                data.AddMember("op", packet_opcode::heartbeat, data_allocator);
                data.AddMember("d", NULL, data_allocator);
                if (last_sequence_number != -1) {
                    data["d"] = last_sequence_number;
                }

                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                data.Accept(writer);
                std::string json_payload = buffer.GetString();
                CreateWebsocketRequest(data, "Sending heartbeat payload: " + json_payload);

                heartbeat_acked = false;

                int heartbeat_interval = hello_packet["d"]["heartbeat_interval"].GetInt();
                logger->Debug("Waiting for next heartbeat (" + std::to_string(heartbeat_interval / 1000.0 - 10) + " seconds)...");

                // Wait for the required heartbeat interval, while waiting it should be acked from another thread.
                // This also checks it should stop this thread.
                long int timer = static_cast<long int>(time(nullptr));
                int ending_time = timer + heartbeat_interval / 1000.0 - 10;

                while (timer <= ending_time) {
                    if (!run) {
                        return;
                    }

                    // Increment the timer
                    timer += (time(NULL) - timer);
                }

                if (!heartbeat_acked && !reconnecting) {
                    logger->Warn(LogTextColor::YELLOW + "Heartbeat wasn't acked, trying to reconnect...");
                    disconnected = true;

                    ReconnectToWebsocket();
                }
            }
        } catch (std::exception& e) {
            logger->Error(LogTextColor::RED + "[HEARTBEAT THREAD] Exception: " + e.what());
        }
    }

    rapidjson::Document Client::GetIdentifyPacket() {
        rapidjson::Document document;
        document.SetObject();

        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        document.AddMember("op", packet_opcode::identify, allocator);

        rapidjson::Value d(rapidjson::kObjectType);
        d.AddMember("token", token, allocator);

        rapidjson::Value properties(rapidjson::kObjectType);
        properties.AddMember("$os", GetOsName(), allocator);
        properties.AddMember("$browser", "DISCPP", allocator);
        properties.AddMember("$device", "DISCPP", allocator);

        d.AddMember("properties", properties, allocator);
        d.AddMember("compress", false, allocator);
        d.AddMember("large_threshold", 250, allocator);

        document.AddMember("d", d, allocator);

        return std::move(document);
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

    void Client::StopClient() {
        stay_disconnected = true;
        DisconnectWebsocket();

        run = false;

        if (heartbeat_thread.joinable()) heartbeat_thread.join();
    }

    discpp::Channel Client::GetChannel(discpp::snowflake id) {
        discpp::Channel channel = GetDMChannel(id);

        if (channel.id == 0) {
            for (const auto &guild : guilds) {
                channel = guild.second->GetChannel(id);

                if (channel.id != 0) return channel;
            }
        }

        return channel;
    }

    discpp::DMChannel Client::GetDMChannel(discpp::snowflake id) {
        auto it = private_channels.find(id);
        if (it != private_channels.end()) {
            return it->second;
        }

        return discpp::DMChannel();
    }

    std::unordered_map<discpp::snowflake, discpp::DMChannel> Client::GetUserDMs() {
        /**
         * @brief Get all DM's for this user. Only supports user tokens!
         *
         * ```cpp
         *      std::vector<discpp::User::Connection> conntections = client->GetUserConnections();
         * ```
         *
         * @return std::vector<discpp::User::Connection>
         */

        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw new ProhibitedEndpointException("/users/@me/channels is a user only endpoint");
        } else {
            std::unordered_map<discpp::snowflake, discpp::DMChannel> dm_channels;

            rapidjson::Document result = SendGetRequest(Endpoint("users/@me/channels"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
            for (auto const& channel : result.GetArray()) {
                rapidjson::Document channel_json(rapidjson::kObjectType);
                channel_json.CopyFrom(channel, channel_json.GetAllocator());

                discpp::DMChannel tmp(channel_json);
                dm_channels.emplace(tmp.id, tmp);
            }

            return dm_channels;
        }
    }

    std::vector<discpp::User::Connection> ClientUser::GetUserConnections() {
		/**
		 * @brief Get all connections of this user.
		 *
		 * ```cpp
		 *      std::vector<discpp::User::Connection> conntections = client->GetUserConnections();
		 * ```
		 *
		 * @return std::vector<discpp::User::Connection>
		 */

		rapidjson::Document result = SendGetRequest(Endpoint("/users/@me/connections"), DefaultHeaders(), id, RateLimitBucketType::GLOBAL);

		std::vector<Connection> connections;
		for (auto const& connection : result.GetArray()) {
			rapidjson::Document connection_json;
			connection_json.CopyFrom(connection, connection_json.GetAllocator());
			connections.push_back(discpp::User::Connection(connection_json));
		}

		return connections;
	}

	ClientUser::ClientUser(rapidjson::Document& json) : User(json) {
		mfa_enabled = GetDataSafely<bool>(json, "mfa_enabled");
		locale = GetDataSafely<std::string>(json, "locale");
		verified = GetDataSafely<bool>(json, "verified");
	}

    ClientUserSettings ClientUser::GetSettings() {
        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw new ProhibitedEndpointException("users/@me/settings is a user only endpoint");
        }
        else {
            rapidjson::Document result = SendGetRequest(Endpoint("users/@me/settings/"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
            ClientUserSettings settings(result);
            this->settings = settings;
            return settings;
        }
    }

    void ClientUser::ModifySettings(ClientUserSettings& settings) {
        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw new ProhibitedEndpointException("users/@me/settings is a user only endpoint");
        }
        else {
            rapidjson::Document new_settings;
            new_settings.SetObject();

            rapidjson::Document::AllocatorType& allocator = new_settings.GetAllocator();
            ClientUserSettings old_settings = this->settings;
            if (settings.afk_timeout != old_settings.afk_timeout) new_settings.AddMember("afk_timeout", settings.afk_timeout, allocator);
            if (settings.custom_status != old_settings.custom_status) new_settings.AddMember("custom_status", settings.custom_status, allocator);
            if (settings.explicit_content_filter != old_settings.explicit_content_filter) new_settings.AddMember("explicit_content_filter", (int) settings.explicit_content_filter, allocator);
            if (settings.theme != old_settings.theme) new_settings.AddMember("theme", ThemeToString(settings.theme), allocator);
            rapidjson::Value friend_source_flags(rapidjson::kObjectType);
            bool add_friend_source_flags = false;
            if (settings.friend_source_flags.GetAll() != old_settings.friend_source_flags.GetAll()) {
                friend_source_flags.AddMember("all", settings.friend_source_flags.GetAll(), allocator);
                add_friend_source_flags = true;
            }
            if (settings.friend_source_flags.GetMutualFriends() != old_settings.friend_source_flags.GetMutualFriends()) {
                friend_source_flags.AddMember("mutual_friends", settings.friend_source_flags.GetMutualFriends(), allocator);
                add_friend_source_flags = true;
            }
            if (settings.friend_source_flags.GetMutualGuilds() != old_settings.friend_source_flags.GetMutualGuilds()) {
                friend_source_flags.AddMember("mutual_guilds", settings.friend_source_flags.GetMutualGuilds(), allocator);
                add_friend_source_flags = true;
            }
            if (add_friend_source_flags) new_settings.AddMember("friend_source_flags", friend_source_flags, allocator);
            if (settings.GetAllowAccessibilityDetection() != old_settings.GetAllowAccessibilityDetection()) new_settings.AddMember("allow_accessibility_detection", settings.GetAllowAccessibilityDetection(), allocator);
            if (settings.GetAnimateEmoji() != old_settings.GetAnimateEmoji()) new_settings.AddMember("animate_emoji", settings.GetAnimateEmoji(), allocator);
            if (settings.GetContactSyncEnabled() != old_settings.GetContactSyncEnabled()) new_settings.AddMember("contact_sync_enabled", settings.GetContactSyncEnabled(), allocator);
            if (settings.GetConvertEmoticons() != old_settings.GetConvertEmoticons()) new_settings.AddMember("convert_emoticons", settings.GetConvertEmoticons(), allocator);
            if (settings.GetDefaultGuildsRestricted() != old_settings.GetDefaultGuildsRestricted()) new_settings.AddMember("default_guilds_restricted", settings.GetDefaultGuildsRestricted(), allocator);
            if (settings.GetDetectPlatformAccounts() != old_settings.GetDetectPlatformAccounts()) new_settings.AddMember("detect_platform_accounts", settings.GetDetectPlatformAccounts(), allocator);
            if (settings.GetDeveloperMode() != old_settings.GetDeveloperMode()) new_settings.AddMember("developer_mode", settings.GetDeveloperMode(), allocator);
            if (settings.GetDisableGamesTab() != old_settings.GetDisableGamesTab()) new_settings.AddMember("disable_games_tab", settings.GetDisableGamesTab(), allocator);
            if (settings.GetEnableTtsCommand() != old_settings.GetEnableTtsCommand()) new_settings.AddMember("enable_tts_command", settings.GetEnableTtsCommand(), allocator);
            if (settings.GetGifAutoPlay() != old_settings.GetGifAutoPlay()) new_settings.AddMember("gif_auto_play", settings.GetGifAutoPlay(), allocator);
            if (settings.GetInlineAttachmentMedia() != old_settings.GetInlineAttachmentMedia()) new_settings.AddMember("inline_attachment_media", settings.GetInlineAttachmentMedia(), allocator);
            if (settings.GetInlineEmbedMedia() != old_settings.GetInlineEmbedMedia()) new_settings.AddMember("inline_embed_media", settings.GetInlineEmbedMedia(), allocator);
            if (settings.GetMessageDisplayCompact() != old_settings.GetMessageDisplayCompact()) new_settings.AddMember("message_display_compact", settings.GetMessageDisplayCompact(), allocator);
            if (settings.GetNativePhoneIntegrationEnabled() != old_settings.GetNativePhoneIntegrationEnabled()) new_settings.AddMember("native_phone_integration_enabled", settings.GetNativePhoneIntegrationEnabled(), allocator);
            if (settings.GetRenderEmbeds() != old_settings.GetRenderEmbeds()) new_settings.AddMember("render_embeds", settings.GetRenderEmbeds(), allocator);
            if (settings.GetRenderReactions() != old_settings.GetRenderReactions()) new_settings.AddMember("render_reactions", settings.GetRenderReactions(), allocator);
            if (settings.GetShowCurrentGame() != old_settings.GetShowCurrentGame()) new_settings.AddMember("show_current_game", settings.GetShowCurrentGame(), allocator);
            if (settings.GetStreamNotificationsEnabled() != old_settings.GetStreamNotificationsEnabled()) new_settings.AddMember("stream_notifications_enabled", settings.GetStreamNotificationsEnabled(), allocator);

            rapidjson::Document result = SendPatchRequest(Endpoint("users/@me/settings/"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL, cpr::Body(DumpJson(new_settings)));
        }
    }

    void Client::AddFriend(discpp::User user) {
        /**
         * @brief Add a friend. Only supports user tokens!
         *
         * @return void
         */
        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw new ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            rapidjson::Document result = SendPutRequest(Endpoint("users/@me/relationships/" + std::to_string(user.id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
        }
    }

    void Client::RemoveFriend(discpp::User user) {
        /**
         * @brief Remove a friend. Only supports user tokens!
         *
         * @return void
         */
        if(discpp::globals::client_instance->client_user.IsBot()) {
            throw new ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            rapidjson::Document result = SendDeleteRequest(Endpoint("users/@me/relationships/" + std::to_string(user.id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
        }
    }

    std::unordered_map<discpp::snowflake, discpp::UserRelationship> Client::GetRelationships() {
        /**
         * @brief Get all friends. Only supports user tokens!
         *
         * @return std::unordered_map<discpp::snowflake, discpp::UserRelationship>
         */
        //todo implement this endpoint
        if(discpp::globals::client_instance->client_user.IsBot()) {
            throw new ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unordered_map<discpp::snowflake, discpp::UserRelationship> relationships;

            rapidjson::Document result = SendGetRequest(Endpoint("users/@me/relationships/"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
            for (auto const& relationship : result.GetArray()) {
                rapidjson::Document relationship_json(rapidjson::kObjectType);
                relationship_json.CopyFrom(relationship, relationship_json.GetAllocator());

                discpp::UserRelationship tmp(relationship_json);
                relationships.emplace(tmp.id, tmp);
            }
            return relationships;
        }
    }

    std::shared_ptr<discpp::Guild> Client::GetGuild(snowflake guild_id) {
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

        auto it = discpp::globals::client_instance->guilds.find(guild_id);
        if (it != discpp::globals::client_instance->guilds.end()) {
            return it->second;
        }

        throw new DiscordObjectNotFound("Guild not found");
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
        rapidjson::Document result = SendPatchRequest(Endpoint("/users/@me"), DefaultHeaders(), 0, discpp::RateLimitBucketType::GLOBAL, body);

        client_user = discpp::ClientUser(result);

        return client_user;
    }

    void Client::LeaveGuild(discpp::Guild& guild) {
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

    discpp::User Client::ReqestUserIfNotCached(discpp::snowflake id) {
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

        discpp::User user(id);
        if (user.username.empty()) {
            rapidjson::Document result = SendGetRequest(Endpoint("/users/" + std::to_string(id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
            return discpp::User(result);
        }

        return user;
    }

    std::vector<discpp::User::Connection> Client::GetBotUserConnections() {
        /**
         * @brief Get the bot user connections.
         *
         * ```cpp
         *      bot.GetBotUserConnections();
         * ```
         *
         * @return std::vector<discpp::User::Connection>
         */

        rapidjson::Document result = SendGetRequest(Endpoint("/users/@me/connections"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);

        std::vector<discpp::User::Connection> connections;
        for (auto const& connection : result.GetArray()) {
            rapidjson::Document connection_json;
            connection_json.CopyFrom(connection, connection_json.GetAllocator());
            connections.push_back(discpp::User::Connection(connection_json));
        }

        return connections;
    }

    UserRelationship::UserRelationship(rapidjson::Document& json) {
        /**
         * @brief Contructs a UserRelationship from json
         *
         * @return discpp::UserRelationship
         */
        id = SnowflakeFromString(json["id"].GetString());
        nickname = GetDataSafely<std::string>(json, "nickname");
        type = json["type"].GetInt();
        user = ConstructDiscppObjectFromJson(json, "user", discpp::User());
    }

    bool UserRelationship::IsFriend() {
        /**
         * @brief Returns if this relation is a friend.
         *
         * @return bool
         */
        return type == 1;
    }

    bool UserRelationship::IsBlocked() {
        /**
         * @brief Returns if this relation is a block.
         *
         * @return bool
         */
        return type == 2;
    }
}