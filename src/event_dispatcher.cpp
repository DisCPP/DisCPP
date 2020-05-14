#include "event_dispatcher.h"
#include "event_handler.h"
#include "events/all_discord_events.h"
#include "client_config.h"

namespace discpp {
    void EventDispatcher::ReadyEvent(rapidjson::Document& result) {
        // Check if we're just resuming, and if we are dont try to create a new thread.
        if (!discpp::globals::client_instance->heartbeat_thread.joinable()) {
            discpp::globals::client_instance->heartbeat_thread = std::thread{ &Client::HandleHeartbeat, discpp::globals::client_instance };
        }

        discpp::globals::client_instance->ready = true;
        // @TODO: This for some reason causes an exception.
        discpp::globals::client_instance->session_id = result["session_id"].GetString();

        if (discpp::globals::client_instance->config->type == discpp::TokenType::USER) {
            rapidjson::Document user_json;
            user_json.CopyFrom(result["user"], user_json.GetAllocator());

            discpp::ClientUser client_user(user_json);
            discpp::globals::client_instance->client_user = client_user;

            for (const auto& guild : result["guilds"].GetArray()) {
                rapidjson::Document guild_json(rapidjson::kObjectType);
                guild_json.CopyFrom(guild, guild_json.GetAllocator());

                GuildCreateEvent(guild_json);
            }

            for (const auto& private_channel : result["private_channels"].GetArray()) {
                rapidjson::Document private_channel_json(rapidjson::kObjectType);
                private_channel_json.CopyFrom(private_channel, private_channel_json.GetAllocator());

                discpp::DMChannel dm_channel(private_channel_json);

                discpp::globals::client_instance->private_channels.insert({ dm_channel.id, dm_channel });
            }
        } else {
            // Get the bot user
            rapidjson::Document user_json = SendGetRequest(Endpoint("/users/@me"), DefaultHeaders(), {}, {});

            discpp::globals::client_instance->client_user = discpp::ClientUser(user_json);
        }

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
        if (ContainsNotNull(result, "guild_id")) {
            discpp::GuildChannel new_channel = result;
            std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            guild->channels.insert({ guild->id, new_channel });
            discpp::DispatchEvent(discpp::GuildChannelCreateEvent(new_channel));
        } else {
            discpp::DMChannel new_channel = discpp::DMChannel(result);

            globals::client_instance->private_channels.insert({ new_channel.id, new_channel });
            discpp::DispatchEvent(discpp::DMChannelCreateEvent(new_channel));
        }
    }

