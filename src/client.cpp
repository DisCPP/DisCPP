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
#include "event_handler.h"

#include <ixwebsocket/IXNetSystem.h>

namespace discpp {
    uint8_t Client::next_instance_id = 0;
    std::map<uint8_t, Client*> Client::client_instances;

    Client::Client(const std::string& token, ClientConfig* config) : token(token), config(config), cache(discpp::Cache(this)), event_handler(new discpp::EventHandler(this)) {
        fire_command_method = std::bind(discpp::FireCommand, std::placeholders::_1, std::placeholders::_2);

        message_cache_count = config->message_cache_size;

        if (config->logger_path.empty()) {
            logger = new discpp::Logger(config->logger_flags);
        } else {
            logger = new discpp::Logger(config->logger_path, config->logger_flags);
        }

        this->my_instance_id = next_instance_id;
        next_instance_id++;

        client_instances.emplace(my_instance_id, this);
    }

    int Client::Run() {
        EventDispatcher::BindEvents();

        DoFunctionLater([&] {
            rapidjson::Document gateway_request(rapidjson::kObjectType);
            switch (config->type) {
                case TokenType::USER: {
                    std::unique_ptr<rapidjson::Document> user_doc = SendGetRequest(this, Endpoint("/gateway"), {{"Authorization", token}, {"User-Agent", "discpp (https://github.com/DisCPP/DisCPP, v0.0.0)"}}, {}, {});
                    gateway_request.CopyFrom(*user_doc, gateway_request.GetAllocator());

                    break;
                } case TokenType::BOT:
                    std::unique_ptr<rapidjson::Document> bot_doc = SendGetRequest(this, Endpoint("/gateway/bot"), { {"Authorization", "Bot " + token}, {"User-Agent", "discpp (https://github.com/DisCPP/DisCPP, v0.0.0)"} }, {}, {});
                    gateway_request.CopyFrom(*bot_doc, gateway_request.GetAllocator());

                    break;
            }

            if (ContainsNotNull(gateway_request, "url")) {
                if (ContainsNotNull(gateway_request, "session_start_limit") &&
                    gateway_request["session_start_limit"]["remaining"].GetInt() == 0) {

                    logger->Debug(LogTextColor::RED + "GATEWAY ERROR: Maximum start limit reached");
                    throw exceptions::MaximumLimitException("Gateway start limit exceeded!");
                }

                if (ContainsNotNull(gateway_request, "shards")) {
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

    void Shard::CreateWebsocketRequest(rapidjson::Document& json, const std::string& message) {
        std::string json_payload = DumpJson(json);

        if (message.empty()) {
            client.logger->Debug("[SHARD " + std::to_string(id) + "] Sending gateway payload: " + json_payload);
        } else {
            client.logger->Debug(message);
        }

        WaitForRateLimits(&client, client.client_user.id, RateLimitBucketType::GLOBAL);

        //std::lock_guard<std::mutex> lock = std::lock_guard(websocket_client_mutex);
        websocket.sendText(json_payload);
    }

    void Client::SetCommandHandler(const std::function<void(discpp::Shard&, discpp::Message&)>& command_handler) {
        fire_command_method = std::bind(command_handler, std::placeholders::_1, std::placeholders::_2);
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
                rapidjson::Document result;
                result.Parse(msg->str);
                if (result.HasParseError()) client.logger->Debug(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] A non-json payload was received and ignored: \"" + msg->str);
                if (!result.IsNull()) OnWebSocketPacket(result);
                break;
            } default:
                client.logger->Warn(LogTextColor::YELLOW + "[SHARD " + std::to_string(id) + "] Unknown message sent");
                break;
        }
    }

    void Shard::OnWebSocketPacket(rapidjson::Document& result) {
        client.logger->Debug("[SHARD " + std::to_string(id) + "] Received payload: " + DumpJson(result));

        switch (result["op"].GetInt()) {
            case (Opcode::HELLO): {
                if (reconnecting) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
                    client.logger->Info(LogTextColor::GREEN + "[SHARD " + std::to_string(id) + "] Reconnected!");

                    // Send the resume payload
                    rapidjson::Document resume(rapidjson::kObjectType);
                    resume.AddMember("op", 6, resume.GetAllocator());

                    rapidjson::Value resume_d(rapidjson::kObjectType);
                    resume_d.AddMember("token", client.token, resume.GetAllocator());
                    resume_d.AddMember("session_id", session_id, resume.GetAllocator());
                    resume_d.AddMember("seq", last_sequence_number, resume.GetAllocator());

                    resume.AddMember("d", resume_d, resume.GetAllocator());

                    CreateWebsocketRequest(resume);

                    heartbeat_acked = true;
                    reconnecting = false;

                    client.event_handler->TriggerEvent<discpp::ReconnectEvent>(discpp::ReconnectEvent(*this));
                } else {
                    hello_packet.SetObject();
                    hello_packet.CopyFrom(result, hello_packet.GetAllocator());

                    CreateWebsocketRequest(*GetIdentifyPacket());
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
                // Check if the session is resumable
                if (result["d"].GetBool()) {
                    // Send resume payload
                    rapidjson::Document resume(rapidjson::kObjectType);

                    rapidjson::Document::AllocatorType& allocator = resume.GetAllocator();
                    resume.AddMember("op", 6, allocator);

                    rapidjson::Value d(rapidjson::kObjectType);
                    d.AddMember("token", client.token, allocator);
                    d.AddMember("session_id", session_id, allocator);
                    d.AddMember("seq", last_sequence_number, resume.GetAllocator());

                    resume.AddMember("d", d, allocator);

                    CreateWebsocketRequest(resume);
                } else {
                    client.logger->Debug("[SHARD " + std::to_string(id) + "] Waiting 2 seconds before sending an identify packet for invalid session.");
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                    CreateWebsocketRequest(*GetIdentifyPacket());
                }

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

                rapidjson::Document data(rapidjson::kObjectType);
                data.AddMember("op", Opcode::HEARTBEAT, data.GetAllocator());
                data.AddMember("d", NULL, data.GetAllocator());
                if (last_sequence_number != -1) {
                    data["d"] = last_sequence_number;
                }

                std::string json_payload = DumpJson(data);
                CreateWebsocketRequest(data, "[SHARD " + std::to_string(id) + "] Sending heartbeat payload: " + json_payload);

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

    std::unique_ptr<rapidjson::Document> Shard::GetIdentifyPacket() {
        auto document = std::make_unique<rapidjson::Document>(rapidjson::kObjectType);

        rapidjson::Document::AllocatorType& allocator = document->GetAllocator();
        document->AddMember("op", Opcode::IDENTIFY, allocator);

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

        document->AddMember("d", d, allocator);

        return document;
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

        if (client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("/users/@me/channels is a user only endpoint");
        } else {
            std::unordered_map<discpp::Snowflake, discpp::Channel> dm_channels;

            std::unique_ptr<rapidjson::Document> result = SendGetRequest(this, Endpoint("users/@me/channels"), DefaultHeaders(this), 0, RateLimitBucketType::GLOBAL);
            for (auto const& channel : result->GetArray()) {
                rapidjson::Document channel_json(rapidjson::kObjectType);
                channel_json.CopyFrom(channel, channel_json.GetAllocator());

                discpp::Channel tmp(this, channel_json);
                dm_channels.emplace(tmp.id, tmp);
            }

            return dm_channels;
        }
    }

    std::vector<discpp::User::Connection> ClientUser::GetUserConnections() {
        discpp::Client* client = GetClient();
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("/users/@me/connections"), DefaultHeaders(client), id, RateLimitBucketType::GLOBAL);

        std::vector<Connection> connections;
        for (auto const& connection : result->GetArray()) {
            rapidjson::Document connection_json;
            connection_json.CopyFrom(connection, connection_json.GetAllocator());
            connections.emplace_back(connection_json);
        }

        return connections;
    }

    ClientUser::ClientUser(discpp::Client* client, rapidjson::Document& json) : User(client, json) {
        mfa_enabled = GetDataSafely<bool>(json, "mfa_enabled");
        locale = GetDataSafely<std::string>(json, "locale");
        verified = GetDataSafely<bool>(json, "verified");
        premium_type = static_cast<discpp::specials::NitroSubscription>(GetDataSafely<int>(json, "premium_type"));
    }

    ClientUserSettings ClientUser::GetSettings() {
        if (IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/settings is a user only endpoint");
        } else {
            discpp::Client* client = GetClient();
            std::unique_ptr<rapidjson::Document> result = SendGetRequest(client, Endpoint("users/@me/settings/"), DefaultHeaders(client), 0, RateLimitBucketType::GLOBAL);
            ClientUserSettings user_settings(*result);
            this->settings = user_settings;
            return user_settings;
        }
    }

    void ClientUser::ModifySettings(ClientUserSettings& user_settings) {
        if (IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/settings is a user only endpoint");
        } else {
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

            discpp::Client* client = GetClient();
            std::unique_ptr<rapidjson::Document> result = SendPatchRequest(client, Endpoint("users/@me/settings/"), DefaultHeaders(client), 0, RateLimitBucketType::GLOBAL, cpr::Body(DumpJson(new_settings)));
        }
    }

    void Client::AddFriend(const discpp::User& user) {
        if (client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendPutRequest(this, Endpoint("users/@me/relationships/" + std::to_string(user.id)), DefaultHeaders(this), 0, RateLimitBucketType::GLOBAL);
        }
    }

    void Client::RemoveFriend(const discpp::User& user) {
        if (client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unique_ptr<rapidjson::Document> result = SendDeleteRequest(this, Endpoint("users/@me/relationships/" + std::to_string(user.id)), DefaultHeaders(this), 0, RateLimitBucketType::GLOBAL);
        }
    }

    std::unordered_map<discpp::Snowflake, discpp::UserRelationship> Client::GetRelationships() {
        //todo implement this endpoint
        if (client_user.IsBot()) {
            throw exceptions::ProhibitedEndpointException("users/@me/relationships is a user only endpoint");
        } else {
            std::unordered_map<discpp::Snowflake, discpp::UserRelationship> relationships;

            std::unique_ptr<rapidjson::Document> result = SendGetRequest(this, Endpoint("users/@me/relationships/"), DefaultHeaders(this), 0, RateLimitBucketType::GLOBAL);
            for (auto const& relationship : result->GetArray()) {
                rapidjson::Document relationship_json(rapidjson::kObjectType);
                relationship_json.CopyFrom(relationship, relationship_json.GetAllocator());

                discpp::UserRelationship tmp(this, relationship_json);
                relationships.emplace(tmp.id, tmp);
            }
            return relationships;
        }
    }

    discpp::User Client::ModifyCurrentUser(const std::string& username, discpp::Image& avatar) {
        cpr::Body body("{\"username\": \"" + username + "\", \"avatar\": " + avatar.ToDataURI() + "}");
        std::unique_ptr<rapidjson::Document> result = SendPatchRequest(this, Endpoint("/users/@me"), DefaultHeaders(this), 0, discpp::RateLimitBucketType::GLOBAL, body);

        client_user = discpp::ClientUser(this, *result);

        return client_user;
    }

    void Client::LeaveGuild(const discpp::Guild& guild) {
        SendDeleteRequest(this, Endpoint("/users/@me/guilds/" + std::to_string(guild.id)), DefaultHeaders(this), 0, RateLimitBucketType::GLOBAL);
    }

    void Client::UpdatePresence(discpp::Presence& presence) {
        rapidjson::Document payload(rapidjson::kObjectType);
        std::unique_ptr<rapidjson::Document> activity_json = presence.ToJson();

        payload.AddMember("op", Shard::Opcode::STATUS_UPDATE, payload.GetAllocator());
        payload.AddMember("d", *activity_json, payload.GetAllocator());

        shards.front()->CreateWebsocketRequest(payload);
    }

    discpp::User Client::ReqestUserIfNotCached(const discpp::Snowflake& id) {
        discpp::User user(this, id);
        if (user.username.empty()) {
            std::unique_ptr<rapidjson::Document> result = SendGetRequest(this, Endpoint("/users/" + std::to_string(id)), DefaultHeaders(this), 0, RateLimitBucketType::GLOBAL);
            return discpp::User(this, *result);
        }

        return user;
    }

    std::vector<discpp::User::Connection> Client::GetBotUserConnections() {
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(this, Endpoint("/users/@me/connections"), DefaultHeaders(this), 0, RateLimitBucketType::GLOBAL);
        std::vector<discpp::User::Connection> connections;
        for (auto const& connection : result->GetArray()) {
            rapidjson::Document connection_json;
            connection_json.CopyFrom(connection, connection_json.GetAllocator());
            connections.emplace_back(connection_json);
        }

        return connections;
    }

    Client* Client::GetInstance(uint8_t id) {
        auto it = client_instances.find(id);

        if (it != client_instances.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Failed to find client instance with id: " + std::to_string(id));
        }
    }

    uint8_t Client::GetInstanceID() {
        return my_instance_id;
    }

    UserRelationship::UserRelationship(discpp::Client* client, rapidjson::Document& json) {
        id = SnowflakeFromString(json["id"].GetString());
        nickname = GetDataSafely<std::string>(json, "nickname");
        type = json["type"].GetInt();

        rapidjson::Document doc;
        doc.CopyFrom(json["user"], doc.GetAllocator());
        user = discpp::User(client, doc);
    }

    bool UserRelationship::IsFriend() {
        return type == 1;
    }

    bool UserRelationship::IsBlocked() {
        return type == 2;
    }
}
