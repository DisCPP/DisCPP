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
    public:
        Cache(discpp::Client* client);

        std::unordered_map<Snowflake, std::shared_ptr<Member>> members; /**< List of members the current bot can access. */
        std::unordered_map<Snowflake, std::shared_ptr<Guild>> guilds; /**< List of guilds the current bot can access. */
        std::unordered_map<Snowflake, std::shared_ptr<Message>> messages; /**< List of messages the current bot can access. */
        std::unordered_map<discpp::Snowflake, discpp::Channel> private_channels; /**< List of dm channels the current client can access. */

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
