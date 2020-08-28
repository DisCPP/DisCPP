#include "discpp/snowflake.h"
#include <stdlib.h>
#include "discpp/utils.h"

namespace discpp {

    std::string Snowflake::GetFormattedTimestamp(CommonTimeFormat format, const std::string &format_str, bool localtime) const {
        std::string time_format;
        switch (format) {
            case CommonTimeFormat::ISO8601:
                time_format = "%Y-%m-%dT%H:%M:%SZ";
                break;
            case CommonTimeFormat::DEFAULT:
                time_format = "%F @ %r %Z";
                break;
            default:
                time_format = format_str;
                break;
        }
        return discpp::FormatTime(this->GetRawTime(), time_format);
    }

    time_t Snowflake::GetRawTime() const {
        constexpr static uint64_t discord_epoch = 1420070400000;
        return ((this->id >> 22) + discord_epoch) / 1000;
    }
}