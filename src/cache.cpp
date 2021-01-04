//
// Created by SeanOMik on 6/23/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#include "cache.h"
#include "exceptions.h"
#include "utils.h"
#include "client.h"
#include "http_client.h"

discpp::Cache::Cache(discpp::Client *client) : client(client) {

}

const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Member>, discpp::SnowflakeHash> discpp::Cache::GetMembers() {
    std::lock_guard<std::mutex> lock_guard(members_mutex);

    return members;
}

const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Guild>, discpp::SnowflakeHash> discpp::Cache::GetGuilds() {
    std::lock_guard<std::mutex> lock_guard(guilds_mutex);

    return guilds;
}

const std::unordered_map<discpp::Snowflake, std::shared_ptr<discpp::Message>, discpp::SnowflakeHash> discpp::Cache::GetMessages() {
    std::lock_guard<std::mutex> lock_guard(messages_mutex);

    return messages;
}

const std::unordered_map<discpp::Snowflake, discpp::Channel, discpp::SnowflakeHash> discpp::Cache::GetPrivateChannels() {
    std::lock_guard<std::mutex> lock_guard(channels_mutex);

    return private_channels;
}

void discpp::Cache::CacheMember(std::shared_ptr<discpp::Guild> guild, std::shared_ptr<discpp::Member> member) {
    std::lock_guard<std::mutex> members_lock_guard(members_mutex);
    std::lock_guard<std::mutex> guilds_lock_guard(guilds_mutex);

    guild->CacheMember(member);
    members.emplace(member->user.id, member);
}

void discpp::Cache::CacheGuild(std::shared_ptr<discpp::Guild> guild) {
    std::lock_guard<std::mutex> lock_guard(guilds_mutex);

    guilds.emplace(guild->id, guild);
}

void discpp::Cache::CacheMessage(std::shared_ptr<discpp::Message> message) {
    std::lock_guard<std::mutex> lock_guard(messages_mutex);

    messages.emplace(message->id, message);
}

void discpp::Cache::CachePrivateChannel(discpp::Channel channel) {
    std::lock_guard<std::mutex> lock_guard(channels_mutex);

    private_channels.emplace(channel.id, channel);
}

std::shared_ptr<discpp::Guild> discpp::Cache::GetGuild(const discpp::Snowflake &guild_id, bool can_request) {
    std::lock_guard<std::mutex> lock_guard(guilds_mutex);
    auto it = guilds.find(guild_id);
    if (it != guilds.end()) {
        return it->second;
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = client->http_client->SendGetRequest(Endpoint("/guilds/" + (std::string) guild_id), client->http_client->DefaultHeaders(), guild_id, RateLimitBucketType::GUILD);
        auto guild = std::make_shared<discpp::Guild>(client, *result);
        guilds.emplace(guild->id, guild);
        return guild;
    } else {
        throw exceptions::DiscordObjectNotFound("Guild not found of id: " + (std::string) guild_id);
    }
}

discpp::Channel discpp::Cache::GetChannel(const discpp::Snowflake &id, bool can_request) {
    try {
        return GetDMChannel(id, can_request);
    } catch (const exceptions::DiscordObjectNotFound&) {
        std::lock_guard<std::mutex> guilds_lock_guard(guilds_mutex);
        for (const auto &guild : guilds) {
            try {
                discpp::Channel channel = guild.second->GetChannel(id);

                if (channel.id != 0) return channel;
            } catch (const exceptions::DiscordObjectNotFound&) { }
        }

        if (can_request) {
            std::unique_ptr<rapidjson::Document> result = client->http_client->SendGetRequest(Endpoint("/channels/" + (std::string) id), client->http_client->DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
            return discpp::Channel(client, *result);
        } else {
            throw exceptions::DiscordObjectNotFound("Channel not found of id: " + (std::string) id);
        }
    }
}

discpp::Channel discpp::Cache::GetDMChannel(const discpp::Snowflake &id, bool can_request) {
    std::lock_guard<std::mutex> lock_guard(channels_mutex);
    auto it = private_channels.find(id);
    if (it != private_channels.end()) {
        return it->second;
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = client->http_client->SendGetRequest(Endpoint("/channels/" + (std::string) id), client->http_client->DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
        discpp::Channel channel(client, *result);

        private_channels.emplace(channel.id, channel);
        return channel;
    } else {
        throw exceptions::DiscordObjectNotFound("DM Channel not found of id: " + (std::string) id);
    }
}

std::shared_ptr<discpp::Member> discpp::Cache::GetMember(const std::shared_ptr<Guild> &guild, const discpp::Snowflake &id, bool can_request) {
    std::lock_guard<std::mutex> lock_guard(members_mutex);
    auto it = members.find(id);
    if (it != members.end()) {
        return it->second;
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = client->http_client->SendGetRequest(Endpoint("/guilds/" + (std::string) guild->id + "/members/" + (std::string) id), client->http_client->DefaultHeaders(), guild->id, RateLimitBucketType::GUILD);
        auto member = std::make_shared<discpp::Member>(client, *result, guild);
        members.emplace(member->user.id, member);
        return member;
    } else {
        throw exceptions::DiscordObjectNotFound("Member not found of id: " + (std::string) id + ", in guild of id: " + (std::string) guild->id);
    }
}

discpp::Message discpp::Cache::GetDiscordMessage(const discpp::Snowflake &channel_id, const discpp::Snowflake &id, bool can_request) {
    std::lock_guard<std::mutex> lock_guard(messages_mutex);
    auto message = messages.find(id);
    if (message != messages.end()) {
        return *message->second;
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = client->http_client->SendGetRequest(Endpoint("/channels/" + (std::string) channel_id + "/messages/" + (std::string) id),
            client->http_client->DefaultHeaders(), channel_id, RateLimitBucketType::CHANNEL);

        return Message(client, *result);
    } else {
        throw exceptions::DiscordObjectNotFound("Message of id \"" + (std::string) id + "\" was not found!");
    }
}

discpp::User discpp::Cache::GetUser(const discpp::Snowflake &id, bool can_request) {
    std::lock_guard<std::mutex> lock_guard(members_mutex);
    std::lock_guard<std::mutex> guilds_lock_guard(guilds_mutex);

    for (auto const& guild : guilds) {
        auto it = guild.second->members.find(id);
        if (it != guild.second->members.end()) {
            return it->second->user;
        }
    }

    if (can_request) {
        std::unique_ptr<rapidjson::Document> result = client->http_client->SendGetRequest(Endpoint("/users/" + (std::string) id), client->http_client->DefaultHeaders(), id, RateLimitBucketType::GLOBAL);
        return discpp::User(client, *result);
    } else {
        throw exceptions::DiscordObjectNotFound("User not found of id: " + (std::string) id + "!");
    }
}
