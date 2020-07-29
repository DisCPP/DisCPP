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
#include "events/reconnect_event.h"

#include <ixwebsocket/IXNetSystem.h>

#ifdef RAPIDJSON_BACKEND
#include <rapidjson/writer.h>
#elif SIMDJSON_BACKEND

#endif

namespace discpp {
    Client::Client(const std::string& token, ClientConfig* config) : token(token), config(config) {
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
        EventDispatcher::BindEvents();

        DoFunctionLater([&] {
            discpp::JsonObject gateway_request("{}");
            switch (config->type) {
                case TokenType::USER: {
                    gateway_request = *SendGetRequest(Endpoint("/gateway"), {{"Authorization", token}, {"User-Agent", "discpp (https://github.com/DisCPP/DisCPP, v0.0.0)"}}, {}, {});
                    break;
                } case TokenType::BOT:
                    gateway_request = *SendGetRequest(Endpoint("/gateway/bot"), { {"Authorization", "Bot " + token}, {"User-Agent", "discpp (https://github.com/DisCPP/DisCPP, v0.0.0)"} }, {}, {});
                    break;
            }

            if (gateway_request.ContainsNotNull("url")) {
                if (gateway_request.ContainsNotNull("session_start_limit") &&
                    gateway_request["session_start_limit"]["remaining"].GetInt() == 0) {

                    logger->Debug(LogTextColor::RED + "GATEWAY ERROR: Maximum start limit reached");
                    throw exceptions::MaximumLimitException("Gateway start limit exceeded!");
                }

                if (gateway_request.ContainsNotNull("shards")) {
                    int recommended_shards = gateway_request["shards"].GetInt();
                    if (recommended_shards > config->shard_amount) {
                        logger->Warn(LogTextColor::YELLOW + "You set shard amount to \"" + std::to_string(config->shard_amount) + \
                            "\" but discord recommends to use \"" + std::to_string(recommended_shards) + "\", so we're gonna listen to Discord...");

                        config->shard_amount = recommended_shards;
                    }
                }

                // Specify version and encoding just ot be safe
                std::string url = std::string(gateway_request["url"].GetString()) + "/?v=6&encoding=json";

                for (int i = 0; i < config->shard_amount; i++) {
                    auto* shard = new Shard(*this, i, url);
                    shard->WebSocketStart();

                    shards.emplace_back(shard);

                    // We can only start a new shard every 5 seconds.
                    std::this_thread::sleep_for(std::chrono::milliseconds(5050));
                }
            } else {

            }
        });

        while (run) {
            {
                std::lock_guard<std::mutex> futures_guard(this->futures_mutex);
                futures.erase(std::remove_if(futures.begin(), futures.end(), [](const std::future<void>& future) {
                    return future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
                }), futures.end());
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return 0;
    }

    void Shard::CreateWebsocketRequest(const discpp::JsonObject& json, const std::string& message) {
        std::string json_payload = json.DumpJson();

        if (message.empty()) {
            client.logger->Debug("[SHARD " + std::to_string(id) + "] Sending gateway payload: " + json_payload);
        } else {
            client.logger->Debug(message);
        }

        WaitForRateLimits(client.client_user.id, RateLimitBucketType::GLOBAL);

        websocket.sendText(json_payload);
    }

    void Client::SetCommandHandler(const std::function<void(discpp::Client*, discpp::Message)>& command_handler) {
        fire_command_method = command_handler;
    }

    void Shard::DisconnectWebsocket() {
        client.logger->Debug(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] Closing websocket connection...");

        websocket.close(ix::WebSocketCloseConstants::kNormalClosureCode);
        websocket.stop(ix::WebSocketCloseConstants::kNormalClosureCode);
    }

    void Shard::WebSocketStart() {
        client.logger->Debug(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] Connecting to gateway...");

        ix::initNetSystem();

        websocket.setUrl(gateway_endpoint);
        websocket.disableAutomaticReconnection();

        websocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
            OnWebSocketListen(const_cast<ix::WebSocketMessagePtr&>(msg));
        });

        /*ix::SocketTLSOptions tls_options;
        tls_options.certFile = "cacert.pem";
        websocket.setTLSOptions(tls_options);*/

        websocket.start();

        disconnected = false;
    }

