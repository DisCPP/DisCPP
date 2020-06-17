#ifndef DISCPP_RATELIMIT_H
#define DISCPP_RATELIMIT_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>

#include <string>
#include <vector>

namespace discpp {
    class Ratelimit {
    public:
        Ratelimit() = default;
        Ratelimit(const rapidjson::Document& json);

        bool global;
        std::string message;
        int retry_after;
    };

    enum class RatelimitTypes : int {
        POST_MESSAGE,
        DELETE_MESSAGE,
        MODIFY_CHANNEL,
        MODIFY_REACTION,
        PATCH_MEMBER,
        PATCH_MEMBER_NICK,
        PATCH_USERNAME,
        ALL_REQUESTS,
        GATEWAY_CONNECT,
        PRESENCE_UPDATE,
        ALL_SENT_MESSAGES
    };

    template<typename T>
    T GetRatelimits(const RatelimitTypes& limit) {
        std::unordered_map<RatelimitTypes, T> tmp = {
                {RatelimitTypes::POST_MESSAGE, 1}, {RatelimitTypes::DELETE_MESSAGE, .2},
                {RatelimitTypes::MODIFY_CHANNEL, 300}, {RatelimitTypes::MODIFY_REACTION, .25},
                {RatelimitTypes::PATCH_MEMBER, 1}, {RatelimitTypes::PATCH_MEMBER_NICK, 1},
                {RatelimitTypes::PATCH_USERNAME, 1800}, {RatelimitTypes::ALL_REQUESTS, .002},
                {RatelimitTypes::GATEWAY_CONNECT, 5}, {RatelimitTypes::PRESENCE_UPDATE, 12},
                {RatelimitTypes::ALL_SENT_MESSAGES, .5}
        };
        return tmp[limit];
    }
}

#endif
