#include "discpp/event_dispatcher.h"
#include "discpp/event_handler.h"
#include "events/all_discord_events.h"

#include "events/all_discord_events.h"

namespace discpp {
    void EventDispatcher::ReadyEvent(nlohmann::json& result) {
        // Check if we're just resuming, and if we are dont try to create a new thread.
        if (!discpp::globals::bot_instance->heartbeat_thread.joinable()) {
            discpp::globals::bot_instance->heartbeat_thread = std::thread{ &Bot::HandleHeartbeat, discpp::globals::bot_instance };
        }

        discpp::globals::bot_instance->ready = true;
        discpp::globals::bot_instance->session_id = result["session_id"];

        // Get the bot user 
        nlohmann::json user_json = SendGetRequest(Endpoint("/users/@me"), DefaultHeaders(), {}, {});
        discpp::globals::bot_instance->bot_user = discpp::User(user_json);

        discpp::DispatchEvent(discpp::ReadyEvent());
    }

    void EventDispatcher::ResumedEvent(nlohmann::json& result) {
        discpp::DispatchEvent(discpp::ResumedEvent());
    }

    void EventDispatcher::ReconnectEvent(nlohmann::json& result) {
        discpp::DispatchEvent(discpp::ReconnectEvent());
    }

    void EventDispatcher::InvalidSessionEvent(nlohmann::json& result) {
        discpp::DispatchEvent(discpp::InvalidSessionEvent());
    }

    void EventDispatcher::ChannelCreateEvent(nlohmann::json& result) {
        discpp::Channel new_channel = discpp::Channel(result);
        discpp::globals::bot_instance->channels.insert(std::pair<snowflake, Channel>(static_cast<snowflake>(new_channel.id),
            static_cast<Channel>(new_channel)));

        discpp::DispatchEvent(discpp::ChannelCreateEvent(new_channel));
    }

    void EventDispatcher::ChannelUpdateEvent(nlohmann::json& result) {
        discpp::Channel new_channel = discpp::Channel(result);

        std::unordered_map<snowflake, Channel>::iterator it = discpp::globals::bot_instance->channels.find(new_channel.id);
        if (it != discpp::globals::bot_instance->channels.end()) {
            it->second = new_channel;
        }

        discpp::DispatchEvent(discpp::ChannelUpdateEvent(new_channel));
    }

    void EventDispatcher::ChannelDeleteEvent(nlohmann::json& result) {
        discpp::globals::bot_instance->channels.erase(result["id"].get<snowflake>());

        discpp::DispatchEvent(discpp::ChannelDeleteEvent(discpp::Channel(result)));
    }

    void EventDispatcher::ChannelPinsUpdateEvent(nlohmann::json& result) {
        discpp::Channel new_channel = discpp::Channel(result["channel_id"].get<snowflake>());
        new_channel.last_pin_timestamp = GetDataSafely<std::string>(result, "last_pin_timestamp");

        std::unordered_map<snowflake, Channel>::iterator it = discpp::globals::bot_instance->channels.find(new_channel.id);
        if (it != discpp::globals::bot_instance->channels.end()) {
            it->second = new_channel;
        }

        discpp::DispatchEvent(discpp::ChannelPinsUpdateEvent(new_channel));
    }

    void EventDispatcher::GuildCreateEvent(nlohmann::json& result) {
        snowflake guild_id = result["id"].get<snowflake>();
        discpp::Guild guild(result);
        discpp::globals::bot_instance->logger.LogToConsole(LogSeverity::SEV_INFO, LogTextColor::GREEN + "Connected to " + guild.name);
        discpp::globals::bot_instance->guilds.insert(std::pair<snowflake, Guild>(static_cast<snowflake>(guild.id), static_cast<Guild>(guild)));

        discpp::globals::bot_instance->members.insert(guild.members.begin(), guild.members.end());

        for (auto& channel : result["channels"]) {
            discpp::GuildChannel _channel = GuildChannel(channel);
            _channel.guild_id = guild_id;
            discpp::globals::bot_instance->channels.insert(std::pair<snowflake, Channel>(static_cast<snowflake>(_channel.id), static_cast<Channel>(_channel)));
        }

        discpp::DispatchEvent(discpp::GuildCreateEvent(guild));
    }

    void EventDispatcher::GuildUpdateEvent(nlohmann::json& result) {
        discpp::Guild guild(result);
        std::unordered_map<snowflake, Guild>::iterator it = discpp::globals::bot_instance->guilds.find(guild.id);
        if (it != discpp::globals::bot_instance->guilds.end()) {
            it->second = guild;
        }
        discpp::DispatchEvent(discpp::GuildUpdateEvent(guild));
    }

