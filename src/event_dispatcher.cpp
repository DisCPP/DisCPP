#include "event_dispatcher.h"
#include "event_handler.h"
#include "events/all_discord_events.h"
#include "client_config.h"
#include "cache.h"

namespace discpp {
    void EventDispatcher::ReadyEvent(Shard& shard, rapidjson::Document& result) {
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

        if (shard.client.config->type == discpp::TokenType::USER) {
            rapidjson::Document user_json;
            user_json.CopyFrom(result["user"], user_json.GetAllocator());

            discpp::ClientUser client_user(&shard.client, user_json);
            shard.client.client_user = client_user;

            for (const auto& guild : result["guilds"].GetArray()) {
                rapidjson::Document guild_json(rapidjson::kObjectType);
                guild_json.CopyFrom(guild, guild_json.GetAllocator());

                GuildCreateEvent(shard, guild_json);
            }

            for (const auto& private_channel : result["private_channels"].GetArray()) {
                rapidjson::Document private_channel_json(rapidjson::kObjectType);
                private_channel_json.CopyFrom(private_channel, private_channel_json.GetAllocator());

                discpp::Channel dm_channel(&shard.client, private_channel_json);

                shard.client.cache->private_channels.insert({ dm_channel.id, dm_channel });
            }
        } else {
            if (shard.client.client_user.id == 0) {
                // Get the bot user
                std::unique_ptr<rapidjson::Document> user_json = SendGetRequest(&shard.client, Endpoint("/users/@me"), DefaultHeaders(&shard.client), {}, {});

                shard.client.client_user = discpp::ClientUser(&shard.client, *user_json);
            }
        }

        shard.client.event_handler->TriggerEvent<discpp::ReadyEvent>(discpp::ReadyEvent(shard, result));
    }

    void EventDispatcher::ResumedEvent(Shard& shard, rapidjson::Document& result) {
        shard.client.event_handler->TriggerEvent<discpp::ResumedEvent>(discpp::ResumedEvent(shard));
    }

    void EventDispatcher::ReconnectEvent(Shard& shard, rapidjson::Document& result) {
        shard.client.event_handler->TriggerEvent<discpp::ReconnectEvent>(discpp::ReconnectEvent(shard));
    }

    void EventDispatcher::InvalidSessionEvent(Shard& shard, rapidjson::Document& result) {
        shard.client.event_handler->TriggerEvent<discpp::InvalidSessionEvent>(discpp::InvalidSessionEvent(shard));
    }

