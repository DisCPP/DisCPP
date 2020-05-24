//
// Created by SeanOMik on 5/23/2020.
// Github: https://github.com/SeanOMik
// Email: seanomik@gmail.com
//

#ifndef DISCORDCLIENT_EXCEPTIONS_H
#define DISCORDCLIENT_EXCEPTIONS_H

#include <rapidjson/document.h>

#include "utils.h"

#include <exception>

namespace discpp {
    class DiscordObjectNotFound : public std::runtime_error {
    public:
        DiscordObjectNotFound(rapidjson::Document& json) : std::runtime_error(std::to_string(json["code"].GetInt()) + ": " + json["message"].GetString()) {

        }
    };

    inline std::runtime_error GetException(rapidjson::Document& json) {
        if (json["message"] == "Unknown User") {
            return DiscordObjectNotFound(json);
        }

        return std::runtime_error(std::to_string(json["code"].GetInt()) + ": " + json["message"].GetString());
    }
}

#endif //DISCORDCLIENT_EXCEPTIONS_H
