//
// Created by SeanOMik on 5/23/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#ifndef DISCORDCLIENT_EXCEPTIONS_H
#define DISCORDCLIENT_EXCEPTIONS_H

#include "utils.h"

#include <exception>

namespace discpp {
    class DiscordObjectNotFound : public std::runtime_error {
    public:
        DiscordObjectNotFound(rapidjson::Document& json) : std::runtime_error(std::to_string(json["code"].GetInt()) + ": " + json["message"].GetString()) {}
        DiscordObjectNotFound(const std::string& str) : std::runtime_error(str) {}
    };

    class StartLimitException : public std::runtime_error {
    public:
        StartLimitException() : std::runtime_error("Maximum start limit reached") {}
    };

    class ProhibitedEndpointException : public std::runtime_error {
    public:
        ProhibitedEndpointException(const std::string& msg) : std::runtime_error(msg) {}
    };

    class AuthenticationException : public std::runtime_error {
    public:
        AuthenticationException() : std::runtime_error("Invalid token, failed to connect to gateway") {}
    };

    inline std::runtime_error GetException(rapidjson::Document& json) {
        if (json["message"] == "Unknown User") {
            return DiscordObjectNotFound(json);
        }

        return std::runtime_error(std::to_string(json["code"].GetInt()) + ": " + json["message"].GetString());
    }
}

#endif //DISCORDCLIENT_EXCEPTIONS_H
