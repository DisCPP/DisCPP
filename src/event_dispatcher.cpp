#include "event_dispatcher.h"
#include "event_handler.h"
#include "events/all_discord_events.h"
#include "client_config.h"

namespace discpp {
    void EventDispatcher::ReadyEvent(Shard& shard, const discpp::JsonObject& result) {
        // Check if we're just resuming, and if we are dont try to create a new thread.
        if (!shard.heartbeat_thread.joinable()) {
            Shard* sh = &shard;
            shard.heartbeat_thread = std::thread([sh] {
                sh->HandleHeartbeat();
            });
        }

        shard.ready = true;
        // @TODO: This for some reason causes an exception.
        shard.session_id = result["session_id"].GetString();

        if (discpp::globals::client_instance->config->type == discpp::TokenType::USER) {
            /*rapidjson::Document user_json;
            user_json.CopyFrom(result["user"], user_json.GetAllocator());*/

            discpp::ClientUser client_user(result["user"]);
            discpp::globals::client_instance->client_user = client_user;

            result.IterateThrough("guilds", [&] (const discpp::JsonObject& guild_json)->bool {
                GuildCreateEvent(shard, guild_json);
                return true;
            });

            result.IterateThrough("private_channel", [&] (const discpp::JsonObject& private_channel_json)->bool {
                discpp::Channel dm_channel(private_channel_json);
                discpp::globals::client_instance->cache.private_channels.insert({ dm_channel.id, dm_channel });
                return true;
            });
        } else {
            if (globals::client_instance->client_user.id == 0) {
                // Get the bot user
                std::unique_ptr<discpp::JsonObject> user_json = SendGetRequest(Endpoint("/users/@me"), DefaultHeaders(), {}, {});
                discpp::globals::client_instance->client_user = discpp::ClientUser(*user_json);
            }
        }

        discpp::DispatchEvent(discpp::ReadyEvent(result));
    }

