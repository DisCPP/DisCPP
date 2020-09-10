#ifndef DISCPP_SNOWFLAKE_H
#define DISCPP_SNOWFLAKE_H

#include <string>

namespace discpp {
    class Snowflake {
    private:
        uint64_t id;
    public:
        Snowflake() : id(0) {}
        Snowflake(const uint64_t& snowflake) : id(snowflake) {}
        explicit Snowflake(const std::string& snowflake) : id(std::stoll(snowflake)) {}

        operator uint64_t() const { return id; }
        operator std::string() const { return std::to_string(id); }
    };
}

namespace std {
    template<>
    struct hash<discpp::Snowflake> {
        std::size_t operator()(const discpp::Snowflake& snowflake) const {
          return std::hash<uint64_t>()((uint64_t) snowflake);
        }
    };
}

#endif