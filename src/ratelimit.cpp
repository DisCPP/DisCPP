#include "ratelimit.h"
#include "utils.h"
#include "json_object.h"

namespace discpp {
    Ratelimit::Ratelimit(const discpp::JsonObject& json) {
        this->global = json.Get<bool>("global");
        this->message = json.Get<std::string>("message");
        this->retry_after = json.Get<int>("retry_after");
    }
}