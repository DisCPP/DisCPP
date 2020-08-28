#include "discpp/snowflake.h"
#include <stdlib.h>
#include "discpp/utils.h"

namespace discpp {

    std::string Snowflake::GetFormattedTimestamp(CommonTimeFormat format, const std::string &format_str, bool localtime) const {
        bool use_format_str = true;
        std::string time_format;
        switch (format) {
            case CommonTimeFormat::ISO8601:
                use_format_str = false;
                time_format = "%Y-%m-%dT%H:%M:%SZ";
                break;
            case CommonTimeFormat::DEFAULT:
                use_format_str = false;
                time_format = "%F @ %r %Z";
                break;
            default:
                use_format_str = true;
                time_format = format_str;
                break;
        }
        return discpp::FormatTime(this->GetRawTime(), format_str);
    }

    time_t Snowflake::GetRawTime() const {
        constexpr static uint64_t discord_epoch = 1420070400000;
        return ((this->id >> 22) + discord_epoch) / 1000;
    }
}