    void EventDispatcher::ResumedEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::DispatchEvent(discpp::ResumedEvent());
    }

    void EventDispatcher::ReconnectEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::DispatchEvent(discpp::ReconnectEvent());
    }

    void EventDispatcher::InvalidSessionEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::DispatchEvent(discpp::InvalidSessionEvent());
    }

    void EventDispatcher::ChannelCreateEvent(Shard& shard, const discpp::JsonObject& result) {
        if (result.ContainsNotNull("guild_id")) {
            discpp::Channel new_channel(result);
            std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            guild->channels.insert({ new_channel.id, new_channel });
            discpp::DispatchEvent(discpp::ChannelCreateEvent(new_channel));
        } else {
            discpp::Channel new_channel(result);

            globals::client_instance->cache.private_channels.insert({ new_channel.id, new_channel });
            discpp::DispatchEvent(discpp::ChannelCreateEvent(new_channel));
        }
    }

    void EventDispatcher::ChannelUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        if (result.ContainsNotNull("guild_id")) {
            discpp::Channel updated_channel(result);
            std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            auto guild_chan_it = guild->channels.find(updated_channel.id);
            if (guild_chan_it != guild->channels.end()) {
                guild_chan_it->second = updated_channel;
            }

            discpp::DispatchEvent(discpp::ChannelUpdateEvent(updated_channel));
        } else {
            discpp::Channel updated_channel(result);

            auto client_chan_it = discpp::globals::client_instance->cache.private_channels.find(updated_channel.id);
            client_chan_it->second = updated_channel;

            discpp::DispatchEvent(discpp::ChannelUpdateEvent(updated_channel));
        }
    }

    void EventDispatcher::ChannelDeleteEvent(Shard& shard, const discpp::JsonObject& result) {
        if (result.ContainsNotNull("guild_id")) {
            discpp::Channel updated_channel(result);
            std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            discpp::DispatchEvent(discpp::ChannelUpdateEvent(updated_channel));
        } else {
            discpp::Channel updated_channel(result);

            discpp::DispatchEvent(discpp::ChannelUpdateEvent(updated_channel));
        }
    }

    void EventDispatcher::ChannelPinsUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        if (result.ContainsNotNull("guild_id")) {
            discpp::Channel pin_update_channel = discpp::Channel(SnowflakeFromString(result["channel_id"].GetString()));
            discpp::Guild guild(pin_update_channel.guild_id);

            auto it = guild.channels.find(pin_update_channel.id);
            if (it != guild.channels.end()) {
                it->second.last_pin_timestamp = TimeFromDiscord(result["last_pin_timestamp"].GetString());
            }

            discpp::DispatchEvent(discpp::ChannelPinsUpdateEvent(pin_update_channel));
        } else {
            discpp::Channel pin_update_channel = discpp::Channel(SnowflakeFromString(result["channel_id"].GetString()));

            auto it = globals::client_instance->cache.private_channels.find(pin_update_channel.id);
            if (it != globals::client_instance->cache.private_channels.end()) {
                it->second.last_pin_timestamp = TimeFromDiscord(result["last_pin_timestamp"].GetString());
            }

            discpp::DispatchEvent(discpp::ChannelPinsUpdateEvent(pin_update_channel));
        }
    }

    void EventDispatcher::GuildCreateEvent(Shard& shard, const discpp::JsonObject& result) {
        Snowflake guild_id = SnowflakeFromString(result["id"].GetString());

        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(result);
        globals::client_instance->cache.guilds.emplace(guild_id, guild);
        globals::client_instance->cache.members.insert(guild->members.begin(), guild->members.end());

        discpp::DispatchEvent(discpp::GuildCreateEvent(guild));
    }

    void EventDispatcher::GuildUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(result);

        auto it = globals::client_instance->cache.guilds.find(guild->id);
        if (it != globals::client_instance->cache.guilds.end()) {
            it->second = guild;
        }

        discpp::DispatchEvent(discpp::GuildUpdateEvent(guild));
    }

    void EventDispatcher::GuildDeleteEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(SnowflakeFromString(result["id"].GetString()));

        globals::client_instance->cache.guilds.erase(guild->id);
        discpp::DispatchEvent(discpp::GuildDeleteEvent(guild));
    }

    void EventDispatcher::GuildBanAddEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::Guild guild(SnowflakeFromString(result["guild_id"].GetString()));
        discpp::User user(result["user"]);

        discpp::DispatchEvent(discpp::GuildBanAddEvent(guild, user));
    }

    void EventDispatcher::GuildBanRemoveEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::Guild guild(SnowflakeFromString(result["guild_id"].GetString()));
        discpp::User user(result["user"]);

        discpp::DispatchEvent(discpp::GuildBanRemoveEvent(guild, user));
    }

    void EventDispatcher::GuildEmojisUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

        std::unordered_map<Snowflake, Emoji> emojis;
        result.IterateThrough("emojis", [&] (const discpp::JsonObject& emoji_json)->bool {
            discpp::Emoji tmp = discpp::Emoji(emoji_json);
            emojis.insert({ tmp.id, tmp });
            return true;
        });

        guild->emojis = emojis;
        auto it = globals::client_instance->cache.guilds.find(guild->id);
        if (it != globals::client_instance->cache.guilds.end()) {
            it->second = guild;
        }

        discpp::DispatchEvent(discpp::GuildEmojisUpdateEvent(guild));
    }

    void EventDispatcher::GuildIntegrationsUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::DispatchEvent(discpp::GuildIntegrationsUpdateEvent(discpp::Guild(SnowflakeFromString(result["guild_id"].GetString()))));
    }

    void EventDispatcher::GuildMemberAddEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));
        std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(result, *guild);
        globals::client_instance->cache.members.insert({ member->user.id, member });

        discpp::DispatchEvent(discpp::GuildMemberAddEvent(guild, member));
    }

    void EventDispatcher::GuildMemberRemoveEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));
        std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(SnowflakeFromString(result["user"]["id"].GetString()), *guild);
        globals::client_instance->cache.members.erase(member->user.id);

        discpp::DispatchEvent(discpp::GuildMemberRemoveEvent(guild, member));
    }

    void EventDispatcher::GuildMemberUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(SnowflakeFromString(result["guild_id"].GetString()));
        auto it = guild->members.find(static_cast<Snowflake>(SnowflakeFromString(result["user"]["id"].GetString())));

        std::shared_ptr<discpp::Member> member;
        if (it != guild->members.end()) {
            member = it->second;
        } else {
            member = std::make_shared<discpp::Member>(SnowflakeFromString(result["user"]["id"].GetString()), *guild);
            guild->members.insert({ member->user.id, member });
        }

        member->roles.clear();
        result.IterateThrough("roles", [&] (const discpp::JsonObject& role_json)->bool {
            member->roles.emplace_back(SnowflakeFromString(role_json.GetString()));
            return true;
        });

        if (result.ContainsNotNull("nick")) {
            member->nick = result["nick"].GetString();
        }

        discpp::DispatchEvent(discpp::GuildMemberUpdateEvent(guild, member));
    }

    void EventDispatcher::GuildMembersChunkEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));
        std::unordered_map<discpp::Snowflake, discpp::Member> members;
        result.IterateThrough("members", [&] (const discpp::JsonObject& member_json)->bool {
            discpp::Member tmp(member_json, *guild);
            members.emplace(tmp.user.id, tmp);
            return true;
        });

        int chunk_index = result["chunk_index"].GetInt();
        int chunk_count = result["chunk_count"].GetInt();

        std::vector<discpp::Presence> presences;
        if (result.ContainsNotNull("presences")) {
            result.IterateThrough("presences", [&] (const discpp::JsonObject& presence_json)->bool {
                discpp::Presence tmp(presence_json);
                presences.push_back(tmp);
                return true;
            });
        }
        std::string nonce = result.Get<std::string>("nonce");

        discpp::DispatchEvent(discpp::GuildMembersChunkEvent(guild, members, chunk_index, chunk_count, presences, nonce));
    }

    void EventDispatcher::GuildRoleCreateEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(Snowflake(result["guild_id"].GetString()));
        auto role = std::make_shared<discpp::Role>(result["role"]);

        auto it = guild->GetRole(role->id);
        if (it != nullptr) {
            it = role;
        }

        discpp::DispatchEvent(discpp::GuildRoleCreateEvent(guild, role));
    }

    void EventDispatcher::GuildRoleUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(Snowflake(result["guild_id"].GetString()));
        auto role = std::make_shared<discpp::Role>(result["role"]);

        auto it = guild->GetRole(role->id);
        if (it != nullptr) {
            it = role;
        }

        discpp::DispatchEvent(discpp::GuildRoleUpdateEvent(guild, role));
    }

    void EventDispatcher::GuildRoleDeleteEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::Guild guild(SnowflakeFromString((result["guild_id"].GetString())));
        discpp::Role role(SnowflakeFromString(result["role_id"].GetString()), guild);

        guild.roles.erase(role.id);

        discpp::DispatchEvent(discpp::GuildRoleDeleteEvent(role));
    }

    void EventDispatcher::MessageCreateEvent(Shard& shard, const discpp::JsonObject& result) {
        std::shared_ptr<discpp::Message> message = std::make_shared<discpp::Message>(result);
        if (!globals::client_instance->cache.messages.empty()) {
            if (globals::client_instance->cache.messages.size() >= discpp::globals::client_instance->message_cache_count) {
                globals::client_instance->cache.messages.erase(globals::client_instance->cache.messages.begin());
            }

            globals::client_instance->cache.messages.insert({message->id, message});
        }

        if (discpp::globals::client_instance->config->type == discpp::TokenType::BOT) {
            discpp::globals::client_instance->DoFunctionLater(discpp::globals::client_instance->fire_command_method, discpp::globals::client_instance, *message);
        }

        discpp::DispatchEvent(discpp::MessageCreateEvent(*message));
    }

    void EventDispatcher::MessageUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        auto message_it = globals::client_instance->cache.messages.find(SnowflakeFromString(result["id"].GetString()));

        discpp::Message old_message;
        discpp::Message edited_message = discpp::Message(result);
        bool is_edited = result.ContainsNotNull("edited_timestamp");
        if (message_it != globals::client_instance->cache.messages.end()) {
            if (globals::client_instance->cache.messages.size() >= discpp::globals::client_instance->message_cache_count) {
                globals::client_instance->cache.messages.erase(globals::client_instance->cache.messages.begin());
            }

            old_message = *message_it->second;
        }

        discpp::DispatchEvent(discpp::MessageUpdateEvent(edited_message, old_message, is_edited));
    }

    void EventDispatcher::MessageDeleteEvent(Shard& shard, const discpp::JsonObject& result) {
        auto message = globals::client_instance->cache.messages.find(SnowflakeFromString(result["id"].GetString()));

        if (message != globals::client_instance->cache.messages.end()) {
            discpp::DispatchEvent(discpp::MessageDeleteEvent(*message->second));

            globals::client_instance->cache.messages.erase(message);
        }
    }

    void EventDispatcher::MessageDeleteBulkEvent(Shard& shard, const discpp::JsonObject& result) {
        std::vector<discpp::Message> msgs;
        result.IterateThrough("ids", [&] (const discpp::JsonObject& id_json)->bool {
            auto message = globals::client_instance->cache.messages.find(Snowflake(id_json.GetString()));
            if (message != globals::client_instance->cache.messages.end()) {
                // Make sure the messages values are up to date.
                if (result.ContainsNotNull("guild_id")) {
                    std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(Snowflake(result["guild_id"].GetString()));;
                    message->second->guild = guild;

                    auto channel_it = guild->channels.find(Snowflake(result["channel_id"].GetString()));
                    if (channel_it != guild->channels.end()) {
                        message->second->channel = channel_it->second;
                    }
                } else {
                    auto channel_it = globals::client_instance->cache.private_channels.find(Snowflake(result["channel_id"].GetString()));

                    if (channel_it != globals::client_instance->cache.private_channels.end()) {
                        message->second->channel = channel_it->second;
                    }
                }

                msgs.push_back(*message->second);
            }

            return true;
        });

        for (discpp::Message message : msgs) {
            globals::client_instance->cache.messages.erase(message.id);
        }

        discpp::DispatchEvent(discpp::MessageBulkDeleteEvent(msgs));
    }

    void EventDispatcher::MessageReactionAddEvent(Shard& shard, const discpp::JsonObject& result) {
        auto message = globals::client_instance->cache.messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != globals::client_instance->cache.messages.end()) {
            // Make sure the messages values are up to date.
            discpp::Channel channel;
            if (result.ContainsNotNull("guild_id")) {
                std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->channel.guild_id = guild->id;
                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = globals::client_instance->cache.private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != globals::client_instance->cache.private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            discpp::Emoji emoji(result["emoji"]);

            discpp::User user(SnowflakeFromString(result["user_id"].GetString()));

            auto reaction = std::find_if(message->second->reactions.begin(), message->second->reactions.end(),
            [&emoji](discpp::Reaction react) {
                return react.emoji == emoji;
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
        } else {
            discpp::Channel channel = globals::client_instance->cache.GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            auto tmp = channel.RequestMessage(SnowflakeFromString(result["message_id"].GetString()));
            if (!tmp.has_value()) {
                throw std::runtime_error("couldn't find message");
            }
            discpp::Message message = tmp.value();

            if (result.ContainsNotNull("guild_id")) {
                channel.guild_id = Snowflake(result["guild_id"].GetString());
                message.guild = globals::client_instance->cache.GetGuild(Snowflake(result["guild_id"].GetString()));
            }

            discpp::Emoji emoji(result["emoji"]);

            discpp::User user(SnowflakeFromString(result["user_id"].GetString()));
            discpp::DispatchEvent(discpp::MessageReactionAddEvent(message, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveEvent(Shard& shard, const discpp::JsonObject& result) {
        auto message = globals::client_instance->cache.messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != globals::client_instance->cache.messages.end()) {
            // Make sure the messages values are up to date.
            discpp::Channel channel;
            if (result.ContainsNotNull("guild_id")) {
                std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = globals::client_instance->cache.private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != globals::client_instance->cache.private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            discpp::Emoji emoji(result["emoji"]);

            discpp::User user(SnowflakeFromString(result["user_id"].GetString()));

            auto reaction = std::find_if(message->second->reactions.begin(), message->second->reactions.end(),
                 [&emoji](discpp::Reaction react) {
                     return react.emoji == emoji;
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
        } else {
            discpp::Channel channel = globals::client_instance->cache.GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            auto tmp = channel.RequestMessage(SnowflakeFromString(result["message_id"].GetString()));
            if (!tmp.has_value()) {
                throw std::runtime_error("error finding message");
            }
            discpp::Message message = tmp.value();

            if (result.ContainsNotNull("guild_id")) {
                channel.guild_id = Snowflake(result["guild_id"].GetString());
                message.guild = globals::client_instance->cache.GetGuild(Snowflake(result["guild_id"].GetString()));
            }

            discpp::Emoji emoji(result["emoji"]);

            discpp::User user(SnowflakeFromString(result["user_id"].GetString()));
            discpp::DispatchEvent(discpp::MessageReactionRemoveEvent(message, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveAllEvent(Shard& shard, const discpp::JsonObject& result) {
        auto message = globals::client_instance->cache.messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != globals::client_instance->cache.messages.end()) {
            discpp::Channel channel;
            if (result.ContainsNotNull("guild_id")) {
                std::shared_ptr<discpp::Guild> guild = globals::client_instance->cache.GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = globals::client_instance->cache.private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != globals::client_instance->cache.private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            discpp::DispatchEvent(discpp::MessageReactionRemoveAllEvent(*message->second));
        } else {
            discpp::Channel channel = globals::client_instance->cache.GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            auto tmp = channel.RequestMessage(SnowflakeFromString(result["message_id"].GetString()));
            if (!tmp.has_value()) {
                throw std::runtime_error("error can't find message");
            }
            discpp::Message message = tmp.value();

            if (result.ContainsNotNull("guild_id")) {
                channel.guild_id = Snowflake(result["guild_id"].GetString());
                message.guild = globals::client_instance->cache.GetGuild(Snowflake(result["guild_id"].GetString()));
            }

            discpp::DispatchEvent(discpp::MessageReactionRemoveAllEvent(message));
        }
    }

    void EventDispatcher::PresenceUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::DispatchEvent(discpp::PresenseUpdateEvent(discpp::User(result["user"])));
    }

    void EventDispatcher::TypingStartEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::User user(SnowflakeFromString(result["user_id"].GetString()));

        discpp::Channel channel;
        if (result.ContainsNotNull("guild_id")) {
            discpp::Guild guild(SnowflakeFromString(result["guild_id"].GetString()));
            channel = guild.GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
        } else {
            channel = discpp::Channel(SnowflakeFromString(result["channel_id"].GetString()));
        }

        int timestamp = result["timestamp"].GetInt();

        discpp::DispatchEvent(discpp::TypingStartEvent(user, channel, timestamp));
    }

    void EventDispatcher::UserUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::User user(result);

        discpp::DispatchEvent(discpp::UserUpdateEvent(user));
    }

    void EventDispatcher::VoiceStateUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::DispatchEvent(discpp::VoiceStateUpdateEvent(result));
    }

    void EventDispatcher::VoiceServerUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::DispatchEvent(discpp::VoiceServerUpdateEvent(result));
    }

    void EventDispatcher::WebhooksUpdateEvent(Shard& shard, const discpp::JsonObject& result) {
        discpp::Channel channel(SnowflakeFromString(result["channel_id"].GetString()));
        channel.guild_id = SnowflakeFromString(result["guild_id"].GetString());

        discpp::DispatchEvent(discpp::WebhooksUpdateEvent(channel));
    }

    void EventDispatcher::BindEvents() {
        internal_event_map["READY"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::ReadyEvent(shard, result); };
        internal_event_map["RESUMED"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::ResumedEvent(shard, result); };
        internal_event_map["INVALID_SESSION"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::InvalidSessionEvent(shard, result); };
        internal_event_map["CHANNEL_CREATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::ChannelCreateEvent(shard, result); };
        internal_event_map["CHANNEL_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::ChannelUpdateEvent(shard, result); };
        internal_event_map["CHANNEL_DELETE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::ChannelDeleteEvent(shard, result); };
        internal_event_map["CHANNEL_PINS_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::ChannelPinsUpdateEvent(shard, result); };
        internal_event_map["GUILD_CREATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildCreateEvent(shard, result); };
        internal_event_map["GUILD_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildUpdateEvent(shard, result); };
        internal_event_map["GUILD_DELETE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildDeleteEvent(shard, result); };
        internal_event_map["GUILD_BAN_ADD"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildBanAddEvent(shard, result); };
        internal_event_map["GUILD_BAN_REMOVE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildBanRemoveEvent(shard, result); };
        internal_event_map["GUILD_EMOJIS_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildEmojisUpdateEvent(shard, result); };
        internal_event_map["GUILD_INTEGRATIONS_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildIntegrationsUpdateEvent(shard, result); };
        internal_event_map["GUILD_MEMBER_ADD"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildMemberAddEvent(shard, result); };
        internal_event_map["GUILD_MEMBER_REMOVE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildMemberRemoveEvent(shard, result); };
        internal_event_map["GUILD_MEMBER_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildMemberUpdateEvent(shard, result); };
        internal_event_map["GUILD_MEMBERS_CHUNK"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildMembersChunkEvent(shard, result); };
        internal_event_map["GUILD_ROLE_CREATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildRoleCreateEvent(shard, result); };
        internal_event_map["GUILD_ROLE_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildRoleUpdateEvent(shard, result); };
        internal_event_map["GUILD_ROLE_DELETE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::GuildRoleDeleteEvent(shard, result); };
        internal_event_map["MESSAGE_CREATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::MessageCreateEvent(shard, result); };
        internal_event_map["MESSAGE_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::MessageUpdateEvent(shard, result); };
        internal_event_map["MESSAGE_DELETE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::MessageDeleteEvent(shard, result); };
        internal_event_map["MESSAGE_DELETE_BULK"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::MessageDeleteBulkEvent(shard, result); };
        internal_event_map["MESSAGE_REACTION_ADD"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::MessageReactionAddEvent(shard, result); };
        internal_event_map["MESSAGE_REACTION_REMOVE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::MessageReactionRemoveEvent(shard, result); };
        internal_event_map["MESSAGE_REACTION_REMOVE_ALL"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::MessageReactionRemoveAllEvent(shard, result); };
        internal_event_map["PRESENCE_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::PresenceUpdateEvent(shard, result); };
        internal_event_map["TYPING_START"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::TypingStartEvent(shard, result); };
        internal_event_map["USER_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::UserUpdateEvent(shard, result); };
        internal_event_map["VOICE_STATE_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::VoiceStateUpdateEvent(shard, result); };
        internal_event_map["VOICE_SERVER_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::VoiceServerUpdateEvent(shard, result); };
        internal_event_map["WEBHOOKS_UPDATE"] = [&](Shard& shard, const discpp::JsonObject& result) { discpp::EventDispatcher::WebhooksUpdateEvent(shard, result); };
    }

    void EventDispatcher::RegisterGatewayCustomEvent(const char* event_name, const std::function<void(Shard& shard, const discpp::JsonObject&)>& func) {
        internal_event_map[event_name] = func;
    }

    void EventDispatcher::HandleDiscordEvent(Shard& shard, const discpp::JsonObject& j, const std::string& event_name) {
#ifdef RAPIDJSON_BACKEND
        discpp::JsonObject data = j["d"];

        if (j.ContainsNotNull("s")) {
            shard.last_sequence_number = j["s"].GetInt();
        } else {
            shard.last_sequence_number = 0;
        }

        Shard* sh = &shard;
        if (internal_event_map.find(event_name) != internal_event_map.end()) {
            globals::client_instance->DoFunctionLater([sh, data, event_name] {
                internal_event_map[event_name](*sh, data);
            });
        }
#elif SIMDJSON_BACKEND

#endif
    }
}