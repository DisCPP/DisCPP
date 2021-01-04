#include "http_client.h"
#include "exceptions.h"
#include "client.h"
#include "log.h"
#include "client_config.h"

std::map<std::string, std::string, discpp::CaseInsensitiveLess> discpp::HttpClient::DefaultHeaders(std::map<std::string, std::string, CaseInsensitiveLess> add) {
    std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers = {
            { "User-Agent", "DiscordBot (https://github.com/seanomik/DisCPP, v0.0.0)" },
            { "X-RateLimit-Precision", "millisecond"}};

    // Add the correct authorization header depending on the token type.
    if (client->config.type == TokenType::USER) {
        headers.insert({ "Authorization", client->token });
    } else {
        headers.insert({ "Authorization", "Bot " + client->token });
    }

    for (auto head : add) {
        headers.insert(headers.end(), head);
    }

    return headers;
}

std::unique_ptr<rapidjson::Document> discpp::HttpClient::HandleResponse(std::string response_body, int status_code, const std::map<std::string, std::string, CaseInsensitiveLess> &headers,
        discpp::RateLimitBucketType ratelimit_bucket, discpp::Snowflake object) {

    if (client) {
        client->logger->Debug("Received requested payload: " + response_body);
    }

    auto tmp = std::make_unique<rapidjson::Document>();
    if (!response_body.empty() && response_body[0] == '[' && response_body[response_body.size() - 1] == ']') {
        tmp->SetArray();
    } else {
        tmp->SetObject();
    }

    // Handle http response codes and throw an exception if it failed.
    if (status_code != 200 && status_code != 201 && status_code != 204) {
        std::string response_msg;
        switch (status_code) {
            case 304:
                response_msg = "NOT MODIFIED";
                break;
            case 400:
                response_msg = "BAD REQUEST";
                break;
            case 401:
                response_msg = "UNAUTHORIZED";
                break;
            case 403:
                response_msg = "FORBIDDEN";
                break;
            case 404:
                response_msg = "NOT FOUND";
                break;
            case 405:
                response_msg = "METHOD NOT ALLOWED";
                break;
            case 249:
                response_msg = "TOO MANY REQUESTS";
                break;
            case 502:
                response_msg = "GATEWAY UNAVAILABLE";
                break;
            default:
                response_msg = "SERVER ERROR";
                break;
        }

        throw discpp::exceptions::http::HTTPResponseException(status_code, response_msg);
    }

    discpp::HandleRateLimits(headers, object, ratelimit_bucket);
    tmp->Parse((!response_body.empty() ? response_body.c_str() : "{}"));

    // Check if we were returned a json error and throw an exception if so.
    if (!tmp->IsNull() && tmp->IsObject() && ContainsNotNull(*tmp, "code")) {
        discpp::ThrowException(*tmp);
    }

    // This shows an error in inteliisense for some reason but compiles fine.
    return tmp;
}
