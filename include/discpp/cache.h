//
// Created by SeanOMik on 6/23/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#ifndef DISCPP_CACHE_H
#define DISCPP_CACHE_H

#include "member.h"
#include "guild.h"
#include "message.h"
#include "channel.h"

#include <memory>
#include <unordered_map>

namespace discpp {
    class Cache {
        friend class EventDispatcher;
        friend class User;
    private:
        std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Member>, discpp::SnowflakeHash> members;
        std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Guild>, discpp::SnowflakeHash> guilds;
        std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Message>, discpp::SnowflakeHash> messages;
        std::unordered_map<discpp::Snowflake, discpp::Channel, discpp::SnowflakeHash> private_channels;

        std::mutex members_mutex;
        std::mutex guilds_mutex;
        std::mutex messages_mutex;
        std::mutex channels_mutex;
    public:
        Cache(discpp::Client* client);

        /**
         * @brief Get all members the bot is handling. Do not modify contents since it will break thread-safety!
         * If you just want to get a member, use discpp::Cache::GetMember().
         *
         * @return const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Member>, discpp::SnowflakeHash>
         */
        const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Member>, discpp::SnowflakeHash> GetMembers();

        /**
         * @brief Get all guilds the bot is in. Do not modify contents since it will break thread-safety!
         * If you just want to get a guild, use discpp::Cache::GetGuild().
         *
         * @return const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Guild>, discpp::SnowflakeHash>
         */
        const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Guild>, discpp::SnowflakeHash> GetGuilds();

        /**
         * @brief Get all messages the bot has seen or requested. Do not modify contents since it will break thread-safety!
         * If you just want to get a message, use discpp::Cache::GetMessage().
         *
         * @return const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Message>, discpp::SnowflakeHash>
         */
        const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Message>, discpp::SnowflakeHash> GetMessages();

        /**
         * @brief Get all private channels the bot is handling. Do not modify contents since it will break thread-safety!
         * If you just want to get a private channel, use discpp::Cache::GetPrivateChannel().
         *
         * @return const std::unordered_map<discpp::Snowflake, discpp::Channel, discpp::SnowflakeHash>
         */
        const std::unordered_map<discpp::Snowflake, discpp::Channel, discpp::SnowflakeHash> GetPrivateChannels();

        /**
         * @brief Cache a discpp::Member.
         *
         * @param[in] guild The guild that the member is in.
         * @param[in] member The member to cache.
         *
         * @return void
         */
        void CacheMember(std::shared_ptr<discpp::Guild> guild, std::shared_ptr<discpp::Member> member);

        /**
         * @brief Cache a discpp::Guild.
         *
         * @param[in] guild The guild to cache.
         *
         * @return void
         */
        void CacheGuild(std::shared_ptr<discpp::Guild> guild);

        /**
         * @brief Cache a discpp::Message.
         *
         * @param[in] message The message to cache.
         *
         * @return void
         */
        void CacheMessage(std::shared_ptr<discpp::Message> message);

        /**
         * @brief Cache a discpp::Channel that's a DM channel.
         *
         * @param[in] channel The DM channel to cache.
         *
         * @return void
         */
        void CachePrivateChannel(discpp::Channel channel);

        /**
         * @brief Gets a discpp::Guild from a guild id.
         *
         * If you set `can_request` to true, and the guild is not found in cache, then we will request
         * the guild from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * @param[in] guild_id The guild id of the guild you want to get.
         * @param[in] can_request Determines if we can request the guild from REST API if its not found in cache.
         *
         * @return std::shared_ptr<discpp::Guild>
         */
        std::shared_ptr<discpp::Guild> GetGuild(const Snowflake& guild_id, bool can_request = false);

        /**
         * @brief Gets a channel from guild cache and private caches.
         *
         * If you set `can_request` to true, and the channel is not found in cache, then we will request
         * the channel from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * If the id is of a DM channel's id, it will return that DM channel.
         *
         * @return discpp::Channel
         */
        discpp::Channel GetChannel(const discpp::Snowflake& id, bool can_request = false);

        /**
         * @brief Get a DM channel with id.
         *
         * If you set `can_request` to true, and the dm channel is not found in cache, then we will request
         * the dm channel from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * @return discpp::Channel
         */
        discpp::Channel GetDMChannel(const discpp::Snowflake& id, bool can_request = false);

        /**
         * @brief Get a member with id.
         *
         * If you set `can_request` to true, and the member is not found in cache, then we will request
         * the member from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * @return discpp::Member
         */
        std::shared_ptr<discpp::Member> GetMember(const std::shared_ptr<Guild> &guild, const discpp::Snowflake& id, bool can_request = false);

        /**
         * @brief Get a user from id.
         *
         * If you set `can_request` to true, and the user is not found in the member cache, then we will request
         * the user from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * @return discpp::Member
         */
        discpp::User GetUser(const discpp::Snowflake& id, bool can_request = false);

        /**
         * @brief Get a member with id.
         *
         * If you set `can_request` to true, and the message is not found in cache, then we will request
         * the message from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * @param[in] channel_id The channel_id of the message.
         * @param[in] id The id of the message.
         * @param[in] can_request Whether or not the library can request the message from the REST API.
         *
         * @return discpp::Message
         */
        discpp::Message GetDiscordMessage(const Snowflake& channel_id, const Snowflake& id, bool can_request = false);
    private:
        discpp::Client* client;
    };
}

#endif //DISCPP_CACHE_H