    void EventDispatcher::GuildDeleteEvent(nlohmann::json& result) {
        discpp::Guild guild;
        guild.id = result["id"].get<snowflake>();
        guild.unavailable = true;
        discpp::globals::bot_instance->guilds.erase(guild.id);

        discpp::DispatchEvent(discpp::GuildDeleteEvent(guild));
    }

    void EventDispatcher::GuildBanAddEvent(nlohmann::json& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::User user(result["user"]);

        discpp::DispatchEvent(discpp::GuildBanAddEvent(guild, user));
    }

    void EventDispatcher::GuildBanRemoveEvent(nlohmann::json& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::User user(result["user"]);

        discpp::DispatchEvent(discpp::GuildBanRemoveEvent(guild, user));
    }

    void EventDispatcher::GuildEmojisUpdateEvent(nlohmann::json& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        std::unordered_map<snowflake, Emoji> emojis;
        for (nlohmann::json emoji : result["emojis"]) {
            discpp::Emoji tmp = discpp::Emoji(emoji);
            emojis.insert(std::pair<snowflake, Emoji>(static_cast<snowflake>(tmp.id), static_cast<Emoji>(tmp)));
        }

        guild.emojis = emojis;
        std::unordered_map<snowflake, Guild>::iterator it = discpp::globals::bot_instance->guilds.find(guild.id);
        if (it != discpp::globals::bot_instance->guilds.end()) {
            it->second = guild;
        }

        discpp::DispatchEvent(discpp::GuildEmojisUpdateEvent(guild));
    }

    void EventDispatcher::GuildIntegrationsUpdateEvent(nlohmann::json& result) {
        discpp::DispatchEvent(
            discpp::GuildIntegrationsUpdateEvent(discpp::Guild(result["guild_id"].get<snowflake>())));
    }

    void EventDispatcher::GuildMemberAddEvent(nlohmann::json& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::Member member(result, guild.id);
        discpp::globals::bot_instance->members.insert(std::pair<snowflake, Member>(static_cast<snowflake>(member.id), static_cast<Member>(member)));

        discpp::DispatchEvent(discpp::GuildMemberAddEvent(guild, member));
    }

    void EventDispatcher::GuildMemberRemoveEvent(nlohmann::json& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::Member member(result["user"]["id"].get<snowflake>(), guild);
        discpp::globals::bot_instance->members.erase(member.id);

        discpp::DispatchEvent(discpp::GuildMemberRemoveEvent(guild, member));
    }

