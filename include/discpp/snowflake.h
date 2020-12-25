#ifndef DISCPP_SNOWFLAKE_H
#define DISCPP_SNOWFLAKE_H

#include <string>

namespace discpp {
    enum class CommonTimeFormat : int {
        CUSTOM, DEFAULT, ISO8601, EUROPEAN, AMERICAN
    };

    class Snowflake {
        friend struct SnowflakeHash;
    private:
        uint64_t id;
    public:
        Snowflake() : id(0) {}
        Snowflake(const uint64_t& snowflake) : id(snowflake) {}
        explicit Snowflake(const std::string& snowflake) : id(std::stoll(snowflake)) {}

        operator uint64_t() const { return id; }
        explicit operator std::string() const { return std::to_string(id); }

        std::string GetFormattedTimestamp(CommonTimeFormat format = CommonTimeFormat::DEFAULT, const std::string& format_str = "", bool localtime_format = false) const;
        time_t GetRawTime() const;

        // These operators are defined to make sure that the class isn't casted to uint64 before being checked with the operator.
        // We could probably define more, but these are the ones that would probably be used the most.
        bool operator==(const Snowflake& other) {
            return other.id == this->id;
        }

        bool operator!=(const Snowflake& other) {
            return other.id != this->id;
        }

        bool operator>(const Snowflake& other) {
            return other.id > this->id;
        }

        bool operator<(const Snowflake& other) {
            return other.id < this->id;
        }

        bool operator>=(const Snowflake& other) {
            return other.id >= this->id;
        }

        bool operator<=(const Snowflake& other) {
            return other.id <= this->id;
        }
    };

    struct SnowflakeHash {
        std::size_t operator()(const Snowflake& s) const noexcept {
            return std::hash<uint64_t>{}(s.id);
        }
    };
}

#endif