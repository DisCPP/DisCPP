//
// Created by sean_ on 12/21/2020.
//

#include "ixweb_http_client.h"
#include "client.h"
#include "log.h"
#include "client_config.h"
#include "exceptions.h"

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXHttpClient.h>

std::unique_ptr<rapidjson::Document> discpp::IXWebHttpClient::SendGetRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object,
        discpp::RateLimitBucketType ratelimit_bucket, std::string body) {

    if (client != nullptr) {
        client->logger->Debug("Sending get request, URL: " + url + ", body: " + body);
    }

    ix::initNetSystem();

    WaitForRateLimits(client, object, ratelimit_bucket);

    ix::HttpClient httpClient;
    ix::HttpRequestArgsPtr args = httpClient.createRequest();
    args->extraHeaders = ix::WebSocketHttpHeaders(headers.begin(), headers.end());
    args->body = body;
    ix::HttpResponsePtr result;
    result = httpClient.get(url, args);

    std::unique_ptr<rapidjson::Document> doc = HandleResponse(result->body, result->statusCode,
        std::map<std::string, std::string, discpp::CaseInsensitiveLess>(result->headers.begin(), result->headers.end()),
            ratelimit_bucket, object);

    return doc;
}

std::unique_ptr<rapidjson::Document> discpp::IXWebHttpClient::SendPostRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object,
        discpp::RateLimitBucketType ratelimit_bucket, std::string body) {

    if (client != nullptr) {
        client->logger->Debug("Sending post request, URL: " + url + ", body: " + body);
    }

    ix::initNetSystem();

    WaitForRateLimits(client, object, ratelimit_bucket);

    ix::HttpClient httpClient;
    ix::HttpRequestArgsPtr args = httpClient.createRequest();
    args->extraHeaders = ix::WebSocketHttpHeaders(headers.begin(), headers.end());
    ix::HttpResponsePtr result;
    result = httpClient.post(url, body, args);

    std::unique_ptr<rapidjson::Document> doc = HandleResponse(result->body, result->statusCode,
        std::map<std::string, std::string, discpp::CaseInsensitiveLess>(result->headers.begin(), result->headers.end()),
            ratelimit_bucket, object);

    return doc;
}

std::unique_ptr<rapidjson::Document> discpp::IXWebHttpClient::SendPutRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object,
        discpp::RateLimitBucketType ratelimit_bucket, std::string body) {

    if (client != nullptr) {
        client->logger->Debug("Sending get request, URL: " + url + ", body: " + body);
    }

    ix::initNetSystem();

    WaitForRateLimits(client, object, ratelimit_bucket);

    ix::HttpClient httpClient;
    ix::HttpRequestArgsPtr args = httpClient.createRequest();
    args->extraHeaders = ix::WebSocketHttpHeaders(headers.begin(), headers.end());
    ix::HttpResponsePtr result;
    result = httpClient.put(url, body, args);

    std::unique_ptr<rapidjson::Document> doc = HandleResponse(result->body, result->statusCode,
        std::map<std::string, std::string, discpp::CaseInsensitiveLess>(result->headers.begin(), result->headers.end()),
            ratelimit_bucket, object);

    return doc;
}

std::unique_ptr<rapidjson::Document> discpp::IXWebHttpClient::SendPatchRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object,
        discpp::RateLimitBucketType ratelimit_bucket, std::string body) {

    if (client != nullptr) {
        client->logger->Debug("Sending patch request, URL: " + url + ", body: " + body);
    }

    ix::initNetSystem();

    WaitForRateLimits(client, object, ratelimit_bucket);

    ix::HttpClient httpClient;
    ix::HttpRequestArgsPtr args = httpClient.createRequest();
    args->extraHeaders = ix::WebSocketHttpHeaders(headers.begin(), headers.end());
    ix::HttpResponsePtr result;
    result = httpClient.patch(url, body, args);

    std::unique_ptr<rapidjson::Document> doc = HandleResponse(result->body, result->statusCode,
        std::map<std::string, std::string, discpp::CaseInsensitiveLess>(result->headers.begin(), result->headers.end()),
            ratelimit_bucket, object);

    return doc;
}

std::unique_ptr<rapidjson::Document> discpp::IXWebHttpClient::SendDeleteRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object,
        discpp::RateLimitBucketType ratelimit_bucket) {

    if (client != nullptr) {
        client->logger->Debug("Sending delete request, URL: " + url);
    }

    ix::initNetSystem();

    WaitForRateLimits(client, object, ratelimit_bucket);

    ix::HttpClient httpClient;
    ix::HttpRequestArgsPtr args = httpClient.createRequest();
    args->extraHeaders = ix::WebSocketHttpHeaders(headers.begin(), headers.end());
    ix::HttpResponsePtr result;
    result = httpClient.del(url, args);

    std::unique_ptr<rapidjson::Document> doc = HandleResponse(result->body, result->statusCode,
        std::map<std::string, std::string, discpp::CaseInsensitiveLess>(result->headers.begin(), result->headers.end()),
        ratelimit_bucket, object);

    return doc;
}
