#ifndef DISCPP_SNOWFLAKE_H
#define DISCPP_SNOWFLAKE_H

#include <string>

namespace discpp {
    class Snowflake {
    private:
        uint64_t id;
    public:
        constexpr Snowflake() noexcept : id(0) {}
        constexpr Snowflake(const uint64_t& snowflake) noexcept : id(snowflake) {}
        explicit Snowflake(const std::string& snowflake) noexcept : id(std::stoll(snowflake)) {}
        operator uint64_t() const { return id; }
        operator std::string() const { return std::to_string(id); }
    };
}

namespace std {
    template <>
    struct hash<discpp::Snowflake> {
        std::size_t operator()(const discpp::Snowflake& k) const {
            return hash<uint64_t>()(k);
        }
    };
}

#endif