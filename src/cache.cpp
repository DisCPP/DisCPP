//
// Created by SeanOMik on 6/23/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#include "cache.h"
#include "exceptions.h"
#include "utils.h"

std::shared_ptr<discpp::Guild> discpp::Cache::GetGuild(const discpp::Snowflake &guild_id) {
    auto it = guilds.find(guild_id);
    if (it != guilds.end()) {
        return it->second;
    }

    rapidjson::Document result = SendGetRequest(Endpoint("/guilds/" + std::to_string(guild_id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
    auto guild = std::make_shared<discpp::Guild>(result);
    guilds.emplace(guild->id, guild);
    return guild;
}

discpp::Channel discpp::Cache::GetChannel(const discpp::Snowflake &id) {
    discpp::Channel channel = GetDMChannel(id);

    if (channel.id == 0) {
        for (const auto &guild : guilds) {
            channel = guild.second->GetChannel(id);

            if (channel.id != 0) return channel;
        }

        rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);

        return discpp::Channel(result);
    } else {
        return channel;
    }
}

discpp::Channel discpp::Cache::GetDMChannel(const discpp::Snowflake &id) {
    auto it = private_channels.find(id);
    if (it != private_channels.end()) {
        return it->second;
    }

    rapidjson::Document result = SendGetRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
    discpp::Channel channel(result);

    private_channels.emplace(channel.id, channel);
    return channel;
}
