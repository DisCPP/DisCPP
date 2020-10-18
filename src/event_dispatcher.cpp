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

        if (shard.client.config.type == discpp::TokenType::USER) {
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

                shard.client.cache->CachePrivateChannel(dm_channel);
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
            std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));

            guild->CacheChannel(new_channel);

            shard.client.event_handler->TriggerEvent<discpp::ChannelCreateEvent>(discpp::ChannelCreateEvent(shard, new_channel));
        } else {
            discpp::Channel new_channel(&shard.client, result);

            shard.client.cache->CachePrivateChannel(new_channel);
            shard.client.event_handler->TriggerEvent<discpp::ChannelCreateEvent>(discpp::ChannelCreateEvent(shard, new_channel));
        }
    }

    void EventDispatcher::ChannelUpdateEvent(Shard& shard, rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Channel updated_channel(&shard.client, result);
            std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));

            std::lock_guard<std::mutex> lock_guard(guild->channels_mutex);
            auto guild_chan_it = guild->channels.find(updated_channel.id);
            if (guild_chan_it != guild->channels.end()) {
                guild_chan_it->second = updated_channel;
            }

            shard.client.event_handler->TriggerEvent<discpp::ChannelUpdateEvent>(discpp::ChannelUpdateEvent(shard, updated_channel));
        } else {
            discpp::Channel updated_channel(&shard.client, result);

            std::lock_guard<std::mutex> lock_guard(shard.client.cache->channels_mutex);
            auto client_chan_it = shard.client.cache->private_channels.find(updated_channel.id);
            client_chan_it->second = updated_channel;

            shard.client.event_handler->TriggerEvent<discpp::ChannelUpdateEvent>(discpp::ChannelUpdateEvent(shard, updated_channel));
        }
    }

    void EventDispatcher::ChannelDeleteEvent(Shard& shard, rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Channel deleted_channel(&shard.client, result);
            std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));

            shard.client.event_handler->TriggerEvent<discpp::ChannelDeleteEvent>(discpp::ChannelDeleteEvent(shard, deleted_channel));
        } else {
            discpp::Channel deleted_channel(&shard.client, result);

            shard.client.event_handler->TriggerEvent<discpp::ChannelDeleteEvent>(discpp::ChannelDeleteEvent(shard, deleted_channel));
        }
    }

    void EventDispatcher::ChannelPinsUpdateEvent(Shard& shard, rapidjson::Document& result) {
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Channel pin_update_channel = discpp::Channel(&shard.client, Snowflake(result["channel_id"].GetString()));
            discpp::Guild guild(&shard.client, pin_update_channel.guild_id);

            std::lock_guard<std::mutex> lock_guard(guild.channels_mutex);
            auto it = guild.channels.find(pin_update_channel.id);
            if (it != guild.channels.end()) {
                it->second.last_pin_timestamp = TimeFromDiscord(result["last_pin_timestamp"].GetString());
            }

            shard.client.event_handler->TriggerEvent<discpp::ChannelPinsUpdateEvent>(discpp::ChannelPinsUpdateEvent(shard, pin_update_channel));
        } else {
            discpp::Channel pin_update_channel = discpp::Channel(&shard.client, Snowflake(result["channel_id"].GetString()));

            std::lock_guard<std::mutex> lock_guard(shard.client.cache->channels_mutex);
            auto it = shard.client.cache->private_channels.find(pin_update_channel.id);
            if (it != shard.client.cache->private_channels.end()) {
                it->second.last_pin_timestamp = TimeFromDiscord(result["last_pin_timestamp"].GetString());
            }

            shard.client.event_handler->TriggerEvent<discpp::ChannelPinsUpdateEvent>(discpp::ChannelPinsUpdateEvent(shard, pin_update_channel));
        }
    }

    void EventDispatcher::GuildCreateEvent(Shard& shard, rapidjson::Document& result) {
        Snowflake guild_id = Snowflake(result["id"].GetString());

        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(&shard.client, result);
        shard.client.cache->CacheGuild(guild);

        // Add the guild's members into the clients cache.
        std::lock_guard<std::mutex> lock_guard(shard.client.cache->members_mutex);
        shard.client.cache->members.insert(guild->members.begin(), guild->members.end());

        shard.client.event_handler->TriggerEvent<discpp::GuildCreateEvent>(discpp::GuildCreateEvent(shard, guild));
    }

    void EventDispatcher::GuildUpdateEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(&shard.client, result);

        std::lock_guard<std::mutex> lock_guard(shard.client.cache->guilds_mutex);
        auto it = shard.client.cache->guilds.find(guild->id);
        if (it != shard.client.cache->guilds.end()) {
            it->second = guild;
        }

        shard.client.event_handler->TriggerEvent<discpp::GuildUpdateEvent>(discpp::GuildUpdateEvent(shard, guild));
    }

    void EventDispatcher::GuildDeleteEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(&shard.client, Snowflake(result["id"].GetString()));

        std::lock_guard<std::mutex> lock_guard(shard.client.cache->guilds_mutex);
        shard.client.cache->guilds.erase(guild->id);
        shard.client.event_handler->TriggerEvent<discpp::GuildDeleteEvent>(discpp::GuildDeleteEvent(shard, guild));
    }

    void EventDispatcher::GuildBanAddEvent(Shard& shard, rapidjson::Document& result) {
        discpp::Guild guild(&shard.client, Snowflake(result["guild_id"].GetString()));
        rapidjson::Document user_json;
        user_json.CopyFrom(result["user"], user_json.GetAllocator());
        discpp::User user(&shard.client, user_json);

        shard.client.event_handler->TriggerEvent<discpp::GuildBanAddEvent>(discpp::GuildBanAddEvent(shard, guild, user));
    }

    void EventDispatcher::GuildBanRemoveEvent(Shard& shard, rapidjson::Document& result) {
        discpp::Guild guild(&shard.client, Snowflake(result["guild_id"].GetString()));
        rapidjson::Document user_json;
        user_json.CopyFrom(result["user"], user_json.GetAllocator());
        discpp::User user(&shard.client, user_json);

        shard.client.event_handler->TriggerEvent<discpp::GuildBanRemoveEvent>(discpp::GuildBanRemoveEvent(shard, guild, user));
    }

    void EventDispatcher::GuildEmojisUpdateEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));

        std::unordered_map<Snowflake, Emoji> emojis;
        for (auto& emoji : result["emojis"].GetArray()) {
            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(emoji, emoji_json.GetAllocator());

            discpp::Emoji tmp = discpp::Emoji(&shard.client, emoji_json);
            emojis.insert({ tmp.id, tmp });
        }

        std::lock_guard<std::mutex> emojis_lock_guard(guild->emojis_mutex);
        guild->emojis = emojis;

        std::lock_guard<std::mutex> guilds_lock_guard(shard.client.cache->guilds_mutex);
        auto it = shard.client.cache->guilds.find(guild->id);
        if (it != shard.client.cache->guilds.end()) {
            it->second = guild;
        }

        shard.client.event_handler->TriggerEvent<discpp::GuildEmojisUpdateEvent>(discpp::GuildEmojisUpdateEvent(shard, guild));
    }

    void EventDispatcher::GuildIntegrationsUpdateEvent(Shard& shard, rapidjson::Document& result) {
        shard.client.event_handler->TriggerEvent<discpp::GuildIntegrationsUpdateEvent>(discpp::GuildIntegrationsUpdateEvent(
                shard, discpp::Guild(&shard.client, Snowflake(result["guild_id"].GetString()))));
    }

    void EventDispatcher::GuildMemberAddEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));
        std::shared_ptr<discpp::Member> member = std::make_shared<discpp::Member>(&shard.client, result, guild);

        shard.client.cache->CacheMember(guild, member);

        shard.client.event_handler->TriggerEvent<discpp::GuildMemberAddEvent>(discpp::GuildMemberAddEvent(shard, guild, member));
    }

    void EventDispatcher::GuildMemberRemoveEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));
        std::shared_ptr<discpp::Member> member = guild->GetMember(Snowflake(result["user"]["id"].GetString()));

        std::lock_guard<std::mutex> guild_member_guard(guild->members_mutex);
        guild->members.erase(member->user.id);

        // Only remove the member from member cache if the user isn't in more than one guild.
        if (member->user.GetMutualGuilds().size() == 1) {
            std::lock_guard<std::mutex> lock_guard(shard.client.cache->members_mutex);
            shard.client.cache->members.erase(member->user.id);
        }

        shard.client.event_handler->TriggerEvent<discpp::GuildMemberRemoveEvent>(discpp::GuildMemberRemoveEvent(shard, guild, member));
    }

    void EventDispatcher::GuildMemberUpdateEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = std::make_shared<discpp::Guild>(&shard.client, Snowflake(result["guild_id"].GetString()));
        std::lock_guard<std::mutex> guild_member_guard(guild->members_mutex);
        auto it = guild->members.find(Snowflake(result["user"]["id"].GetString()));

        std::shared_ptr<discpp::Member> member;
        if (it != guild->members.end()) {
            member = it->second;
        } else {
            member = std::make_shared<discpp::Member>(&shard.client, Snowflake(result["user"]["id"].GetString()), guild);
            guild->members.insert({ member->user.id, member });
        }

        member->roles.clear();
        for (auto& role : result["roles"].GetArray()) {
            rapidjson::Document role_json;
            role_json.CopyFrom(role, role_json.GetAllocator());

            member->roles.emplace_back(Snowflake(role_json.GetString()));
        }
        rapidjson::Value::ConstMemberIterator itr = result.FindMember("nick");
        if (discpp::ContainsNotNull(result, "nick")) {
            member->nick = result["nick"].GetString();
        }

        shard.client.event_handler->TriggerEvent<discpp::GuildMemberUpdateEvent>(discpp::GuildMemberUpdateEvent(shard, guild, member));
    }

    void EventDispatcher::GuildMembersChunkEvent(Shard& shard, rapidjson::Document& result) {
        std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));
        std::unordered_map<discpp::Snowflake, discpp::Member> members;
        for (auto const& member : result["members"].GetArray()) {
            rapidjson::Document member_json(rapidjson::kObjectType);
            member_json.CopyFrom(member, member_json.GetAllocator());

            discpp::Member tmp(&shard.client, member_json, guild);
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
        discpp::Guild guild(&shard.client, Snowflake((result["guild_id"].GetString())));
        discpp::Role role(&shard.client, Snowflake(result["role_id"].GetString()), guild);

        std::lock_guard<std::mutex> guild_roles_guard(guild.roles_mutex);
        guild.roles.erase(role.id);

        shard.client.event_handler->TriggerEvent<discpp::GuildRoleDeleteEvent>(discpp::GuildRoleDeleteEvent(shard, role));
    }

    void EventDispatcher::MessageCreateEvent(Shard& shard, rapidjson::Document& result) {
        std::lock_guard<std::mutex> messages_guard(shard.client.cache->messages_mutex);
        std::shared_ptr<discpp::Message> message = std::make_shared<discpp::Message>(&shard.client, result);
        if (!shard.client.cache->messages.empty()) {
            if (shard.client.cache->messages.size() >= shard.client.message_cache_count) {
                shard.client.cache->messages.erase(shard.client.cache->messages.begin());
            }

            shard.client.cache->messages.insert({message->id, message});
        }

        // Only run the command handler if the token is a bot.
        if (shard.client.config.type == discpp::TokenType::BOT) {
            shard.client.DoFunctionLater([message, &shard]() { shard.client.fire_command_method(shard, *message); });
        }

        shard.client.event_handler->TriggerEvent<discpp::MessageCreateEvent>(discpp::MessageCreateEvent(shard, *message));
    }

    void EventDispatcher::MessageUpdateEvent(Shard& shard, rapidjson::Document& result) {
        std::lock_guard<std::mutex> messages_guard(shard.client.cache->messages_mutex);
        auto message_it = shard.client.cache->messages.find(Snowflake(result["id"].GetString()));

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
        std::lock_guard<std::mutex> messages_guard(shard.client.cache->messages_mutex);
        auto message = shard.client.cache->messages.find(Snowflake(result["id"].GetString()));

        if (message != shard.client.cache->messages.end()) {
            shard.client.event_handler->TriggerEvent<discpp::MessageDeleteEvent>(discpp::MessageDeleteEvent(shard, *message->second));

            shard.client.cache->messages.erase(message);
        }
    }

    void EventDispatcher::MessageDeleteBulkEvent(Shard& shard, rapidjson::Document& result) {
        std::lock_guard<std::mutex> messages_guard(shard.client.cache->messages_mutex);
        std::vector<discpp::Message> msgs;
        for (auto& id : result["ids"].GetArray()) {
            rapidjson::Document id_json;
            id_json.CopyFrom(id, id_json.GetAllocator());
            auto message = shard.client.cache->messages.find(Snowflake(id_json.GetString()));

            if (message != shard.client.cache->messages.end()) {
                // Make sure the messages values are up to date.
                if (ContainsNotNull(result, "guild_id")) {
                    std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));;
                    message->second->guild = guild;

                    auto channel_it = guild->channels.find(Snowflake(result["channel_id"].GetString()));
                    if (channel_it != guild->channels.end()) {
                        message->second->channel = channel_it->second;
                    }
                } else {
                    auto channel_it = shard.client.cache->private_channels.find(Snowflake(result["channel_id"].GetString()));

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
        auto message = shard.client.cache->GetDiscordMessage(Snowflake(result["channel_id"].GetString()), Snowflake(result["message_id"].GetString()));

        if (message.id != 0) {
            // Make sure the messages values are up to date.
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));

                message.channel.guild_id = guild->id;
                message.guild = guild;
                channel = guild->GetChannel(Snowflake(result["channel_id"].GetString()));
            } else {
                std::lock_guard<std::mutex> channels_guard(shard.client.cache->channels_mutex);
                auto it = shard.client.cache->private_channels.find(Snowflake(result["channel_id"].GetString()));

                if (it != shard.client.cache->private_channels.end()) {
                    channel = it->second;
                }
            }
            message.channel = channel;

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(&shard.client, emoji_json);

            discpp::User user(&shard.client, Snowflake(result["user_id"].GetString()));

            auto reaction = std::find_if(message.reactions.begin(), message.reactions.end(),
            [&emoji](discpp::Reaction react) {
                return react.emoji == emoji;
            });

            if (reaction != message.reactions.end()) {
                reaction->count++;

                if (user.IsBot()) {
                    reaction->from_bot = true;
                }
            } else {
                discpp::Reaction r = discpp::Reaction(1, user.IsBot(), emoji);
                message.reactions.push_back(r);
            }

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionAddEvent>(discpp::MessageReactionAddEvent(shard, message, emoji, user));
        } else {
            discpp::Channel channel = shard.client.cache->GetChannel(Snowflake(result["channel_id"].GetString()));
            message = channel.RequestMessage(Snowflake(result["message_id"].GetString()));

            if (ContainsNotNull(result, "guild_id")) {
                channel.guild_id = Snowflake(result["guild_id"].GetString());
                message.guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));
            }

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(&shard.client, emoji_json);

            discpp::User user(&shard.client, Snowflake(result["user_id"].GetString()));

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionAddEvent>(discpp::MessageReactionAddEvent(shard, message, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveEvent(Shard& shard, rapidjson::Document& result) {
        auto message = shard.client.cache->GetDiscordMessage(Snowflake(result["channel_id"].GetString()), Snowflake(result["message_id"].GetString()));

        if (message.id != 0) {
            // Make sure the messages values are up to date.
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));

                message.guild = guild;
                channel = guild->GetChannel(Snowflake(result["channel_id"].GetString()));
            } else {
                std::lock_guard<std::mutex> channels_guard(shard.client.cache->channels_mutex);
                auto it = shard.client.cache->private_channels.find(Snowflake(result["channel_id"].GetString()));

                if (it != shard.client.cache->private_channels.end()) {
                    channel = it->second;
                }
            }
            message.channel = channel;

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(&shard.client, emoji_json);

            discpp::User user(&shard.client, Snowflake(result["user_id"].GetString()));

            auto reaction = std::find_if(message.reactions.begin(), message.reactions.end(),
                 [&emoji](discpp::Reaction react) {
                     return react.emoji == emoji;
                 });

            if (reaction != message.reactions.end()) {
                if (reaction->count == 1) {
                    message.reactions.erase(reaction);
                } else {
                    reaction->count--;

                    // @TODO: Add a way to change reaction::from_bot
                }
            }

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionRemoveEvent>(discpp::MessageReactionRemoveEvent(shard, message, emoji, user));
        } else {
            discpp::Channel channel = shard.client.cache->GetChannel(Snowflake(result["channel_id"].GetString()));
            message = channel.RequestMessage(Snowflake(result["message_id"].GetString()));

            if (ContainsNotNull(result, "guild_id")) {
                channel.guild_id = Snowflake(result["guild_id"].GetString());
                message.guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));
            }

            rapidjson::Document emoji_json;
            emoji_json.CopyFrom(result["emoji"], emoji_json.GetAllocator());
            discpp::Emoji emoji(&shard.client, emoji_json);

            discpp::User user(&shard.client, Snowflake(result["user_id"].GetString()));

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionRemoveEvent>(discpp::MessageReactionRemoveEvent(shard, message, emoji, user));
        }
    }

    void EventDispatcher::MessageReactionRemoveAllEvent(Shard& shard, rapidjson::Document& result) {
        auto message = shard.client.cache->GetDiscordMessage(Snowflake(result["channel_id"].GetString()), Snowflake(result["message_id"].GetString()));

        if (message.id != 0) {
            discpp::Channel channel;
            if (ContainsNotNull(result, "guild_id")) {
                std::shared_ptr<discpp::Guild> guild = shard.client.cache->GetGuild(Snowflake(result["guild_id"].GetString()));

                message.guild = guild;
                channel = guild->GetChannel(Snowflake(result["channel_id"].GetString()));
            } else {
                std::lock_guard<std::mutex> channels_guard(shard.client.cache->channels_mutex);
                auto it = shard.client.cache->private_channels.find(Snowflake(result["channel_id"].GetString()));

                if (it != shard.client.cache->private_channels.end()) {
                    channel = it->second;
                }
            }
            message.channel = channel;

            shard.client.event_handler->TriggerEvent<discpp::MessageReactionRemoveAllEvent>(discpp::MessageReactionRemoveAllEvent(shard, message));
        } else {
            discpp::Channel channel = shard.client.cache->GetChannel(Snowflake(result["channel_id"].GetString()));
            message = channel.RequestMessage(Snowflake(result["message_id"].GetString()));

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
        discpp::User user(&shard.client, Snowflake(result["user_id"].GetString()));

        discpp::Channel channel;
        if (ContainsNotNull(result, "guild_id")) {
            discpp::Guild guild(&shard.client, Snowflake(result["guild_id"].GetString()));
            channel = guild.GetChannel(Snowflake(result["channel_id"].GetString()));
        } else {
            channel = discpp::Channel(&shard.client, Snowflake(result["channel_id"].GetString()));
        }

        int timestamp = result["timestamp"].GetInt();

        shard.client.event_handler->TriggerEvent<discpp::TypingStartEvent>(discpp::TypingStartEvent(shard, user, channel, timestamp));
    }

    void EventDispatcher::UserUpdateEvent(Shard& shard, rapidjson::Document& result) {
        discpp::User user(&shard.client, result);

        shard.client.event_handler->TriggerEvent<discpp::UserUpdateEvent>(discpp::UserUpdateEvent(shard, user));
    }

    void EventDispatcher::VoiceStateUpdateEvent(Shard& shard, rapidjson::Document& result) {
        VoiceState voice_state(&shard.client, result);
        auto guild = shard.client.cache->GetGuild(voice_state.guild_id);

        auto it = std::find_if(guild->voice_states.begin(), guild->voice_states.end(), [&](const discpp::VoiceState& v) { return voice_state.user_id == v.user_id; });
        if (it != guild->voice_states.end()) {
            if (voice_state.channel_id == 0) {
                guild->voice_states.erase(it);
            } else {
                *it = voice_state;
            }
        } else {
            guild->voice_states.emplace_back(voice_state);
        }

        shard.client.event_handler->TriggerEvent<discpp::VoiceStateUpdateEvent>(discpp::VoiceStateUpdateEvent(shard, voice_state));
    }

    void EventDispatcher::VoiceServerUpdateEvent(Shard& shard, rapidjson::Document& result) {
        shard.client.event_handler->TriggerEvent<discpp::VoiceServerUpdateEvent>(discpp::VoiceServerUpdateEvent(shard, result));
    }

    void EventDispatcher::WebhooksUpdateEvent(Shard& shard, rapidjson::Document& result) {
        discpp::Channel channel(&shard.client, Snowflake(result["channel_id"].GetString()));
        channel.guild_id = Snowflake(result["guild_id"].GetString());

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
            shard.last_sequence_number.store(j["s"].GetInt());
        } else {
            shard.last_sequence_number.store(0);
        }

        Shard* sh = &shard;
        if (internal_event_map.find(event_name) != internal_event_map.end()) {
            shard.client.DoFunctionLater([sh, data = std::move(data_ptr), event_name] {
                internal_event_map[event_name](*sh, *data);
            });
        }
    }
}