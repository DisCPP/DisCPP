//
// Created by SeanOMik on 5/23/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#ifndef DISCORDCLIENT_EXCEPTIONS_H
#define DISCORDCLIENT_EXCEPTIONS_H

#include "utils.h"
#include "permission.h"

#include <exception>

namespace discpp {
    namespace exceptions {
        class DiscordObjectNotFound : public std::runtime_error {
        public:
            explicit DiscordObjectNotFound(rapidjson::Document &json) : std::runtime_error(
                std::to_string(json["code"].GetInt()) + ": " + json["message"].GetString()) {}

            explicit DiscordObjectNotFound(const std::string &str) : std::runtime_error(str) {}
        };

        class MaximumLimitException : public std::runtime_error {
        public:
            explicit MaximumLimitException(rapidjson::Document &json) : std::runtime_error(
                std::to_string(json["code"].GetInt()) + ": " + json["message"].GetString()) {}

            explicit MaximumLimitException(const std::string &str) : std::runtime_error(str) {}
        };

        class ProhibitedEndpointException : public std::runtime_error {
        public:
            explicit ProhibitedEndpointException(const std::string &msg) : std::runtime_error(msg) {}
        };

        class AuthenticationException : public std::runtime_error {
        public:
            AuthenticationException() : std::runtime_error("Invalid token, failed to connect to gateway") {}

            explicit AuthenticationException(const std::string &str) : std::runtime_error(str) {}
        };

        class RequestTooLargeException : public std::runtime_error {
        public:
            explicit RequestTooLargeException(const std::string &str) : std::runtime_error(str) {}
        };

        class FeatureDisabledException : public std::runtime_error {
        public:
            explicit FeatureDisabledException(const std::string &str) : std::runtime_error(str) {}
        };

        class BannedException : public std::runtime_error {
        public:
            explicit BannedException(const std::string &str) : std::runtime_error(str) {}
        };

        class MissingAccessException : public std::runtime_error {
        public:
            explicit MissingAccessException(const std::string &str) : std::runtime_error(str) {}
        };

        class InvalidAccountTypeException : public std::runtime_error {
        public:
            explicit InvalidAccountTypeException(const std::string &str) : std::runtime_error(str) {}
        };

        class GuildWidgetDisabledException : public std::runtime_error {
        public:
            explicit GuildWidgetDisabledException(const std::string &str) : std::runtime_error(str) {}
        };

        class OAuth2Exception : public std::runtime_error {
        public:
            explicit OAuth2Exception(const std::string &str) : std::runtime_error(str) {}
        };

        class EndpointParameterException : public std::runtime_error {
        public:
            explicit EndpointParameterException(const std::string &str) : std::runtime_error(str) {}
        };

        class APIOverloadedException : public std::runtime_error {
        public:
            explicit APIOverloadedException(const std::string &str) : std::runtime_error(str) {}
        };

        class InvalidAPIVersionException : public std::runtime_error {
        public:
            explicit InvalidAPIVersionException(const std::string &str) : std::runtime_error(str) {}
        };

        class RatelimitTooLong : public std::runtime_error {
        public:
            explicit RatelimitTooLong(const std::string &str, int milli_sec_ratelimit) : std::runtime_error(str), milli_sec_ratelimit(milli_sec_ratelimit) {}

            int milli_sec_ratelimit;
        };

        namespace http {
            class HTTPResponseException : public std::runtime_error {
            public:
                explicit HTTPResponseException(const std::int32_t &response_code, const std::string &str) :
                    std::runtime_error(str), response_code(response_code) {}

                std::int32_t response_code;
            };
        }
    }

    inline void ThrowException(rapidjson::Document& json) {
        switch (json["code"].GetInt()) {
            case 10001:
            case 10002:
            case 10003:
            case 10004:
            case 10005:
            case 10006:
            case 10007:
            case 10008:
            case 10009:
            case 10010:
            case 10011:
            case 10012:
            case 10013:
            case 10014:
            case 10015:
            case 10026:
            case 10027:
            case 10028:
            case 10029:
            case 10030:
            case 10031:
            case 10032:
            case 10036:
                throw exceptions::DiscordObjectNotFound(json["message"].GetString());
            case 20001:
            case 20002:
            case 500003: // Cannot execute action on a DM channel.
                throw exceptions::ProhibitedEndpointException(json["message"].GetString());
            case 30001:
            case 30002:
            case 30003:
            case 30005:
            case 30007:
            case 30010:
            case 30013:
            case 30015:
            case 30016:
                throw exceptions::MaximumLimitException(json["message"].GetString());
            case 400001:
            case 400002:
                throw exceptions::AuthenticationException(json["message"].GetString());
            case 400005:
                throw exceptions::RequestTooLargeException(json["message"].GetString());
            case 400006:
                throw exceptions::FeatureDisabledException(json["message"].GetString());
            case 400007:
                throw exceptions::BannedException(json["message"].GetString());
            case 50001:
                throw exceptions::MissingAccessException(json["message"].GetString());
            case 50003:
                throw exceptions::InvalidAccountTypeException(json["message"].GetString());
            case 50004:
                throw exceptions::GuildWidgetDisabledException(json["message"].GetString());
            case 50005:
            case 50006:
            case 50007:
            case 50008:
            case 50009:
                throw exceptions::MissingAccessException(json["message"].GetString());
            case 50010:
            case 50011:
            case 50012:
            case 50025:
                throw exceptions::OAuth2Exception(json["message"].GetString());
            case 50013:
                throw NoPermissionException(json["message"].GetString());
            case 50014:
                throw exceptions::AuthenticationException(json["message"].GetString());
            case 50015:
            case 50016:
            case 50019:
            case 50020:
            case 50021:
            case 50034:
            case 50035:
            case 50036:
            case 90001:
                throw exceptions::EndpointParameterException(json["message"].GetString());
            case 50041:
                throw exceptions::InvalidAPIVersionException(json["message"].GetString());
            case 130000:
                throw exceptions::APIOverloadedException(json["message"].GetString());
        }

        throw std::runtime_error(std::to_string(json["code"].GetInt()) + ": " + json["message"].GetString());
    }
}

#endif //DISCORDCLIENT_EXCEPTIONS_H