    void EventDispatcher::ChannelUpdateEvent(rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::GuildChannel updated_channel = result;
            std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            auto guild_chan_it = guild->channels.find(updated_channel.id);
            guild_chan_it->second = updated_channel;

            discpp::DispatchEvent(discpp::GuildChannelUpdateEvent(updated_channel));
        } else {
            discpp::DMChannel updated_channel = discpp::DMChannel(result);

            auto client_chan_it = discpp::globals::client_instance->private_channels.find(updated_channel.id);
            client_chan_it->second = updated_channel;

            discpp::DispatchEvent(discpp::DMChannelUpdateEvent(updated_channel));
        }
    }

    void EventDispatcher::ChannelDeleteEvent(rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::GuildChannel updated_channel = result;
            std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            auto guild_chan_it = guild->channels.erase(updated_channel.id);

            discpp::DispatchEvent(discpp::GuildChannelUpdateEvent(updated_channel));
        } else {
            discpp::DMChannel updated_channel = discpp::DMChannel(result);

            auto client_chan_it = discpp::globals::client_instance->private_channels.erase(updated_channel.id);

            discpp::DispatchEvent(discpp::DMChannelUpdateEvent(updated_channel));
        }
    }

    void EventDispatcher::ChannelPinsUpdateEvent(rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::GuildChannel pin_update_channel = discpp::GuildChannel(SnowflakeFromString(result["channel_id"].GetString()), SnowflakeFromString(result["guild_id"].GetString()));
            discpp::Guild guild(pin_update_channel.guild_id);

            auto it = guild.channels.find(pin_update_channel.id);
            if (it != guild.channels.end()) {
                it->second.last_pin_timestamp = result["last_pin_timestamp"].GetString();
            }

            discpp::DispatchEvent(discpp::ChannelPinsUpdateEvent(pin_update_channel));
        } else {
            discpp::DMChannel pin_update_channel = discpp::DMChannel(SnowflakeFromString(result["channel_id"].GetString()));

            auto it = globals::client_instance->private_channels.find(pin_update_channel.id);
            if (it != globals::client_instance->private_channels.end()) {
                it->second.last_pin_timestamp = result["last_pin_timestamp"].GetString();
            }

            discpp::DispatchEvent(discpp::ChannelPinsUpdateEvent(pin_update_channel));
        }
    }

    void EventDispatcher::GuildCreateEvent(rapidjson::Document& result) {
        snowflake guild_id = SnowflakeFromString(result["id"].GetString());

        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(result);
        discpp::globals::client_instance->guilds.insert({ guild->id, guild });
        discpp::globals::client_instance->members.insert(guild->members.begin(), guild->members.end());

        discpp::DispatchEvent(discpp::GuildCreateEvent(guild));
    }

    void EventDispatcher::GuildUpdateEvent(rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(result);

        auto it = discpp::globals::client_instance->guilds.find(guild->id);
        if (it != discpp::globals::client_instance->guilds.end()) {
            it->second = guild;
        }

        discpp::DispatchEvent(discpp::GuildUpdateEvent(guild));
    }

    void EventDispatcher::GuildDeleteEvent(rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(SnowflakeFromString(result["id"].GetString()));

        discpp::globals::client_instance->guilds.erase(guild->id);
        discpp::DispatchEvent(discpp::GuildDeleteEvent(guild));
    }

    void EventDispatcher::GuildBanAddEvent(rapidjson::Document& result) {
        discpp::Guild guild(SnowflakeFromString(result["guild_id"].GetString()));
        rapidjson::Document user_json;
        user_json.CopyFrom(result["user"], user_json.GetAllocator());
        discpp::User user(user_json);

        discpp::DispatchEvent(discpp::GuildBanAddEvent(guild, user));
    }

    void EventDispatcher::GuildBanRemoveEvent(rapidjson::Document& result) {
        discpp::Guild guild(SnowflakeFromString(result["guild_id"].GetString()));
        rapidjson::Document user_json;
        user_json.CopyFrom(result["user"], user_json.GetAllocator());
        discpp::User user(user_json);

        discpp::DispatchEvent(discpp::GuildBanRemoveEvent(guild, user));
    }

    void EventDispatcher::GuildEmojisUpdateEvent(rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

        std::unordered_map<snowflake, Emoji> emojis;
        for (auto& emoji : result["emojis"].GetArray()) {
            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(emoji, emoji_json.GetAllocator());

            discpp::Emoji tmp = discpp::Emoji(emoji_json);
            emojis.insert({ tmp.id, tmp });
        }

        guild->emojis = emojis;
        auto it = discpp::globals::client_instance->guilds.find(guild->id);
        if (it != discpp::globals::client_instance->guilds.end()) {
            it->second = guild;
        }

        discpp::DispatchEvent(discpp::GuildEmojisUpdateEvent(guild));
    }

    void EventDispatcher::GuildIntegrationsUpdateEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::GuildIntegrationsUpdateEvent(discpp::Guild(SnowflakeFromString(result["guild_id"].GetString()))));
    }

    void EventDispatcher::GuildMemberAddEvent(rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));
        std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(result, *guild);
        discpp::globals::client_instance->members.insert({ member->id, member });

        discpp::DispatchEvent(discpp::GuildMemberAddEvent(guild, member));
    }

    void EventDispatcher::GuildMemberRemoveEvent(rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));
        std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(SnowflakeFromString(result["user"]["id"].GetString()), *guild);
        discpp::globals::client_instance->members.erase(member->id);

        discpp::DispatchEvent(discpp::GuildMemberRemoveEvent(guild, member));
    }

    void EventDispatcher::GuildMemberUpdateEvent(rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(SnowflakeFromString(result["guild_id"].GetString()));
        auto it = guild->members.find(static_cast<snowflake>(SnowflakeFromString(result["user"]["id"].GetString())));

        std::shared_ptr<discpp::Member> member;
        if (it != guild->members.end()) {
            member = it->second;
        } else {
            member = std::make_shared<discpp::Member>(SnowflakeFromString(result["user"]["id"].GetString()), *guild);
            guild->members.insert({ member->id, member });
        }

        member->roles.clear();
        for (auto& role : result["roles"].GetArray()) {
            rapidjson::Document role_json;
            role_json.CopyFrom(role, role_json.GetAllocator());

            std::shared_ptr<discpp::Role> tmp = std::make_shared<discpp::Role>(SnowflakeFromString(role_json.GetString()), *guild);
            member->roles.insert({ tmp->id, tmp });
        }
        rapidjson::Value::ConstMemberIterator itr = result.FindMember("nick");
        if (itr != result.MemberEnd() && !result["nick"].IsNull()) {
            member->nick = result["nick"].GetString();
        }

        discpp::DispatchEvent(discpp::GuildMemberUpdateEvent(guild, member));
    }

    void EventDispatcher::GuildMembersChunkEvent(rapidjson::Document& result) {
        discpp::DispatchEvent(discpp::GuildMembersChunkEvent());
    }

    void EventDispatcher::GuildRoleCreateEvent(rapidjson::Document& result) {
        rapidjson::Document role_json = GetDocumentInsideJson(result, "role");
        discpp::Role role(role_json);

        discpp::DispatchEvent(discpp::GuildRoleCreateEvent(role));
    }

    void EventDispatcher::GuildRoleUpdateEvent(rapidjson::Document& result) {
        rapidjson::Document role_json = GetDocumentInsideJson(result, "role");
        discpp::Role role(role_json);

        discpp::DispatchEvent(discpp::GuildRoleUpdateEvent(role));
    }

    void EventDispatcher::GuildRoleDeleteEvent(rapidjson::Document& result) {
        discpp::Guild guild(SnowflakeFromString((result["guild_id"].GetString())));

        rapidjson::Document role_json = GetDocumentInsideJson(result, "role");
        discpp::Role role(role_json);

        guild.roles.erase(role.id);

        discpp::DispatchEvent(discpp::GuildRoleDeleteEvent(role));
    }

    void EventDispatcher::MessageCreateEvent(rapidjson::Document& result) {
        std::shared_ptr<discpp::Message> message = std::make_shared<discpp::Message>(result);
        if (discpp::globals::client_instance->messages.size() >= discpp::globals::client_instance->message_cache_count) {
            discpp::globals::client_instance->messages.erase(discpp::globals::client_instance->messages.begin());
        }
        discpp::globals::client_instance->messages.insert({ message->id, message });

        if (discpp::globals::client_instance->config->type == discpp::TokenType::BOT) {
            discpp::globals::client_instance->DoFunctionLater(discpp::globals::client_instance->fire_command_method, discpp::globals::client_instance, *message);
        }

        discpp::DispatchEvent(discpp::MessageCreateEvent(*message));
    }

    void EventDispatcher::MessageUpdateEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(SnowflakeFromString(result["id"].GetString()));

        if (message != discpp::globals::client_instance->messages.end()) {
            if (discpp::globals::client_instance->messages.size() >= discpp::globals::client_instance->message_cache_count) {
                discpp::globals::client_instance->messages.erase(discpp::globals::client_instance->messages.begin());
            }

            discpp::Message old_message(SnowflakeFromString(result["id"].GetString()));
            bool is_edited = ContainsNotNull(result, "edited_timestamp");

            discpp::DispatchEvent(discpp::MessageUpdateEvent(*message->second, old_message, is_edited));
        }
    }

    void EventDispatcher::MessageDeleteEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(SnowflakeFromString(result["id"].GetString()));

        if (message != discpp::globals::client_instance->messages.end()) {
            discpp::DispatchEvent(discpp::MessageDeleteEvent(*message->second));

            discpp::globals::client_instance->messages.erase(message);
        }
    }

    void EventDispatcher::MessageDeleteBulkEvent(rapidjson::Document& result) {
        std::vector<discpp::Message> msgs;
        for (auto& id : result["ids"].GetArray()) {
            rapidjson::Document id_json;
            id_json.CopyFrom(id, id_json.GetAllocator());
            auto message = discpp::globals::client_instance->messages.find(SnowflakeFromString(id_json.GetString()));

            if (message != discpp::globals::client_instance->messages.end()) {
                // Make sure the messages values are up to date.
                if (ContainsNotNull(result, "guild_id")) {
                    std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));;
                    message->second->guild = guild;

                    auto channel_it = guild->channels.find(SnowflakeFromString(result["channel_id"].GetString()));
                    if (channel_it != guild->channels.end()) {
                        message->second->channel = channel_it->second;
                    }
                } else {
                    auto channel_it = globals::client_instance->private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                    if (channel_it != globals::client_instance->private_channels.end()) {
                        message->second->channel = channel_it->second;
                    }
                }

                msgs.push_back(*message->second);
            }
        }

        for (discpp::Message message : msgs) {
            discpp::globals::client_instance->messages.erase(message.id);
        }

        discpp::DispatchEvent(discpp::MessageBulkDeleteEvent(msgs));
    }

    void EventDispatcher::MessageReactionAddEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != discpp::globals::client_instance->messages.end()) {
            // Make sure the messages values are up to date.
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = globals::client_instance->private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != globals::client_instance->private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(emoji_json);

            discpp::User user(SnowflakeFromString(result["user_id"].GetString()));

            auto reaction = std::find_if(message->second->reactions.begin(), message->second->reactions.end(),
            [emoji](discpp::Reaction react) {
                 auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();

                 // If the other's name is empty but mine is not then it must have unicode.
                 if (react.emoji.name.empty() && !emoji.name.empty() && emoji.unicode.empty()) {
                     return wstr_converter.to_bytes(react.emoji.unicode) == emoji.name;
                 } else if (!react.emoji.name.empty() && emoji.name.empty() && !emoji.unicode.empty()) {
                     return wstr_converter.to_bytes(emoji.unicode) == emoji.name;
                 } else if (react.emoji.id != 0 && emoji.id != 0 && !react.emoji.name.empty() && !emoji.name.empty()) {
                     return react.emoji.id == emoji.id && react.emoji.name == emoji.name;
                 }

                 return false;
            });

            if (reaction != message->second->reactions.end()) {
                reaction->count++;

                if (user.IsBot()) {
                    reaction->from_bot = true;
                }
            } else {
                discpp::Reaction r = discpp::Reaction(1, user.IsBot(), emoji);
                message->second->reactions.push_back(r);
            }

            discpp::DispatchEvent(discpp::MessageReactionAddEvent(*message->second, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != discpp::globals::client_instance->messages.end()) {
            // Make sure the messages values are up to date.
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = globals::client_instance->private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != globals::client_instance->private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(emoji_json);

            discpp::User user(SnowflakeFromString(result["user_id"].GetString()));

            auto reaction = std::find_if(message->second->reactions.begin(), message->second->reactions.end(),
                [emoji](discpp::Reaction react) {
                    auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();

                    // If the other's name is empty but mine is not then it must have unicode.
                    if (react.emoji.name.empty() && !emoji.name.empty() && emoji.unicode.empty()) {
                        return wstr_converter.to_bytes(react.emoji.unicode) == emoji.name;
                    } else if (!react.emoji.name.empty() && emoji.name.empty() && !emoji.unicode.empty()) {
                        return wstr_converter.to_bytes(emoji.unicode) == emoji.name;
                    } else if (react.emoji.id != 0 && emoji.id != 0 && !react.emoji.name.empty() && !emoji.name.empty()) {
                        return react.emoji.id == emoji.id && react.emoji.name == emoji.name;
                    }

                    return false;
                });

            if (reaction != message->second->reactions.end()) {
                if (reaction->count == 1) {
                    message->second->reactions.erase(reaction);
                } else {
                    reaction->count--;

                    // @TODO: Add a way to change reaction::from_bot
                }
            }

            discpp::DispatchEvent(discpp::MessageReactionRemoveEvent(*message->second, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveAllEvent(rapidjson::Document& result) {
        auto message = discpp::globals::client_instance->messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != discpp::globals::client_instance->messages.end()) {
            //if (auto channel_it = globals::client_instance->channels.find(SnowflakeFromString(result["channel_id"].GetString())); channel_it != globals::client_instance->channels.end()) {
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = globals::client_instance->private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != globals::client_instance->private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            discpp::DispatchEvent(discpp::MessageReactionRemoveAllEvent(*message->second));
        }
    }

    void EventDispatcher::PresenceUpdateEvent(rapidjson::Document& result) {
        rapidjson::Document user_json;
        user_json.CopyFrom(result["user"], user_json.GetAllocator());
        discpp::DispatchEvent(discpp::PresenseUpdateEvent(discpp::User(user_json)));
    }

    void EventDispatcher::TypingStartEvent(rapidjson::Document& result) {
        discpp::User user(SnowflakeFromString(result["user_id"].GetString()));

        discpp::Channel channel;
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Guild guild(SnowflakeFromString(result["guild_id"].GetString()));
            channel = guild.GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
        } else {
            channel = discpp::DMChannel(SnowflakeFromString(result["channel_id"].GetString()));
        }

        int timestamp = result["timestamp"].GetInt();

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
        discpp::GuildChannel channel(SnowflakeFromString(result["channel_id"].GetString()), static_cast<snowflake>(result["guild_id"].GetInt64()));
        channel.guild_id = SnowflakeFromString(result["guild_id"].GetString());

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
        std::shared_ptr<rapidjson::Document> data_ptr;

        rapidjson::Document data(rapidjson::kObjectType);
        data.CopyFrom(j["d"], data.GetAllocator());

        // This shows an error for intellisense but compiles fine.
#ifndef __INTELLISENSE__
        data_ptr = std::make_shared<rapidjson::Document>(std::move(data));
#endif

        rapidjson::Value::ConstMemberIterator itr = j.FindMember("s");
        if (itr != j.MemberEnd() && j["s"].IsNumber()) {
            discpp::globals::client_instance->last_sequence_number = j["s"].GetInt();
        } else {
            discpp::globals::client_instance->last_sequence_number = -1;
        }

        if (internal_event_map.find(event_name) != internal_event_map.end()) {
            globals::client_instance->futures.push_back(std::async(std::launch::async, [data_ptr, event_name] {
                internal_event_map[event_name](*data_ptr);
            }));
        }
    }
}