    void Shard::HandleDiscordDisconnect(const ix::WebSocketMessagePtr& msg) {
        // if we're reconnecting this just stop here.
        if (reconnecting) {
            client.logger->Debug("[SHARD " + std::to_string(id) + "] Websocket was closed for reconnecting...");
            return;
        } else if (client.stay_disconnected) {
            client.logger->Warn(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] Websocket was closed.");
            return;
        } else {
            client.logger->Error(LogTextColor::RED + "[SHARD " + std::to_string(id) + "] Websocket was closed with error: " + std::to_string(msg->closeInfo.code) + ", " + msg->closeInfo.reason + "! Attempting reconnect...");
        }

        heartbeat_acked = false;
        disconnected = true;

        reconnecting = true;
        client.DoFunctionLater(&Shard::ReconnectToWebsocket, this);
    }

    void Shard::OnWebSocketListen(ix::WebSocketMessagePtr& msg) {
        switch(msg->type) {
            case ix::WebSocketMessageType::Open:
                client.logger->Info(LogTextColor::GREEN + "[SHARD " + std::to_string(id) + "] Connected to gateway!");
                disconnected = false;
                break;
            case ix::WebSocketMessageType::Close: {
                std::lock_guard<std::mutex> futures_guard(client.futures_mutex);
                client.futures.push_back(std::async(std::launch::async, &Shard::HandleDiscordDisconnect, this, std::move(msg)));
                break;
            } case ix::WebSocketMessageType::Error:
                client.logger->Error(LogTextColor::RED + "[SHARD " + std::to_string(id) + "] Error: " + msg->errorInfo.reason);
                break;
            case ix::WebSocketMessageType::Message:{
#ifdef RAPIDJSON_BACKEND
                rapidjson::Document json_result;
                json_result.Parse(msg->str);
                if (json_result.HasParseError()) {
                    client.logger->Debug(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] A non-json payload was received and ignored: \"" + msg->str);
                }

                if (json_result.IsNull()) break;

                discpp::JsonObject result(json_result);
#elif SIMDJSON_BACKEND

#endif
                OnWebSocketPacket(result);
                break;
            } default:
                client.logger->Warn(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] Unknown message sent");
                break;
        }
    }

    void Shard::OnWebSocketPacket(const discpp::JsonObject& result) {
        client.logger->Debug("[SHARD " + std::to_string(id) + "] Received payload: " + result.DumpJson());

        switch (result["op"].GetInt()) {
            case (Opcode::HELLO): {
                if (reconnecting) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
                    client.logger->Info(LogTextColor::GREEN + "[SHARD " + std::to_string(id) + "] Reconnected!");

#ifdef RAPIDJSON_BACKEND
                    // Send the resume payload
                    rapidjson::Document resume(rapidjson::kObjectType);
                    resume.AddMember("op", 6, resume.GetAllocator());

                    rapidjson::Value resume_d(rapidjson::kObjectType);
                    resume_d.AddMember("token", client.token, resume.GetAllocator());
                    resume_d.AddMember("session_id", session_id, resume.GetAllocator());
                    resume_d.AddMember("seq", last_sequence_number, resume.GetAllocator());

                    resume.AddMember("d", resume_d, resume.GetAllocator());

                    CreateWebsocketRequest(discpp::JsonObject(resume));
#elif SIMDJSON_BACKEND

#endif
                    heartbeat_acked = true;
                    reconnecting = false;

                    discpp::EventHandler<discpp::ReconnectEvent>::TriggerEvent(discpp::ReconnectEvent());
                } else {
                    hello_packet = result;

                    CreateWebsocketRequest(GetIdentifyPacket());
                }
                break;
            }
            case Opcode::HEARTBEAT_ACK:
                heartbeat_acked = true;
                break;
            case Opcode::RECONNECT:
                client.DoFunctionLater(&Shard::ReconnectToWebsocket, this);
                break;
            case Opcode::INVALID_SESSION:
#ifdef RAPIDJSON_BACKEND
                // Check if the session is resumable
                if (result["d"].GetBool()) {
#ifdef RAPIDJSON_BACKEND
                    // Send resume payload
                    rapidjson::Document resume(rapidjson::kObjectType);

                    rapidjson::Document::AllocatorType& allocator = resume.GetAllocator();
                    resume.AddMember("op", 6, allocator);

                    rapidjson::Value d(rapidjson::kObjectType);
                    d.AddMember("token", client.token, allocator);
                    d.AddMember("session_id", session_id, allocator);
                    d.AddMember("seq", last_sequence_number, resume.GetAllocator());

                    resume.AddMember("d", d, allocator);

                    CreateWebsocketRequest(discpp::JsonObject(resume));
#elif SIMDJSON_BACKEND

#endif
                } else {
                    client.logger->Debug("[SHARD " + std::to_string(id) + "] Waiting 2 seconds before sending an identify packet for invalid session.");
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    CreateWebsocketRequest(GetIdentifyPacket());
                }
#elif SIMDJSON_BACKEND

#endif
                break;
            default:
                EventDispatcher::HandleDiscordEvent(*this, result, result["t"].GetString());
                break;
        }

        packet_counter++;
    }

    void Shard::HandleHeartbeat() {
        try {
            while (client.run) {
                // Make sure that it doesn't try to do anything while its trying to reconnect.
                while (reconnecting && !client.run) {}

#ifdef RAPIDJSON_BACKEND
                rapidjson::Document data(rapidjson::kObjectType);
                data.AddMember("op", Opcode::HEARTBEAT, data.GetAllocator());
                data.AddMember("d", NULL, data.GetAllocator());
                if (last_sequence_number != -1) {
                    data["d"] = last_sequence_number;
                }

                discpp::JsonObject json(data);
                CreateWebsocketRequest(json, "[SHARD " + std::to_string(id) + "] Sending heartbeat payload: " + json.DumpJson());
#elif SIMDJSON_BACKEND

#endif

                heartbeat_acked = false;

                int heartbeat_interval = hello_packet["d"]["heartbeat_interval"].GetInt();
                client.logger->Debug("[SHARD " + std::to_string(id) + "] Waiting for next heartbeat (" + std::to_string(heartbeat_interval / 1000.0 - 10) + " seconds)...");

                // Wait for the required heartbeat interval, while waiting it should be acked from another thread.
                // This also checks it should stop this thread.
                if (!heartbeat_waiter.WaitFor(std::chrono::milliseconds(heartbeat_interval)) || !client.run) {
                    break;
                }

                if (!heartbeat_acked && !reconnecting) {
                    client.logger->Warn(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] Heartbeat wasn't acked, trying to reconnect...");
                    disconnected = true;

                    ReconnectToWebsocket();
                }
            }

            heartbeat_waiter.Kill();
        } catch (std::exception& e) {
            client.logger->Error(LogTextColor::RED + "[SHARD " + std::to_string(id) + "] [HEARTBEAT THREAD] Exception: " + e.what());
        }
    }

    discpp::JsonObject Shard::GetIdentifyPacket() {
#ifdef RAPIDJSON_BACKEND
        rapidjson::Document document(rapidjson::kObjectType);

        rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
        document.AddMember("op", Opcode::IDENTIFY, allocator);

        rapidjson::Value d(rapidjson::kObjectType);
        d.AddMember("token", client.token, allocator);

        rapidjson::Value properties(rapidjson::kObjectType);
        properties.AddMember("$os", GetOsName(), allocator);
        properties.AddMember("$browser", "DISCPP", allocator);
        properties.AddMember("$device", "DISCPP", allocator);

        d.AddMember("properties", properties, allocator);
        d.AddMember("compress", false, allocator);
        d.AddMember("large_threshold", 250, allocator);

        // We only want to add this if sharding is enabled.
        if (client.config->shard_amount > 1) {
            rapidjson::Value shard(rapidjson::kArrayType);
            shard.PushBack(id, allocator);
            shard.PushBack(client.config->shard_amount, allocator);

            d.AddMember("shard", shard, allocator);
        }

        document.AddMember("d", d, allocator);

        return discpp::JsonObject(document);
#elif SIMDJSON_BACKEND

#endif
    }

    void Shard::ReconnectToWebsocket() {
        client.logger->Info(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] Reconnecting to Discord gateway!");

        reconnecting = true;

        DisconnectWebsocket();
        WebSocketStart();
    }

    void Client::StopClient() {
        stay_disconnected = true;

        for (auto& shard : shards) {
            shard->DisconnectWebsocket();
        }

        run = false;

        for (auto& shard : shards) {
            if (shard->heartbeat_thread.joinable()) shard->heartbeat_thread.join();
        }
    }

    std::unordered_map<discpp::Snowflake, discpp::Channel> Client::GetUserDMs() {

        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("/users/@me/channels is a user only endpoint");
        } else {
            std::unordered_map<discpp::Snowflake, discpp::Channel> dm_channels;

            std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("users/@me/channels"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
            result->IterateThrough([&] (const discpp::JsonObject& channel_json)->bool {
                discpp::Channel tmp(channel_json);
                dm_channels.emplace(tmp.id, tmp);
                return true;
            });

            return dm_channels;
        }
    }

    std::vector<discpp::User::Connection> ClientUser::GetUserConnections() {
        std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/users/@me/connections"), DefaultHeaders(), id, RateLimitBucketType::GLOBAL);

        std::vector<Connection> connections;
        result->IterateThrough([&] (const discpp::JsonObject& connection_json)->bool {
            connections.emplace_back(connection_json);
            return true;
        });

        return connections;
    }

    ClientUser::ClientUser(const discpp::JsonObject& json) : User(json) {
        mfa_enabled = json.Get<bool>("mfa_enabled");
        locale = json.Get<std::string>("locale");
        verified = json.Get<bool>("verified");
        premium_type = static_cast<discpp::specials::NitroSubscription>(json.Get<int>("premium_type"));
    }

    ClientUserSettings ClientUser::GetSettings() {
        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/settings is a user only endpoint");
        }
        else {
            std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("users/@me/settings/"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
            ClientUserSettings user_settings(*result);
            this->settings = user_settings;
            return user_settings;
        }
    }

    void ClientUser::ModifySettings(ClientUserSettings& user_settings) {
        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/settings is a user only endpoint");
        }
        else {
            rapidjson::Document new_settings;
            new_settings.SetObject();

            rapidjson::Document::AllocatorType& allocator = new_settings.GetAllocator();
            ClientUserSettings old_settings = this->settings;
            if (user_settings.afk_timeout != old_settings.afk_timeout) new_settings.AddMember("afk_timeout", user_settings.afk_timeout, allocator);
            if (user_settings.custom_status != old_settings.custom_status) new_settings.AddMember("custom_status", user_settings.custom_status, allocator);
            if (user_settings.explicit_content_filter != old_settings.explicit_content_filter) new_settings.AddMember("explicit_content_filter", (int) user_settings.explicit_content_filter, allocator);
            if (user_settings.theme != old_settings.theme) new_settings.AddMember("theme", ThemeToString(user_settings.theme), allocator);
            rapidjson::Value friend_source_flags(rapidjson::kObjectType);
            bool add_friend_source_flags = false;
            if (user_settings.friend_source_flags.GetAll() != old_settings.friend_source_flags.GetAll()) {
                friend_source_flags.AddMember("all", user_settings.friend_source_flags.GetAll(), allocator);
                add_friend_source_flags = true;
            }
            if (user_settings.friend_source_flags.GetMutualFriends() != old_settings.friend_source_flags.GetMutualFriends()) {
                friend_source_flags.AddMember("mutual_friends", user_settings.friend_source_flags.GetMutualFriends(), allocator);
                add_friend_source_flags = true;
            }
            if (user_settings.friend_source_flags.GetMutualGuilds() != old_settings.friend_source_flags.GetMutualGuilds()) {
                friend_source_flags.AddMember("mutual_guilds", user_settings.friend_source_flags.GetMutualGuilds(), allocator);
                add_friend_source_flags = true;
            }
            if (add_friend_source_flags) new_settings.AddMember("friend_source_flags", friend_source_flags, allocator);
            if (user_settings.GetAllowAccessibilityDetection() != old_settings.GetAllowAccessibilityDetection()) new_settings.AddMember("allow_accessibility_detection", user_settings.GetAllowAccessibilityDetection(), allocator);
            if (user_settings.GetAnimateEmoji() != old_settings.GetAnimateEmoji()) new_settings.AddMember("animate_emoji", user_settings.GetAnimateEmoji(), allocator);
            if (user_settings.GetContactSyncEnabled() != old_settings.GetContactSyncEnabled()) new_settings.AddMember("contact_sync_enabled", user_settings.GetContactSyncEnabled(), allocator);
            if (user_settings.GetConvertEmoticons() != old_settings.GetConvertEmoticons()) new_settings.AddMember("convert_emoticons", user_settings.GetConvertEmoticons(), allocator);
            if (user_settings.GetDefaultGuildsRestricted() != old_settings.GetDefaultGuildsRestricted()) new_settings.AddMember("default_guilds_restricted", user_settings.GetDefaultGuildsRestricted(), allocator);
            if (user_settings.GetDetectPlatformAccounts() != old_settings.GetDetectPlatformAccounts()) new_settings.AddMember("detect_platform_accounts", user_settings.GetDetectPlatformAccounts(), allocator);
            if (user_settings.GetDeveloperMode() != old_settings.GetDeveloperMode()) new_settings.AddMember("developer_mode", user_settings.GetDeveloperMode(), allocator);
            if (user_settings.GetDisableGamesTab() != old_settings.GetDisableGamesTab()) new_settings.AddMember("disable_games_tab", user_settings.GetDisableGamesTab(), allocator);
            if (user_settings.GetEnableTtsCommand() != old_settings.GetEnableTtsCommand()) new_settings.AddMember("enable_tts_command", user_settings.GetEnableTtsCommand(), allocator);
            if (user_settings.GetGifAutoPlay() != old_settings.GetGifAutoPlay()) new_settings.AddMember("gif_auto_play", user_settings.GetGifAutoPlay(), allocator);
            if (user_settings.GetInlineAttachmentMedia() != old_settings.GetInlineAttachmentMedia()) new_settings.AddMember("inline_attachment_media", user_settings.GetInlineAttachmentMedia(), allocator);
            if (user_settings.GetInlineEmbedMedia() != old_settings.GetInlineEmbedMedia()) new_settings.AddMember("inline_embed_media", user_settings.GetInlineEmbedMedia(), allocator);
            if (user_settings.GetMessageDisplayCompact() != old_settings.GetMessageDisplayCompact()) new_settings.AddMember("message_display_compact", user_settings.GetMessageDisplayCompact(), allocator);
            if (user_settings.GetNativePhoneIntegrationEnabled() != old_settings.GetNativePhoneIntegrationEnabled()) new_settings.AddMember("native_phone_integration_enabled", user_settings.GetNativePhoneIntegrationEnabled(), allocator);
            if (user_settings.GetRenderEmbeds() != old_settings.GetRenderEmbeds()) new_settings.AddMember("render_embeds", user_settings.GetRenderEmbeds(), allocator);
            if (user_settings.GetRenderReactions() != old_settings.GetRenderReactions()) new_settings.AddMember("render_reactions", user_settings.GetRenderReactions(), allocator);
            if (user_settings.GetShowCurrentGame() != old_settings.GetShowCurrentGame()) new_settings.AddMember("show_current_game", user_settings.GetShowCurrentGame(), allocator);
            if (user_settings.GetStreamNotificationsEnabled() != old_settings.GetStreamNotificationsEnabled()) new_settings.AddMember("stream_notifications_enabled", user_settings.GetStreamNotificationsEnabled(), allocator);

#ifdef RAPIDJSON_BACKEND
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            new_settings.Accept(writer);
            cpr::Body body(buffer.GetString());
#elif SIMDJSON_BACKEND

#endif

            std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(Endpoint("users/@me/settings/"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL, body);
        }
    }

    void Client::AddFriend(const discpp::User& user) {
        if (!discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<discpp::JsonObject> result = SendPutRequest(Endpoint("users/@me/relationships/" + std::to_string(user.id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
        }
    }

    void Client::RemoveFriend(const discpp::User& user) {
        if(discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<discpp::JsonObject> result = SendDeleteRequest(Endpoint("users/@me/relationships/" + std::to_string(user.id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
        }
    }

    std::unordered_map<discpp::Snowflake, discpp::UserRelationship> Client::GetRelationships() {
        //todo implement this endpoint
        if(discpp::globals::client_instance->client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("users/@me/relationships/"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);

            std::unordered_map<discpp::Snowflake, discpp::UserRelationship> relationships;
            result->IterateThrough([&] (const discpp::JsonObject& relationship_json)->bool {
                discpp::UserRelationship tmp(relationship_json);
                relationships.emplace(tmp.id, tmp);
                return true;
            });

            return relationships;
        }
    }

    discpp::User Client::ModifyCurrentUser(const std::string& username, discpp::Image& avatar) {
        cpr::Body body("{\"username\": \"" + username + "\", \"avatar\": " + avatar.ToDataURI() + "}");
        std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(Endpoint("/users/@me"), DefaultHeaders(), 0, discpp::RateLimitBucketType::GLOBAL, body);

        client_user = discpp::ClientUser(*result);

        return client_user;
    }

    void Client::LeaveGuild(const discpp::Guild& guild) {
        SendDeleteRequest(Endpoint("/users/@me/guilds/" + std::to_string(guild.id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
    }

    void Client::UpdatePresence(discpp::Presence& presence) {
#ifdef RAPIDJSON_BACKEND
        rapidjson::Document payload(rapidjson::kObjectType);
        payload.AddMember("op", Shard::Opcode::STATUS_UPDATE, payload.GetAllocator());
        payload.AddMember("d", *presence.ToJson().GetRawJson(), payload.GetAllocator());

        shards.front()->CreateWebsocketRequest(discpp::JsonObject(payload));
#elif SIMDJSON_BACKEND

#endif
    }

    discpp::User Client::ReqestUserIfNotCached(const discpp::Snowflake& id) {
        discpp::User user(id);
        if (user.username.empty()) {
            std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/users/" + std::to_string(id)), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);
            return discpp::User(*result);
        }

        return user;
    }

    std::vector<discpp::User::Connection> Client::GetBotUserConnections() {
        std::unique_ptr<discpp::JsonObject> result = SendGetRequest(Endpoint("/users/@me/connections"), DefaultHeaders(), 0, RateLimitBucketType::GLOBAL);

        std::vector<discpp::User::Connection> connections;
        result->IterateThrough([&] (const discpp::JsonObject& connection_json)->bool {
            connections.emplace_back(connection_json);
            return true;
        });

        return connections;
    }

    UserRelationship::UserRelationship(const discpp::JsonObject& json) {
        id = SnowflakeFromString(json["id"].GetString());
        nickname = json.Get<std::string>("nickname");
        type = json["type"].GetInt();
        user = json.ConstructDiscppObjectFromJson("user", discpp::User());
    }

    bool UserRelationship::IsFriend() {
        return type == 1;
    }

    bool UserRelationship::IsBlocked() {
        return type == 2;
    }
}