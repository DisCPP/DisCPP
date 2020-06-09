#include "discpp/ratelimit.h"
#include "discpp/utils.h"

namespace discpp {
    Ratelimit::Ratelimit(const rapidjson::Document& json) {
        this->global = GetDataSafely<bool>(json, "global");
        this->message = GetDataSafely<std::string>(json, "message");
        this->retry_after = GetDataSafely<int>(json, "retry_after");
    }
}