#ifndef DISCPP_HTTP_CLIENT_H
#define DISCPP_HTTP_CLIENT_H

#include <memory>
#include <string>
#include <map>

#include "snowflake.h"
#include "utils.h"

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>

namespace discpp {
    class Client;

    class HttpClient {
    protected:
        discpp::Client* client;
    public:
        HttpClient() {

        }

        explicit HttpClient(discpp::Client* client) : client(client) {

        }

        void SetClient(discpp::Client* client) {
            this->client = client;
        }

        /**
         * @brief Sends a get request to a url.
         *
         * @param[in] url The url to create a request to.
         * @param[in] headers The http header.
         * @param[in] object The object id to handle the ratelimits for.
         * @param[in] ratelimit_bucket The rate limit bucket.
         * @param[in] body The body of the request.
         *
         * @return std::unique_ptr<rapidjson::Document>
         */
        virtual std::unique_ptr<rapidjson::Document> SendGetRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, discpp::RateLimitBucketType ratelimit_bucket, std::string body = {}) = 0;

        /**
         * @brief Sends a post request to a url.
         *
         * @param[in] url The url to create a request to.
         * @param[in] headers The http header.
         * @param[in] object The object id to handle the ratelimits for.
         * @param[in] ratelimit_bucket The rate limit bucket.
         * @param[in] body The body of the request.
         *
         * @return std::unique_ptr<rapidjson::Document>
         */
        virtual std::unique_ptr<rapidjson::Document> SendPostRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, discpp::RateLimitBucketType ratelimit_bucket, std::string body = {}) = 0;

        /**
         * @brief Sends a put request to a url.
         *
         * @param[in] url The url to create a request to.
         * @param[in] headers The http header.
         * @param[in] object The object id to handle the ratelimits for.
         * @param[in] ratelimit_bucket The rate limit bucket.
         * @param[in] body The body of the request.
         *
         * @return std::unique_ptr<rapidjson::Document>
         */
        virtual std::unique_ptr<rapidjson::Document> SendPutRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, discpp::RateLimitBucketType ratelimit_bucket, std::string body = {}) = 0;

        /**
         * @brief Sends a patch request to a url.
         *
         * @param[in] url The url to create a request to.
         * @param[in] headers The http header.
         * @param[in] object The object id to handle the ratelimits for.
         * @param[in] ratelimit_bucket The rate limit bucket.
         * @param[in] body The body of the request.
         *
         * @return std::unique_ptr<rapidjson::Document>
         */
        virtual std::unique_ptr<rapidjson::Document> SendPatchRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, discpp::RateLimitBucketType ratelimit_bucket, std::string body = {}) = 0;

        /**
         * @brief Sends a delete request to a url.
         *
         * @param[in] url The url to create a request to.
         * @param[in] headers The http header.
         * @param[in] object The object id to handle the ratelimits for.
         * @param[in] ratelimit_bucket The rate limit bucket.
         *
         * @return std::unique_ptr<rapidjson::Document>
         */
        virtual std::unique_ptr<rapidjson::Document> SendDeleteRequest(std::string url, std::map<std::string, std::string, discpp::CaseInsensitiveLess> headers, discpp::Snowflake object, discpp::RateLimitBucketType ratelimit_bucket) = 0;

        /**
         * @brief Gets the default headers to communicate with discord.
         *
         * @param[in] add The headers to add to the default ones.
         *
         * @return std::map<std::string, std::string>
         */
        virtual std::map<std::string, std::string, discpp::CaseInsensitiveLess> DefaultHeaders(std::map<std::string, std::string, CaseInsensitiveLess> add = {});

        /**
         * @brief Handles a response from the discpp servers.
         *
         * @param[in] reponse The cpr response from the servers.
         * @param[in] object The object id to handle the ratelimits for.
         * @param[in] ratelimit_bucket The rate limit bucket.
         *
         * @return std::unique_ptr<rapidjson::Document>
         */
        virtual std::unique_ptr<rapidjson::Document> HandleResponse(std::string response_body, int status_code, const std::map<std::string, std::string, CaseInsensitiveLess> &headers, discpp::RateLimitBucketType ratelimit_bucket, discpp::Snowflake object);
    };
}

#endif //DISCPP_HTTP_CLIENT_H