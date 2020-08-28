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
            case CommonTimeFormat::AMERICAN:
                time_format = "%m-%d-%Y %I:%M:%S %p";
                break;
            case CommonTimeFormat::EUROPEAN:
                time_format = "%d-%m-%Y %H:%M:%S";
                break;
            case CommonTimeFormat::DEFAULT:
                time_format = (localtime ? "%F @ %r %Z" : "%F @ %r");
                break;
            default:
                time_format = format_str;
                break;
        }

        time_t time = this->GetRawTime();

        struct tm now{};
#ifndef __linux__
        if (localtime) {
            localtime_s(&now, &time);
        } else {
            gmtime_s(&now, &time);
        }
#else
        if (localtime) {
            now = *localtime(&time);
        } else {
            now = *gmtime(&time);
        }
#endif

        char buffer[256];
        strftime(buffer, sizeof(buffer), time_format.c_str(), &now);

        return buffer;
    }

    time_t Snowflake::GetRawTime() const {
        constexpr static uint64_t discord_epoch = 1420070400000;
        return ((this->id >> 22) + discord_epoch) / 1000;
    }
}