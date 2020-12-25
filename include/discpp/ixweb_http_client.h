//
// Created by sean_ on 12/21/2020.
//

#ifndef DISCPP_IXWEBHTTPCLIENT_H
#define DISCPP_IXWEBHTTPCLIENT_H

#include "http_client.h"

namespace discpp {
    class IXWebHttpClient : public discpp::HttpClient {
    public:
        IXWebHttpClient() = default;

        explicit IXWebHttpClient(discpp::Client* client) : discpp::HttpClient(client) {

        }

        std::unique_ptr<rapidjson::Document> SendGetRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {}) override;
        std::unique_ptr<rapidjson::Document> SendPostRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {}) override;
        std::unique_ptr<rapidjson::Document> SendPutRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {}) override;
        std::unique_ptr<rapidjson::Document> SendPatchRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {}) override;
        std::unique_ptr<rapidjson::Document> SendDeleteRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket) override;
    };
}

#endif //DISCPP_IXWEBHTTPCLIENT_H
