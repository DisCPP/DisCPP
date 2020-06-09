#ifndef DISCPP_RATELIMIT_H
#define DISCPP_RATELIMIT_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>

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
