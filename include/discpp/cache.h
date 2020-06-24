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
        std::unordered_map<Snowflake, std::shared_ptr<Member>> members; /**< List of members the current bot can access. */
        std::unordered_map<Snowflake, std::shared_ptr<Guild>> guilds; /**< List of guilds the current bot can access. */
        std::unordered_map<Snowflake, std::shared_ptr<Message>> messages; /**< List of messages the current bot can access. */
        std::unordered_map<discpp::Snowflake, discpp::Channel> private_channels; /**< List of dm channels the current client can access. */

        /**
         * @brief Gets a discpp::Guild from a guild id.
         *
         * If the guild isn't cached for some reason, it will retrieved via the REST api.
         * If the guild was requested from REST api, it will be added to cache.
         *
         * @param[in] guild_id The guild id of the guild you want to get.
         *
         * @return std::shared_ptr<discpp::Guild>
         */
        std::shared_ptr<discpp::Guild> GetGuild(const Snowflake& guild_id);

        /**
         * @brief Gets a channel from guild cache and private caches.
         *
         * If the channel isn't cached for some reason, it will retrieved via the REST api.
         *
         * @return discpp::Channel
         */
        discpp::Channel GetChannel(const discpp::Snowflake& id);

        /**
         * @brief Get a DM channel with id
         *
         * If the channel isn't cached for some reason, it will retrieved via the REST api.
         *
         * @return discpp::Channel
         */
        discpp::Channel GetDMChannel(const discpp::Snowflake& id);

        /**
         * @brief Get a DM channel with id
         *
         * If the member isn't cached for some reason, it will retrieved via the REST api.
         *
         * @return discpp::Member
         */
        discpp::Member GetMember(const discpp::Snowflake& id);
    };
}

#endif //DISCPP_CACHE_H