    void EventDispatcher::ChannelCreateEvent(Shard& shard, rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Channel new_channel(&shard.client, result);
            std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            guild->channels.insert({ new_channel.id, new_channel });

            shard.client.event_handler->TriggerEvent<discpp::ChannelCreateEvent>(discpp::ChannelCreateEvent(shard, new_channel));
        } else {
            discpp::Channel new_channel(&shard.client, result);

            shard.client.cache->private_channels.insert({ new_channel.id, new_channel });
            shard.client.event_handler->TriggerEvent<discpp::ChannelCreateEvent>(discpp::ChannelCreateEvent(shard, new_channel));
        }
    }

    void EventDispatcher::ChannelUpdateEvent(Shard& shard, rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Channel updated_channel(&shard.client, result);
            std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            auto guild_chan_it = guild->channels.find(updated_channel.id);
            if (guild_chan_it != guild->channels.end()) {
                guild_chan_it->second = updated_channel;
            }

            shard.client.event_handler->TriggerEvent<discpp::ChannelUpdateEvent>(discpp::ChannelUpdateEvent(shard, updated_channel));
        } else {
            discpp::Channel updated_channel(&shard.client, result);

            auto client_chan_it = shard.client.cache->private_channels.find(updated_channel.id);
            client_chan_it->second = updated_channel;

            shard.client.event_handler->TriggerEvent<discpp::ChannelUpdateEvent>(discpp::ChannelUpdateEvent(shard, updated_channel));
        }
    }

    void EventDispatcher::ChannelDeleteEvent(Shard& shard, rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Channel deleted_channel(&shard.client, result);
            std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

            shard.client.event_handler->TriggerEvent<discpp::ChannelDeleteEvent>(discpp::ChannelDeleteEvent(shard, deleted_channel));
        } else {
            discpp::Channel deleted_channel(&shard.client, result);

            shard.client.event_handler->TriggerEvent<discpp::ChannelDeleteEvent>(discpp::ChannelDeleteEvent(shard, deleted_channel));
        }
    }

    void EventDispatcher::ChannelPinsUpdateEvent(Shard& shard, rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Channel pin_update_channel = discpp::Channel(&shard.client, SnowflakeFromString(result["channel_id"].GetString()));
            discpp::Guild guild(&shard.client, pin_update_channel.guild_id);

            auto it = guild.channels.find(pin_update_channel.id);
            if (it != guild.channels.end()) {
                it->second.last_pin_timestamp = TimeFromDiscord(result["last_pin_timestamp"].GetString());
            }

            shard.client.event_handler->TriggerEvent<discpp::ChannelPinsUpdateEvent>(discpp::ChannelPinsUpdateEvent(shard, pin_update_channel));
        } else {
            discpp::Channel pin_update_channel = discpp::Channel(&shard.client, SnowflakeFromString(result["channel_id"].GetString()));

            auto it = shard.client.cache->private_channels.find(pin_update_channel.id);
            if (it != shard.client.cache->private_channels.end()) {
                it->second.last_pin_timestamp = TimeFromDiscord(result["last_pin_timestamp"].GetString());
            }

            shard.client.event_handler->TriggerEvent<discpp::ChannelPinsUpdateEvent>(discpp::ChannelPinsUpdateEvent(shard, pin_update_channel));
        }
    }

    void EventDispatcher::GuildCreateEvent(Shard& shard, rapidjson::Document& result) {
        Snowflake guild_id = SnowflakeFromString(result["id"].GetString());

        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(&shard.client, result);
        shard.client.cache->guilds.emplace(guild_id, guild);
        shard.client.cache->members.insert(guild->members.begin(), guild->members.end());

        shard.client.event_handler->TriggerEvent<discpp::GuildCreateEvent>(discpp::GuildCreateEvent(shard, guild));
    }

    void EventDispatcher::GuildUpdateEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(&shard.client, result);

        auto it = shard.client.cache->guilds.find(guild->id);
        if (it != shard.client.cache->guilds.end()) {
            it->second = guild;
        }

        shard.client.event_handler->TriggerEvent<discpp::GuildUpdateEvent>(discpp::GuildUpdateEvent(shard, guild));
    }

    void EventDispatcher::GuildDeleteEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(&shard.client, SnowflakeFromString(result["id"].GetString()));

        shard.client.cache->guilds.erase(guild->id);
        shard.client.event_handler->TriggerEvent<discpp::GuildDeleteEvent>(discpp::GuildDeleteEvent(shard, guild));
    }

    void EventDispatcher::GuildBanAddEvent(Shard& shard, rapidjson::Document& result) {
        discpp::Guild guild(&shard.client, SnowflakeFromString(result["guild_id"].GetString()));
        rapidjson::Document user_json;
        user_json.CopyFrom(result["user"], user_json.GetAllocator());
        discpp::User user(&shard.client, user_json);

        shard.client.event_handler->TriggerEvent<discpp::GuildBanAddEvent>(discpp::GuildBanAddEvent(shard, guild, user));
    }

    void EventDispatcher::GuildBanRemoveEvent(Shard& shard, rapidjson::Document& result) {
        discpp::Guild guild(&shard.client, SnowflakeFromString(result["guild_id"].GetString()));
        rapidjson::Document user_json;
        user_json.CopyFrom(result["user"], user_json.GetAllocator());
        discpp::User user(&shard.client, user_json);

        shard.client.event_handler->TriggerEvent<discpp::GuildBanRemoveEvent>(discpp::GuildBanRemoveEvent(shard, guild, user));
    }

    void EventDispatcher::GuildEmojisUpdateEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

        std::unordered_map<Snowflake, Emoji> emojis;
        for (auto& emoji : result["emojis"].GetArray()) {
            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(emoji, emoji_json.GetAllocator());

            discpp::Emoji tmp = discpp::Emoji(&shard.client, emoji_json);
            emojis.insert({ tmp.id, tmp });
        }

        guild->emojis = emojis;
        auto it = shard.client.cache->guilds.find(guild->id);
        if (it != shard.client.cache->guilds.end()) {
            it->second = guild;
        }

        shard.client.event_handler->TriggerEvent<discpp::GuildEmojisUpdateEvent>(discpp::GuildEmojisUpdateEvent(shard, guild));
    }

    void EventDispatcher::GuildIntegrationsUpdateEvent(Shard& shard, rapidjson::Document& result) {
        shard.client.event_handler->TriggerEvent<discpp::GuildIntegrationsUpdateEvent>(discpp::GuildIntegrationsUpdateEvent(shard, discpp::Guild(&shard.client, SnowflakeFromString(result["guild_id"].GetString()))));
    }

    void EventDispatcher::GuildMemberAddEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));
        std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(&shard.client, result, *guild);
        shard.client.cache->members.insert({ member->user.id, member });

        shard.client.event_handler->TriggerEvent<discpp::GuildMemberAddEvent>(discpp::GuildMemberAddEvent(shard, guild, member));
    }

    void EventDispatcher::GuildMemberRemoveEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));
        std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(&shard.client, SnowflakeFromString(result["user"]["id"].GetString()), *guild);
        shard.client.cache->members.erase(member->user.id);

        shard.client.event_handler->TriggerEvent<discpp::GuildMemberRemoveEvent>(discpp::GuildMemberRemoveEvent(shard, guild, member));
    }

    void EventDispatcher::GuildMemberUpdateEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(&shard.client, SnowflakeFromString(result["guild_id"].GetString()));
        auto it = guild->members.find(static_cast<Snowflake>(SnowflakeFromString(result["user"]["id"].GetString())));

        std::shared_ptr<discpp::Member> member;
        if (it != guild->members.end()) {
            member = it->second;
        } else {
            member = std::make_shared<discpp::Member>(&shard.client, SnowflakeFromString(result["user"]["id"].GetString()), *guild);
            guild->members.insert({ member->user.id, member });
        }

        member->roles.clear();
        for (auto& role : result["roles"].GetArray()) {
            rapidjson::Document role_json;
            role_json.CopyFrom(role, role_json.GetAllocator());

            member->roles.emplace_back(SnowflakeFromString(role_json.GetString()));
        }
        rapidjson::Value::ConstMemberIterator itr = result.FindMember("nick");
        if (discpp::ContainsNotNull(result, "nick")) {
            member->nick = result["nick"].GetString();
        }

        shard.client.event_handler->TriggerEvent<discpp::GuildMemberUpdateEvent>(discpp::GuildMemberUpdateEvent(shard, guild, member));
    }

    void EventDispatcher::GuildMembersChunkEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));
        std::unordered_map<discpp::Snowflake, discpp::Member> members;
        for (auto const& member : result["members"].GetArray()) {
            rapidjson::Document member_json(rapidjson::kObjectType);
            member_json.CopyFrom(member, member_json.GetAllocator());

            discpp::Member tmp(&shard.client, member_json, *guild);
            members.emplace(tmp.user.id, tmp);
        }

        int chunk_index = result["chunk_index"].GetInt();
        int chunk_count = result["chunk_count"].GetInt();

        std::vector<discpp::Presence> presences;
        if (ContainsNotNull(result, "presences")) {
            for (auto const &presence : result["presences"].GetArray()) {
                rapidjson::Document presence_json(rapidjson::kObjectType);
                presence_json.CopyFrom(presence, presence_json.GetAllocator());

                discpp::Presence tmp(&shard.client, presence_json);
                presences.push_back(tmp);
            }
        }
        std::string nonce = GetDataSafely<std::string>(result, "nonce");

        shard.client.event_handler->TriggerEvent<discpp::GuildMembersChunkEvent>(discpp::GuildMembersChunkEvent(shard, guild, members, chunk_index, chunk_count, presences, nonce));
    }

    void EventDispatcher::GuildRoleCreateEvent(Shard& shard, rapidjson::Document& result) {
        std::unique_ptr<rapidjson::Document> role_json = GetDocumentInsideJson(result, "role");
        discpp::Role role(&shard.client, *role_json);

        shard.client.event_handler->TriggerEvent<discpp::GuildRoleCreateEvent>(discpp::GuildRoleCreateEvent(shard, role));
    }

    void EventDispatcher::GuildRoleUpdateEvent(Shard& shard, rapidjson::Document& result) {
        std::unique_ptr<rapidjson::Document> role_json = GetDocumentInsideJson(result, "role");
        discpp::Role role(&shard.client, *role_json);

        shard.client.event_handler->TriggerEvent<discpp::GuildRoleUpdateEvent>(discpp::GuildRoleUpdateEvent(shard, role));
    }

    void EventDispatcher::GuildRoleDeleteEvent(Shard& shard, rapidjson::Document& result) {
        discpp::Guild guild(&shard.client, SnowflakeFromString((result["guild_id"].GetString())));
        discpp::Role role(&shard.client, SnowflakeFromString(result["role_id"].GetString()), guild);

        guild.roles.erase(role.id);

        shard.client.event_handler->TriggerEvent<discpp::GuildRoleDeleteEvent>(discpp::GuildRoleDeleteEvent(shard, role));
    }

    void EventDispatcher::MessageCreateEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Message> message = std::make_shared<discpp::Message>(&shard.client, result);
        if (!shard.client.cache->messages.empty()) {
            if (shard.client.cache->messages.size() >= shard.client.message_cache_count) {
                shard.client.cache->messages.erase(shard.client.cache->messages.begin());
            }

            shard.client.cache->messages.insert({message->id, message});
        }

        if (shard.client.config->type == discpp::TokenType::BOT) {
            shard.client.DoFunctionLater([message, &shard]() { shard.client.fire_command_method(shard, *message); });
        }

        shard.client.event_handler->TriggerEvent<discpp::MessageCreateEvent>(discpp::MessageCreateEvent(shard, *message));
    }

    void EventDispatcher::MessageUpdateEvent(Shard& shard, rapidjson::Document& result) {
        auto message_it = shard.client.cache->messages.find(SnowflakeFromString(result["id"].GetString()));

        discpp::Message old_message(&shard.client);
        discpp::Message edited_message = discpp::Message(&shard.client, result);
        bool is_edited = ContainsNotNull(result, "edited_timestamp");
        if (message_it != shard.client.cache->messages.end()) {
            if (shard.client.cache->messages.size() >= shard.client.message_cache_count) {
                shard.client.cache->messages.erase(shard.client.cache->messages.begin());
            }

            old_message = *message_it->second;
        }

        shard.client.event_handler->TriggerEvent<discpp::MessageUpdateEvent>(discpp::MessageUpdateEvent(shard, edited_message, old_message, is_edited));
    }

    void EventDispatcher::MessageDeleteEvent(Shard& shard, rapidjson::Document& result) {
        auto message = shard.client.cache->messages.find(SnowflakeFromString(result["id"].GetString()));

        if (message != shard.client.cache->messages.end()) {
            shard.client.event_handler->TriggerEvent<discpp::MessageDeleteEvent>(discpp::MessageDeleteEvent(shard, *message->second));

            shard.client.cache->messages.erase(message);
        }
    }

    void EventDispatcher::MessageDeleteBulkEvent(Shard& shard, rapidjson::Document& result) {
        std::vector<discpp::Message> msgs;
        for (auto& id : result["ids"].GetArray()) {
            rapidjson::Document id_json;
            id_json.CopyFrom(id, id_json.GetAllocator());
            auto message = shard.client.cache->messages.find(SnowflakeFromString(id_json.GetString()));

            if (message != shard.client.cache->messages.end()) {
                // Make sure the messages values are up to date.
                if (ContainsNotNull(result, "guild_id")) {
                    std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));;
                    message->second->guild = guild;

                    auto channel_it = guild->channels.find(SnowflakeFromString(result["channel_id"].GetString()));
                    if (channel_it != guild->channels.end()) {
                        message->second->channel = channel_it->second;
                    }
                } else {
                    auto channel_it = shard.client.cache->private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                    if (channel_it != shard.client.cache->private_channels.end()) {
                        message->second->channel = channel_it->second;
                    }
                }

                msgs.push_back(*message->second);
            }
        }

        for (discpp::Message message : msgs) {
            shard.client.cache->messages.erase(message.id);
        }

        shard.client.event_handler->TriggerEvent<discpp::MessageBulkDeleteEvent>(discpp::MessageBulkDeleteEvent(shard, msgs));
    }

    void EventDispatcher::MessageReactionAddEvent(Shard& shard, rapidjson::Document& result) {
        auto message = shard.client.cache->messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != shard.client.cache->messages.end()) {
            // Make sure the messages values are up to date.
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->channel.guild_id = guild->id;
                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = shard.client.cache->private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != shard.client.cache->private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(&shard.client, emoji_json);

            discpp::User user(&shard.client, SnowflakeFromString(result["user_id"].GetString()));

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

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionAddEvent>(discpp::MessageReactionAddEvent(shard, *message->second, emoji, user));
        } else {
            discpp::Channel channel = shard.client.cache->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            auto tmp = channel.RequestMessage(SnowflakeFromString(result["message_id"].GetString()));
            if (!tmp.has_value()) {
                throw std::runtime_error("couldn't find message");
            }
            discpp::Message message = tmp.value();

            if (ContainsNotNull(result, "guild_id")) {
                channel.guild_id = Snowflake(result["guild_id"].GetString());
                message.guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));
            }

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(&shard.client, emoji_json);

            discpp::User user(&shard.client, SnowflakeFromString(result["user_id"].GetString()));

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionAddEvent>(discpp::MessageReactionAddEvent(shard, message, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveEvent(Shard& shard, rapidjson::Document& result) {
        auto message = shard.client.cache->messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != shard.client.cache->messages.end()) {
            // Make sure the messages values are up to date.
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = shard.client.cache->private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != shard.client.cache->private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(&shard.client, emoji_json);

            discpp::User user(&shard.client, SnowflakeFromString(result["user_id"].GetString()));

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

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionRemoveEvent>(discpp::MessageReactionRemoveEvent(shard, *message->second, emoji, user));
        } else {
            discpp::Channel channel = shard.client.cache->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            auto tmp = channel.RequestMessage(SnowflakeFromString(result["message_id"].GetString()));
            if (!tmp.has_value()) {
                throw std::runtime_error("error finding message");
            }
            discpp::Message message = tmp.value();

            if (ContainsNotNull(result, "guild_id")) {
                channel.guild_id = Snowflake(result["guild_id"].GetString());
                message.guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));
            }

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(&shard.client, emoji_json);

            discpp::User user(&shard.client, SnowflakeFromString(result["user_id"].GetString()));

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionRemoveEvent>(discpp::MessageReactionRemoveEvent(shard, message, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveAllEvent(Shard& shard, rapidjson::Document& result) {
        auto message = shard.client.cache->messages.find(SnowflakeFromString(result["message_id"].GetString()));

        if (message != shard.client.cache->messages.end()) {
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(SnowflakeFromString(result["guild_id"].GetString()));

                message->second->guild = guild;
                channel = guild->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            } else {
                auto it = shard.client.cache->private_channels.find(SnowflakeFromString(result["channel_id"].GetString()));

                if (it != shard.client.cache->private_channels.end()) {
                    channel = it->second;
                }
            }
            message->second->channel = channel;

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionRemoveAllEvent>(discpp::MessageReactionRemoveAllEvent(shard, *message->second));
        } else {
            discpp::Channel channel = shard.client.cache->GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
            auto tmp = channel.RequestMessage(SnowflakeFromString(result["message_id"].GetString()));
            if (!tmp.has_value()) {
                throw std::runtime_error("error can't find message");
            }
            discpp::Message message = tmp.value();

            if (ContainsNotNull(result, "guild_id")) {
                channel.guild_id = Snowflake(result["guild_id"].GetString());
                message.guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));
            }

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionRemoveAllEvent>(discpp::MessageReactionRemoveAllEvent(shard, message));
        }
    }

    void EventDispatcher::PresenceUpdateEvent(Shard& shard, rapidjson::Document& result) {
        rapidjson::Document user_json;
        user_json.CopyFrom(result["user"], user_json.GetAllocator());

        shard.client.event_handler->TriggerEvent<discpp::PresenseUpdateEvent>(discpp::PresenseUpdateEvent(shard, discpp::User(&shard.client, user_json)));
    }

    void EventDispatcher::TypingStartEvent(Shard& shard, rapidjson::Document& result) {
        discpp::User user(&shard.client, SnowflakeFromString(result["user_id"].GetString()));

        discpp::Channel channel;
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Guild guild(&shard.client, SnowflakeFromString(result["guild_id"].GetString()));
            channel = guild.GetChannel(SnowflakeFromString(result["channel_id"].GetString()));
        } else {
            channel = discpp::Channel(&shard.client, SnowflakeFromString(result["channel_id"].GetString()));
        }

        int timestamp = result["timestamp"].GetInt();

        shard.client.event_handler->TriggerEvent<discpp::TypingStartEvent>(discpp::TypingStartEvent(shard, user, channel, timestamp));
    }

    void EventDispatcher::UserUpdateEvent(Shard& shard, rapidjson::Document& result) {
        discpp::User user(&shard.client, result);

        shard.client.event_handler->TriggerEvent<discpp::UserUpdateEvent>(discpp::UserUpdateEvent(shard, user));
    }

    void EventDispatcher::VoiceStateUpdateEvent(Shard& shard, rapidjson::Document& result) {
        shard.client.event_handler->TriggerEvent<discpp::VoiceStateUpdateEvent>(discpp::VoiceStateUpdateEvent(shard, result));
    }

    void EventDispatcher::VoiceServerUpdateEvent(Shard& shard, rapidjson::Document& result) {
        shard.client.event_handler->TriggerEvent<discpp::VoiceServerUpdateEvent>(discpp::VoiceServerUpdateEvent(shard, result));
    }

    void EventDispatcher::WebhooksUpdateEvent(Shard& shard, rapidjson::Document& result) {
        discpp::Channel channel(&shard.client, Snowflake(result["channel_id"].GetString()));
        channel.guild_id = SnowflakeFromString(result["guild_id"].GetString());

        shard.client.event_handler->TriggerEvent<discpp::WebhooksUpdateEvent>(discpp::WebhooksUpdateEvent(shard, channel));
    }

    void EventDispatcher::BindEvents() {
        internal_event_map["READY"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::ReadyEvent(shard, result); };
        internal_event_map["RESUMED"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::ResumedEvent(shard, result); };
        internal_event_map["INVALID_SESSION"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::InvalidSessionEvent(shard, result); };
        internal_event_map["CHANNEL_CREATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::ChannelCreateEvent(shard, result); };
        internal_event_map["CHANNEL_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::ChannelUpdateEvent(shard, result); };
        internal_event_map["CHANNEL_DELETE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::ChannelDeleteEvent(shard, result); };
        internal_event_map["CHANNEL_PINS_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::ChannelPinsUpdateEvent(shard, result); };
        internal_event_map["GUILD_CREATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildCreateEvent(shard, result); };
        internal_event_map["GUILD_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildUpdateEvent(shard, result); };
        internal_event_map["GUILD_DELETE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildDeleteEvent(shard, result); };
        internal_event_map["GUILD_BAN_ADD"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildBanAddEvent(shard, result); };
        internal_event_map["GUILD_BAN_REMOVE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildBanRemoveEvent(shard, result); };
        internal_event_map["GUILD_EMOJIS_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildEmojisUpdateEvent(shard, result); };
        internal_event_map["GUILD_INTEGRATIONS_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildIntegrationsUpdateEvent(shard, result); };
        internal_event_map["GUILD_MEMBER_ADD"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildMemberAddEvent(shard, result); };
        internal_event_map["GUILD_MEMBER_REMOVE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildMemberRemoveEvent(shard, result); };
        internal_event_map["GUILD_MEMBER_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildMemberUpdateEvent(shard, result); };
        internal_event_map["GUILD_MEMBERS_CHUNK"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildMembersChunkEvent(shard, result); };
        internal_event_map["GUILD_ROLE_CREATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildRoleCreateEvent(shard, result); };
        internal_event_map["GUILD_ROLE_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildRoleUpdateEvent(shard, result); };
        internal_event_map["GUILD_ROLE_DELETE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::GuildRoleDeleteEvent(shard, result); };
        internal_event_map["MESSAGE_CREATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::MessageCreateEvent(shard, result); };
        internal_event_map["MESSAGE_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::MessageUpdateEvent(shard, result); };
        internal_event_map["MESSAGE_DELETE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::MessageDeleteEvent(shard, result); };
        internal_event_map["MESSAGE_DELETE_BULK"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::MessageDeleteBulkEvent(shard, result); };
        internal_event_map["MESSAGE_REACTION_ADD"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::MessageReactionAddEvent(shard, result); };
        internal_event_map["MESSAGE_REACTION_REMOVE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::MessageReactionRemoveEvent(shard, result); };
        internal_event_map["MESSAGE_REACTION_REMOVE_ALL"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::MessageReactionRemoveAllEvent(shard, result); };
        internal_event_map["PRESENCE_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::PresenceUpdateEvent(shard, result); };
        internal_event_map["TYPING_START"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::TypingStartEvent(shard, result); };
        internal_event_map["USER_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::UserUpdateEvent(shard, result); };
        internal_event_map["VOICE_STATE_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::VoiceStateUpdateEvent(shard, result); };
        internal_event_map["VOICE_SERVER_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::VoiceServerUpdateEvent(shard, result); };
        internal_event_map["WEBHOOKS_UPDATE"] = [&](Shard& shard, rapidjson::Document& result) { discpp::EventDispatcher::WebhooksUpdateEvent(shard, result); };
    }

    void EventDispatcher::RegisterGatewayCustomEvent(const char* event_name, const std::function<void(Shard& shard, const rapidjson::Document&)>& func) {
        internal_event_map[event_name] = func;
    }

    void EventDispatcher::HandleDiscordEvent(Shard& shard, rapidjson::Document& j, const std::string& event_name) {
        //std::shared_ptr<rapidjson::Document> data_ptr;

        rapidjson::Document data(rapidjson::kObjectType);
        data.CopyFrom(j["d"], data.GetAllocator());

        // This shows an error for intellisense but compiles fine.
        std::shared_ptr<rapidjson::Document> data_ptr = std::make_shared<rapidjson::Document>(data.GetType());
        data_ptr->CopyFrom(data, data_ptr->GetAllocator());

        if (ContainsNotNull(j, "s")) {
            shard.last_sequence_number = j["s"].GetInt();
        } else {
            shard.last_sequence_number = 0;
        }

        Shard* sh = &shard;
        if (internal_event_map.find(event_name) != internal_event_map.end()) {
            shard.client.DoFunctionLater([sh, data = std::move(data_ptr), event_name] {
                internal_event_map[event_name](*sh, *data);
            });
        }
    }
}