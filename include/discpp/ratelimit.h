#ifndef DISCPP_RATELIMIT_H
#define DISCPP_RATELIMIT_H

#include <rapidjson/rapidjson.h>
#include <string>

namespace discpp {
    class Ratelimit {
    public:
        Ratelimit() = default;
        Ratelimit(const rapidjson::Document& json);

        bool global;
        std::string message;
        int retry_after;
    };
}

#endif
