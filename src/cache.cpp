//
// Created by SeanOMik on 6/23/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#include "cache.h"
#include "exceptions.h"
#include "utils.h"

std::shared_ptr<discpp::Guild> discpp::Cache::GetGuild(const discpp::Snowflake &guild_id, bool can_request) {
    auto it = guilds.find(guild_id);
    if (it != guilds.end()) {
        return it->second;
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(guild_id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
        auto guild = std::make_shared<discpp::Guild>(*result);
        guilds.emplace(guild->id, guild);
        return guild;
    } else {
        throw exceptions::DiscordObjectNotFound("Guild not found of id: " + std::to_string(guild_id));
    }
}

discpp::Channel discpp::Cache::GetChannel(const discpp::Snowflake &id, bool can_request) {
    try {
        return GetDMChannel(id, can_request);
    } catch (exceptions::DiscordObjectNotFound) {
        for (const auto &guild : guilds) {
            discpp::Channel channel = guild.second->GetChannel(id);

            if (channel.id != 0) return channel;
        }

        if (can_request) {
            std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
            return discpp::Channel(*result);
        } else {
            throw exceptions::DiscordObjectNotFound("Channel not found of id: " + std::to_string(id));
        }
    }
}

discpp::Channel discpp::Cache::GetDMChannel(const discpp::Snowflake &id, bool can_request) {
    auto it = private_channels.find(id);
    if (it != private_channels.end()) {
        return it->second;
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/channels/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
        discpp::Channel channel(*result);

        private_channels.emplace(channel.id, channel);
        return channel;
    } else {
        throw exceptions::DiscordObjectNotFound("DM Channel not found of id: " + std::to_string(id));
    }
}

std::shared_ptr<discpp::Member> discpp::Cache::GetMember(const discpp::Snowflake& guild_id, const discpp::Snowflake &id, bool can_request) {
    auto it = members.find(id);
    if (it != members.end()) {
        return it->second;
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/guilds/" + std::to_string(guild_id) + "/members/" + std::to_string(id)), DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
        auto member = std::make_shared<discpp::Member>(*result, guild_id);
        members.emplace(member->user.id, member);
        return member;
    } else {
        throw exceptions::DiscordObjectNotFound("Member not found of id: " + std::to_string(guild_id) + ", in guild of id: " + std::to_string(guild_id));
    }
}

discpp::Message discpp::Cache::GetDiscordMessage(const discpp::Snowflake &channel_id, const discpp::Snowflake &id, bool can_request) {
    auto message = messages.find(id);
    if (message != messages.end()) {
        return *message->second;
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = SendGetRequest(Endpoint("/channels/" + std::to_string(channel_id) + "/messages/" + std::to_string(id)),
            DefaultHeaders(), channel_id, RateLimitBucketType::CHANNEL);

        return Message(*result);
    } else {
        throw exceptions::DiscordObjectNotFound("Message of id \"" + std::to_string(id) + "\" was not found!");
    }
}