    void EventDispatcher::GuildMemberUpdateEvent(nlohmann::json& result) {
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

    void EventDispatcher::GuildMembersChunkEvent(nlohmann::json& result) {
        discpp::DispatchEvent(discpp::GuildMembersChunkEvent());
    }

    void EventDispatcher::GuildRoleCreateEvent(nlohmann::json& result) {
        discpp::Role role(result["role"]);

        discpp::DispatchEvent(discpp::GuildRoleCreateEvent(role));
    }

    void EventDispatcher::GuildRoleUpdateEvent(nlohmann::json& result) {
        discpp::Role role(result["role"]);

        discpp::DispatchEvent(discpp::GuildRoleUpdateEvent(role));
    }

    void EventDispatcher::GuildRoleDeleteEvent(nlohmann::json& result) {
        discpp::Guild guild(result["guild_id"].get<snowflake>());
        discpp::Role role(result["role"]);
        guild.roles.erase(role.id);

        discpp::DispatchEvent(discpp::GuildRoleDeleteEvent(role));
    }

    void EventDispatcher::MessageCreateEvent(nlohmann::json& result) {
        discpp::Message message(result);
        if (discpp::globals::bot_instance->messages.size() >= discpp::globals::bot_instance->message_cache_count) {
            discpp::globals::bot_instance->messages.erase(discpp::globals::bot_instance->messages.begin());
        }
        discpp::globals::bot_instance->messages.insert({ message.id, message });

        discpp::globals::bot_instance->DoFunctionLater(discpp::globals::bot_instance->fire_command_method, discpp::globals::bot_instance, message);
        discpp::DispatchEvent(discpp::MessageCreateEvent(message));
    }

    void EventDispatcher::MessageUpdateEvent(nlohmann::json& result) {
        auto message = discpp::globals::bot_instance->messages.find(result["id"].get<snowflake>());

        if (message != discpp::globals::bot_instance->messages.end()) {
            if (discpp::globals::bot_instance->messages.size() >= discpp::globals::bot_instance->message_cache_count) {
                discpp::globals::bot_instance->messages.erase(discpp::globals::bot_instance->messages.begin());
            }

            discpp::Message old_message(result["id"].get<snowflake>());
            /*std::replace_if(messages.begin(), messages.end(),
                            [message](discpp::Message &msg) { return msg.id == message.id; }, message);*/
            bool is_edited = !result["edited_timestamp"].empty();

            discpp::DispatchEvent(discpp::MessageUpdateEvent(message->second, old_message, is_edited));
        }
    }

    void EventDispatcher::MessageDeleteEvent(nlohmann::json& result) {
        auto message = discpp::globals::bot_instance->messages.find(result["id"].get<snowflake>());

        if (message != discpp::globals::bot_instance->messages.end()) {
            discpp::DispatchEvent(discpp::MessageDeleteEvent(message->second));

            discpp::globals::bot_instance->messages.erase(message);
        }
    }

    void EventDispatcher::MessageDeleteBulkEvent(nlohmann::json& result) {
        std::vector<discpp::Message> msgs;
        for (auto id : result["ids"]) {
            auto message = discpp::globals::bot_instance->messages.find(id.get<snowflake>());

            if (message != discpp::globals::bot_instance->messages.end()) {
                message->second.channel = discpp::Channel(result["channel_id"].get<snowflake>());
                if (result.contains("guild_id")) {
                    message->second.guild = discpp::Guild(result["guild_id"].get<snowflake>());
                }

                msgs.push_back(message->second);
            }
        }

        for (discpp::Message message : msgs) {
            discpp::globals::bot_instance->messages.erase(message.id);
        }

        discpp::DispatchEvent(discpp::MessageBulkDeleteEvent(msgs));
    }

    void EventDispatcher::MessageReactionAddEvent(nlohmann::json& result) {
        auto message = discpp::globals::bot_instance->messages.find(result["message_id"].get<snowflake>());

        if (message != discpp::globals::bot_instance->messages.end()) {
            discpp::Channel channel = discpp::globals::bot_instance->channels.find(result["channel_id"].get<snowflake>())->second;
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

    void EventDispatcher::MessageReactionRemoveEvent(nlohmann::json& result) {
        auto message = discpp::globals::bot_instance->messages.find(result["message_id"].get<snowflake>());

        if (message != discpp::globals::bot_instance->messages.end()) {
            discpp::Channel channel = discpp::globals::bot_instance->channels.find(result["channel_id"].get<snowflake>())->second;
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

    void EventDispatcher::MessageReactionRemoveAllEvent(nlohmann::json& result) {
        auto message = discpp::globals::bot_instance->messages.find(result["message_id"].get<snowflake>());

        if (message != discpp::globals::bot_instance->messages.end()) {
            discpp::Channel channel = discpp::globals::bot_instance->channels.find(result["channel_id"].get<snowflake>())->second;
            message->second.channel = channel;

            discpp::DispatchEvent(discpp::MessageReactionRemoveAllEvent(message->second));
        }
    }

    void EventDispatcher::PresenceUpdateEvent(nlohmann::json& result) {
        discpp::DispatchEvent(discpp::PresenseUpdateEvent(discpp::User(result["user"])));
    }

    void EventDispatcher::TypingStartEvent(nlohmann::json& result) {
        discpp::User user(result["user_id"].get<snowflake>());
        discpp::Channel channel(result["channel_id"].get<snowflake>());
        int timestamp = result["timestamp"].get<int>();

        discpp::DispatchEvent(discpp::TypingStartEvent(user, channel, timestamp));
    }

    void EventDispatcher::UserUpdateEvent(nlohmann::json& result) {
        discpp::User user(result);

        discpp::DispatchEvent(discpp::UserUpdateEvent(user));
    }

    void EventDispatcher::VoiceStateUpdateEvent(nlohmann::json& result) {
        discpp::DispatchEvent(discpp::VoiceStateUpdateEvent(result));
    }

    void EventDispatcher::VoiceServerUpdateEvent(nlohmann::json& result) {
        discpp::DispatchEvent(discpp::VoiceServerUpdateEvent(result));
    }

    void EventDispatcher::WebhooksUpdateEvent(nlohmann::json& result) {
        discpp::GuildChannel channel(result["channel_id"].get<snowflake>());
        channel.guild_id = result["guild_id"].get<snowflake>();

        discpp::DispatchEvent(discpp::WebhooksUpdateEvent(channel));
    }

    void EventDispatcher::BindEvents() {
        internal_event_map["READY"] = [&](nlohmann::json& result) { discpp::EventDispatcher::ReadyEvent(result); };
        internal_event_map["RESUMED"] = [&](nlohmann::json& result) { discpp::EventDispatcher::ChannelCreateEvent(result); };
        internal_event_map["INVALID_SESSION"] = [&](nlohmann::json& result) { discpp::EventDispatcher::InvalidSessionEvent(result); };
        internal_event_map["CHANNEL_CREATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::ChannelCreateEvent(result); };
        internal_event_map["CHANNEL_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::ChannelUpdateEvent(result); };
        internal_event_map["CHANNEL_DELETE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::ChannelDeleteEvent(result); };
        internal_event_map["CHANNEL_PINS_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::ChannelPinsUpdateEvent(result); };
        internal_event_map["GUILD_CREATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildCreateEvent(result); };
        internal_event_map["GUILD_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildUpdateEvent(result); };
        internal_event_map["GUILD_DELETE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildDeleteEvent(result); };
        internal_event_map["GUILD_BAN_ADD"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildBanAddEvent(result); };
        internal_event_map["GUILD_BAN_REMOVE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildBanRemoveEvent(result); };
        internal_event_map["GUILD_EMOJIS_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildEmojisUpdateEvent(result); };
        internal_event_map["GUILD_INTEGRATIONS_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildIntegrationsUpdateEvent(result); };
        internal_event_map["GUILD_MEMBER_ADD"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildMemberAddEvent(result); };
        internal_event_map["GUILD_MEMBER_REMOVE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildMemberRemoveEvent(result); };
        internal_event_map["GUILD_MEMBER_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildMemberUpdateEvent(result); };
        internal_event_map["GUILD_MEMBERS_CHUNK"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildMembersChunkEvent(result); };
        internal_event_map["GUILD_ROLE_CREATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildRoleCreateEvent(result); };
        internal_event_map["GUILD_ROLE_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildRoleUpdateEvent(result); };
        internal_event_map["GUILD_ROLE_DELETE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::GuildRoleDeleteEvent(result); };
        internal_event_map["MESSAGE_CREATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::MessageCreateEvent(result); };
        internal_event_map["MESSAGE_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::MessageUpdateEvent(result); };
        internal_event_map["MESSAGE_DELETE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::MessageDeleteEvent(result); };
        internal_event_map["MESSAGE_DELETE_BULK"] = [&](nlohmann::json& result) { discpp::EventDispatcher::MessageDeleteBulkEvent(result); };
        internal_event_map["MESSAGE_REACTION_ADD"] = [&](nlohmann::json& result) { discpp::EventDispatcher::MessageReactionAddEvent(result); };
        internal_event_map["MESSAGE_REACTION_REMOVE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::MessageReactionRemoveEvent(result); };
        internal_event_map["MESSAGE_REACTION_REMOVE_ALL"] = [&](nlohmann::json& result) { discpp::EventDispatcher::MessageReactionRemoveAllEvent(result); };
        internal_event_map["PRESENCE_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::PresenceUpdateEvent(result); };
        internal_event_map["TYPING_START"] = [&](nlohmann::json& result) { discpp::EventDispatcher::TypingStartEvent(result); };
        internal_event_map["USER_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::UserUpdateEvent(result); };
        internal_event_map["VOICE_STATE_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::VoiceStateUpdateEvent(result); };
        internal_event_map["VOICE_SERVER_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::VoiceServerUpdateEvent(result); };
        internal_event_map["WEBHOOKS_UPDATE"] = [&](nlohmann::json& result) { discpp::EventDispatcher::WebhooksUpdateEvent(result); };
    }

    void EventDispatcher::RunEvent(const std::function<void(nlohmann::json &)>& func, nlohmann::json& json) {
        func(json);
    }

    void EventDispatcher::HandleDiscordEvent(nlohmann::json&  j, std::string event_name) {
        nlohmann::json data = j["d"];
        discpp::globals::bot_instance->last_sequence_number = (j.contains("s") && j["s"].is_number()) ? j["s"].get<int>() : -1;

        if (internal_event_map.find(event_name) != internal_event_map.end()) {
            if (discpp::globals::bot_instance->ready) {
                internal_event_map[event_name](data);
            } else {
                discpp::globals::bot_instance->futures.push_back(std::async(std::launch::async, [&]{ internal_event_map[event_name](data); }));
            }
        }
    }
}