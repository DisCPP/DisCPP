#include "event_dispatcher.h"
#include "event_handler.h"
#include "events/all_discord_events.h"
#include "client_config.h"

namespace discpp {
    void EventDispatcher::ReadyEvent(rapidjson::Document& result) {
        // Check if we're just resuming, and if we are dont try to create a new thread.
        if (!discpp::globals::client_instance->heartbeat_thread.joinable()) {
            discpp::globals::client_instance->heartbeat_thread = std::thread{ &Client::HandleHeartbeat_1, discpp::globals::client_instance };
        }

        discpp::globals::client_instance->ready = true;
        discpp::globals::client_instance->session_id = result["session_id"].GetString();

        // Get the bot user 
        nlohmann::json user_json = SendGetRequest(Endpoint("/users/@me"), DefaultHeaders(), {}, {});
        discpp::globals::client_instance->client_user = discpp::User(user_json);

        discpp::DispatchEvent(discpp::ReadyEvent());
    }

    void EventDispatcher::ResumedEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::ResumedEvent());
    }

    void EventDispatcher::ReconnectEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::ReconnectEvent());
    }

    void EventDispatcher::InvalidSessionEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::InvalidSessionEvent());
    }

    void EventDispatcher::ChannelCreateEvent(rapidjson::Document& result) {
        discpp::Channel new_channel = discpp::Channel(result);
        discpp::globals::client_instance->channels.insert(std::pair<snowflake, Channel>(static_cast<snowflake>(new_channel.id),
            static_cast<Channel>(new_channel)));

        discpp::DispatchEvent(discpp::ChannelCreateEvent(new_channel));
    }

    void EventDispatcher::ChannelUpdateEvent(rapidjson::Document& result) {
        discpp::Channel new_channel = discpp::Channel(result);

        std::unordered_map<snowflake, Channel>::iterator it = discpp::globals::client_instance->channels.find(new_channel.id);
        if (it != discpp::globals::client_instance->channels.end()) {
            it->second = new_channel;
        }

        discpp::DispatchEvent(discpp::ChannelUpdateEvent(new_channel));
    }

    void EventDispatcher::ChannelDeleteEvent(rapidjson::Document& result) {
        discpp::globals::client_instance->channels.erase(result["id"].get<snowflake>());

        discpp::DispatchEvent(discpp::ChannelDeleteEvent(discpp::Channel(result)));
    }

    void EventDispatcher::ChannelPinsUpdateEvent(rapidjson::Document& result) {
        discpp::Channel new_channel = discpp::Channel(result["channel_id"].get<snowflake>());
        new_channel.last_pin_timestamp = GetDataSafely<std::string>(result, "last_pin_timestamp");

        std::unordered_map<snowflake, Channel>::iterator it = discpp::globals::client_instance->channels.find(new_channel.id);
        if (it != discpp::globals::client_instance->channels.end()) {
            it->second = new_channel;
        }

        discpp::DispatchEvent(discpp::ChannelPinsUpdateEvent(new_channel));
    }

    void EventDispatcher::GuildCreateEvent(rapidjson::Document& result) {
        snowflake guild_id = result["id"].get<snowflake>();
        discpp::Guild guild(result);
        //discpp::globals::client_instance->logger->LogToConsole(LogSeverity::SEV_INFO, LogTextColor::GREEN + "Connected to " + guild.name);
        discpp::globals::client_instance->guilds.insert(std::pair<snowflake, Guild>(static_cast<snowflake>(guild.id), static_cast<Guild>(guild)));

        discpp::globals::client_instance->members.insert(guild.members.begin(), guild.members.end());

        for (auto& channel : result["channels"]) {
            discpp::GuildChannel _channel = GuildChannel(channel);
            _channel.guild_id = guild_id;
            discpp::globals::client_instance->channels.insert(std::pair<snowflake, Channel>(static_cast<snowflake>(_channel.id), static_cast<Channel>(_channel)));
        }

        discpp::DispatchEvent(discpp::GuildCreateEvent(guild));
    }

    void EventDispatcher::GuildUpdateEvent(rapidjson::Document& result) {
        discpp::Guild guild(result);
        std::unordered_map<snowflake, Guild>::iterator it = discpp::globals::client_instance->guilds.find(guild.id);
        if (it != discpp::globals::client_instance->guilds.end()) {
            it->second = guild;
        }
        discpp::DispatchEvent(discpp::GuildUpdateEvent(guild));
    }

    void EventDispatcher::GuildDeleteEvent(rapidjson::Document& result) {
        discpp::Guild guild;
        guild.id = result["id"].get<snowflake>();
        guild.unavailable = true;
        discpp::globals::client_instance->guilds.erase(guild.id);

        discpp::DispatchEvent(discpp::GuildDeleteEvent(guild));
    }

    void EventDispatcher::GuildBanAddEvent(rapidjson::Document& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::User user(result["user"]);

        discpp::DispatchEvent(discpp::GuildBanAddEvent(guild, user));
    }

    void EventDispatcher::GuildBanRemoveEvent(rapidjson::Document& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::User user(result["user"]);

        discpp::DispatchEvent(discpp::GuildBanRemoveEvent(guild, user));
    }

    void EventDispatcher::GuildEmojisUpdateEvent(rapidjson::Document& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        std::unordered_map<snowflake, Emoji> emojis;
        for (nlohmann::json emoji : result["emojis"]) {
            discpp::Emoji tmp = discpp::Emoji(emoji);
            emojis.insert(std::pair<snowflake, Emoji>(static_cast<snowflake>(tmp.id), static_cast<Emoji>(tmp)));
        }

        guild.emojis = emojis;
        std::unordered_map<snowflake, Guild>::iterator it = discpp::globals::client_instance->guilds.find(guild.id);
        if (it != discpp::globals::client_instance->guilds.end()) {
            it->second = guild;
        }

        discpp::DispatchEvent(discpp::GuildEmojisUpdateEvent(guild));
    }

    void EventDispatcher::GuildIntegrationsUpdateEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(
            discpp::GuildIntegrationsUpdateEvent(discpp::Guild(result["guild_id"].get<snowflake>())));
    }

    void EventDispatcher::GuildMemberAddEvent(rapidjson::Document& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::Member member(result, guild.id);
        discpp::globals::client_instance->members.insert(std::pair<snowflake, Member>(static_cast<snowflake>(member.id), static_cast<Member>(member)));

        discpp::DispatchEvent(discpp::GuildMemberAddEvent(guild, member));
    }

    void EventDispatcher::GuildMemberRemoveEvent(rapidjson::Document& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::Member member(result["user"]["id"].get<snowflake>(), guild);
        discpp::globals::client_instance->members.erase(member.id);

        discpp::DispatchEvent(discpp::GuildMemberRemoveEvent(guild, member));
    }

    void EventDispatcher::GuildMemberUpdateEvent(rapidjson::Document& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        std::unordered_map<snowflake, Member>::iterator it = guild.members.find(result["user"]["id"]);
        discpp::Member member;
        if (it != guild.members.end()) {
            member = it->second;
        }
        else {
            member = discpp::Member(result["user"]["id"].get<snowflake>(), guild);
            guild.members.insert(std::pair<snowflake, Member>(static_cast<snowflake>(member.id), static_cast<discpp::Member>(member)));
        }
        member.roles.clear();
        for (auto role : result["roles"]) {
            member.roles.push_back(discpp::Role(role, guild));
        }
        if (result.contains("nick") && !result["nick"].is_null()) {
            member.nick = result["nick"];
        }
        discpp::DispatchEvent(discpp::GuildMemberUpdateEvent(guild, member));
    }

    void EventDispatcher::GuildMembersChunkEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::GuildMembersChunkEvent());
    }

    void EventDispatcher::GuildRoleCreateEvent(rapidjson::Document& result) {
        discpp::Role role(result["role"]);

        discpp::DispatchEvent(discpp::GuildRoleCreateEvent(role));
    }

    void EventDispatcher::GuildRoleUpdateEvent(rapidjson::Document& result) {
        discpp::Role role(result["role"]);

        discpp::DispatchEvent(discpp::GuildRoleUpdateEvent(role));
    }

    void EventDispatcher::GuildRoleDeleteEvent(rapidjson::Document& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::Role role(result["role"]);
        guild.roles.erase(role.id);

        discpp::DispatchEvent(discpp::GuildRoleDeleteEvent(role));
    }

    void EventDispatcher::MessageCreateEvent(rapidjson::Document& result) {
        discpp::Message message(result);
        if (discpp::globals::client_instance->messages.size() >= discpp::globals::client_instance->message_cache_count) {
            discpp::globals::client_instance->messages.erase(discpp::globals::client_instance->messages.begin());
        }
        discpp::globals::client_instance->messages.insert({ message.id, message });

        if (discpp::globals::client_instance->config->type == discpp::TokenType::BOT) {
            discpp::globals::client_instance->DoFunctionLater(discpp::globals::client_instance->fire_command_method, discpp::globals::client_instance, message);
        }

        discpp::DispatchEvent(discpp::MessageCreateEvent(message));
    }

    void EventDispatcher::MessageUpdateEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(result["id"].get<snowflake>());

        if (message != discpp::globals::client_instance->messages.end()) {
            if (discpp::globals::client_instance->messages.size() >= discpp::globals::client_instance->message_cache_count) {
                discpp::globals::client_instance->messages.erase(discpp::globals::client_instance->messages.begin());
            }

            discpp::Message old_message(result["id"].get<snowflake>());
            /*std::replace_if(messages.begin(), messages.end(),
                            [message](discpp::Message &msg) { return msg.id == message.id; }, message);*/
            bool is_edited = result.contains("edited_timestamp") && !result["edited_timestamp"].is_null();

            discpp::DispatchEvent(discpp::MessageUpdateEvent(message->second, old_message, is_edited));
        }
    }

    void EventDispatcher::MessageDeleteEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(result["id"].get<snowflake>());

        if (message != discpp::globals::client_instance->messages.end()) {
            discpp::DispatchEvent(discpp::MessageDeleteEvent(message->second));

            discpp::globals::client_instance->messages.erase(message);
        }
    }

    void EventDispatcher::MessageDeleteBulkEvent(rapidjson::Document& result) {
        std::vector<discpp::Message> msgs;
        for (auto id : result["ids"]) {
            auto message = discpp::globals::client_instance->messages.find(id.get<snowflake>());

            if (message != discpp::globals::client_instance->messages.end()) {
                message->second.channel = discpp::Channel(result["channel_id"].get<snowflake>());
                if (result.contains("guild_id")) {
                    message->second.guild = discpp::Guild(result["guild_id"].get<snowflake>());
                }

                msgs.push_back(message->second);
            }
        }

        for (discpp::Message message : msgs) {
            discpp::globals::client_instance->messages.erase(message.id);
        }

        discpp::DispatchEvent(discpp::MessageBulkDeleteEvent(msgs));
    }

    void EventDispatcher::MessageReactionAddEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(result["message_id"].get<snowflake>());

        if (message != discpp::globals::client_instance->messages.end()) {
            discpp::Channel channel = discpp::globals::client_instance->channels.find(result["channel_id"].get<snowflake>())->second;
            message->second.channel = channel;

            if (result.contains("guild_id")) {
                message->second.guild = discpp::Guild(result["guild_id"].get<snowflake>());
            }

            discpp::Emoji emoji(result["emoji"]);
            discpp::User user(result["user_id"].get<snowflake>());

            auto reaction = std::find_if(message->second.reactions.begin(), message->second.reactions.end(),
                [emoji](discpp::Reaction react) {
                    return react.emoji.name == emoji.name || (!react.emoji.id.empty() &&
                        react.emoji.id == emoji.id);
                });

            if (reaction != message->second.reactions.end()) {
                reaction->count++;

                if (user.bot) {
                    reaction->from_bot = true;
                }
            }
            else {
                discpp::Reaction r = discpp::Reaction(1, user.bot, emoji);
                message->second.reactions.push_back(r);
            }

            discpp::DispatchEvent(discpp::MessageReactionAddEvent(message->second, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(result["message_id"].get<snowflake>());

        if (message != discpp::globals::client_instance->messages.end()) {
            discpp::Channel channel = discpp::globals::client_instance->channels.find(result["channel_id"].get<snowflake>())->second;
            message->second.channel = channel;

            discpp::Emoji emoji(result["emoji"]);
            discpp::User user(result["user_id"].get<snowflake>());

            auto reaction = std::find_if(message->second.reactions.begin(), message->second.reactions.end(),
                [emoji](discpp::Reaction react) {
                    return react.emoji.name == emoji.name || react.emoji.id == emoji.id;
                });

            if (reaction != message->second.reactions.end()) {
                if (reaction->count == 1) {
                    message->second.reactions.erase(reaction);
                }
                else {
                    reaction->count--;

                    // @TODO: Add a way to change reaction::from_bot
                }
            }

            discpp::DispatchEvent(discpp::MessageReactionRemoveEvent(message->second, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveAllEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(result["message_id"].get<snowflake>());

        if (message != discpp::globals::client_instance->messages.end()) {
            discpp::Channel channel = discpp::globals::client_instance->channels.find(result["channel_id"].get<snowflake>())->second;
            message->second.channel = channel;

            discpp::DispatchEvent(discpp::MessageReactionRemoveAllEvent(message->second));
        }
    }

    void EventDispatcher::PresenceUpdateEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::PresenseUpdateEvent(discpp::User(result["user"])));
    }

    void EventDispatcher::TypingStartEvent(rapidjson::Document& result) {
        discpp::User user(result["user_id"].get<snowflake>());
        discpp::Channel channel(result["channel_id"].get<snowflake>());
        int timestamp = result["timestamp"].get<int>();

        discpp::DispatchEvent(discpp::TypingStartEvent(user, channel, timestamp));
    }

    void EventDispatcher::UserUpdateEvent(rapidjson::Document& result) {
        discpp::User user(result);

        discpp::DispatchEvent(discpp::UserUpdateEvent(user));
    }

    void EventDispatcher::VoiceStateUpdateEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::VoiceStateUpdateEvent(result));
    }

    void EventDispatcher::VoiceServerUpdateEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::VoiceServerUpdateEvent(result));
    }

    void EventDispatcher::WebhooksUpdateEvent(rapidjson::Document& result) {
        discpp::GuildChannel channel(result["channel_id"].get<snowflake>());
        channel.guild_id = result["guild_id"].get<snowflake>();

        discpp::DispatchEvent(discpp::WebhooksUpdateEvent(channel));
    }

    void EventDispatcher::BindEvents() {
        internal_event_map["READY"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::ReadyEvent(result); };
        internal_event_map["RESUMED"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::ChannelCreateEvent(result); };
        internal_event_map["INVALID_SESSION"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::InvalidSessionEvent(result); };
        internal_event_map["CHANNEL_CREATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::ChannelCreateEvent(result); };
        internal_event_map["CHANNEL_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::ChannelUpdateEvent(result); };
        internal_event_map["CHANNEL_DELETE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::ChannelDeleteEvent(result); };
        internal_event_map["CHANNEL_PINS_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::ChannelPinsUpdateEvent(result); };
        internal_event_map["GUILD_CREATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildCreateEvent(result); };
        internal_event_map["GUILD_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildUpdateEvent(result); };
        internal_event_map["GUILD_DELETE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildDeleteEvent(result); };
        internal_event_map["GUILD_BAN_ADD"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildBanAddEvent(result); };
        internal_event_map["GUILD_BAN_REMOVE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildBanRemoveEvent(result); };
        internal_event_map["GUILD_EMOJIS_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildEmojisUpdateEvent(result); };
        internal_event_map["GUILD_INTEGRATIONS_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildIntegrationsUpdateEvent(result); };
        internal_event_map["GUILD_MEMBER_ADD"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildMemberAddEvent(result); };
        internal_event_map["GUILD_MEMBER_REMOVE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildMemberRemoveEvent(result); };
        internal_event_map["GUILD_MEMBER_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildMemberUpdateEvent(result); };
        internal_event_map["GUILD_MEMBERS_CHUNK"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildMembersChunkEvent(result); };
        internal_event_map["GUILD_ROLE_CREATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildRoleCreateEvent(result); };
        internal_event_map["GUILD_ROLE_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildRoleUpdateEvent(result); };
        internal_event_map["GUILD_ROLE_DELETE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::GuildRoleDeleteEvent(result); };
        internal_event_map["MESSAGE_CREATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::MessageCreateEvent(result); };
        internal_event_map["MESSAGE_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::MessageUpdateEvent(result); };
        internal_event_map["MESSAGE_DELETE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::MessageDeleteEvent(result); };
        internal_event_map["MESSAGE_DELETE_BULK"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::MessageDeleteBulkEvent(result); };
        internal_event_map["MESSAGE_REACTION_ADD"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::MessageReactionAddEvent(result); };
        internal_event_map["MESSAGE_REACTION_REMOVE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::MessageReactionRemoveEvent(result); };
        internal_event_map["MESSAGE_REACTION_REMOVE_ALL"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::MessageReactionRemoveAllEvent(result); };
        internal_event_map["PRESENCE_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::PresenceUpdateEvent(result); };
        internal_event_map["TYPING_START"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::TypingStartEvent(result); };
        internal_event_map["USER_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::UserUpdateEvent(result); };
        internal_event_map["VOICE_STATE_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::VoiceStateUpdateEvent(result); };
        internal_event_map["VOICE_SERVER_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::VoiceServerUpdateEvent(result); };
        internal_event_map["WEBHOOKS_UPDATE"] = [&](rapidjson::Document& result) { discpp::EventDispatcher::WebhooksUpdateEvent(result); };
    }

    void EventDispatcher::RunEvent(const std::function<void(rapidjson::Document &)>& func, rapidjson::Document& json) {
        func(json);
    }

    void EventDispatcher::HandleDiscordEvent(rapidjson::Document& j, std::string event_name) {
        nlohmann::json& data = (nlohmann::json &) j["d"];
        discpp::globals::client_instance->last_sequence_number = (j.contains("s") && j["s"].is_number()) ? j["s"].get<int>() : -1;

        if (internal_event_map.find(event_name) != internal_event_map.end()) {
            // Ignore this intellisense error, it compiles fine, and works.
            globals::client_instance->DoFunctionLater(internal_event_map[event_name], data);
        }
    }
}