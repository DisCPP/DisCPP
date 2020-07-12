#ifndef DISCPP_UTILS_H
#define DISCPP_UTILS_H

#include "discord_object.h"

#include <cpr/cpr.h>

#include <unordered_map>
#include <climits>

namespace discpp {
	class Client;
	class Role;
	class JsonObject;

	namespace globals {
		inline discpp::Client* client_instance;
	}

	namespace specials {
		enum class NitroSubscription : uint8_t {
			NO_NITRO = 0,
			NITRO_CLASSIC = 1,
			NITRO = 2
		};

		enum class NitroTier : uint8_t {
			NO_TIER = 0,
			TIER_1 = 1,
			TIER_2 = 2,
			TIER_3 = 3
		};

		enum VerificationLevel : uint8_t {
			NO_VERIFICATION = 0,
			LOW = 1,
			MEDIUM = 2,
			HIGH = 3,
			VERY_HIGH = 4
		};

		enum DefaultMessageNotificationLevel : uint8_t {
			ALL_MESSAGES = 0,
			ONLY_MENTIONS = 1
		};

		enum class ExplicitContentFilterLevel : uint8_t {
			DISABLED = 0,
			MEMBERS_WITHOUT_ROLES = 1,
			ALL_MEMBERS = 2
		};

		enum class MFALevel : uint8_t {
			NO_MFA = 0,
			ELEVATED = 1
		};
	}

    enum ImageType : int { AUTO, WEBP, PNG, JPEG, GIF };

    enum ChannelType : int {
        GUILD_TEXT, DM, GUILD_VOICE, GROUP_DM,
        GROUP_CATEGORY, GROUP_NEWS, GROUP_STORE
    };

    /**
     * @brief Get the OS name this application is running on.
     *
     * ```cpp
     *      std::string os_name = discpp::GetOSName();
     * ```
     *
     * @return std::string
     */
	std::string GetOsName();

	inline std::string Endpoint(const std::string& endpoint_format) {
		std::string tmp = endpoint_format[0] == '/' ? endpoint_format : '/' + endpoint_format;
		return "https://discordapp.com/api/v6" + tmp;
	}

	template <typename type>
	inline type GetFromVector(const std::vector<type>& vector, const type& item) {
		auto new_item = std::find_if(vector.begin(), vector.end(), [](type a, type b) { return a == b; });

		if (new_item != vector.end()) return new_item;
		return nullptr;
	}

    discpp::Snowflake SnowflakeFromString(const std::string& str);

	// Rate limits
	struct RateLimit {
		int limit = 500;
		int remaining_limit = 500;
		double ratelimit_reset = 0;
	};

	enum RateLimitBucketType : int {
		CHANNEL,
		GUILD,
		WEBHOOK,
		GLOBAL
	};

	inline std::unordered_map<Snowflake, RateLimit> guild_ratelimit;
	inline std::unordered_map<Snowflake, RateLimit> channel_ratelimit;
	inline std::unordered_map<Snowflake, RateLimit> webhook_ratelimit;
	inline RateLimit global_ratelimit;

    /**
     * @brief Wait for rate limits.
     *
     * ```cpp
     *      discpp::WaitForRateLimits(message.id, discpp::RateLimitBucketType::CHANNEL);
     * ```
     *
     * @param[in] object The object id.
     * @param[in] ratelimit_bucket The rate limit bucket.
     *
     * @return int
     */
	int WaitForRateLimits(const Snowflake& object, const RateLimitBucketType& ratelimit_bucket);

    /**
     * @brief Handle rate limites
     *
     * ```cpp
     *      discpp::HandleRateLimits(header, id, bucket);
     * ```
     *
     * @param[in] header The headers to get the wait rate limit from.
     * @param[in] object The object id.
     * @param[in] ratelimit_bucket The rate limit bucket.
     *
     * @return int
     */
	void HandleRateLimits(cpr::Header& header, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket);
	// End of rate limits

    /**
     * @brief Handles a response from the discpp servers.
     *
     * ```cpp
     *      discpp::JsonObject response = discpp::HandleResponse(cpr_response, object, discpp::RateLimitBucketType::CHANNEL);
     * ```
     *
     * @param[in] reponse The cpr response from the servers.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     *
     * @return discpp::JsonObject
     */
	extern std::unique_ptr<discpp::JsonObject> HandleResponse(cpr::Response& response, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket);

    /**
     * @brief Sends a get request to a url.
     *
     * ```cpp
     *      discpp::JsonObject response = discpp::SendGetRequest(url, discpp::DefaultHeaders(), object, discpp::RateLimitBucketType::CHANNEL, {});
     * ```
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     * @param[in] The cpr response body.
     *
     * @return discpp::JsonObject
     */
	extern std::unique_ptr<discpp::JsonObject> SendGetRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body = {});

    /**
     * @brief Sends a post request to a url.
     *
     * ```cpp
     *      discpp::JsonObject response = discpp::SendPostRequest(url, discpp::DefaultHeaders(), object, discpp::RateLimitBucketType::CHANNEL, {});
     * ```
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     * @param[in] The cpr response body.
     *
     * @return discpp::JsonObject
     */
	extern std::unique_ptr<discpp::JsonObject> SendPostRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body = {});

    /**
     * @brief Sends a put request to a url.
     *
     * ```cpp
     *      discpp::JsonObject response = discpp::SendPutRequest(url, discpp::DefaultHeaders(), object, discpp::RateLimitBucketType::CHANNEL, {});
     * ```
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     * @param[in] The cpr response body.
     *
     * @return discpp::JsonObject
     */
	extern std::unique_ptr<discpp::JsonObject> SendPutRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body = {});

    /**
     * @brief Sends a patch request to a url.
     *
     * ```cpp
     *      discpp::JsonObject response = discpp::SendPatchRequest(url, discpp::DefaultHeaders(), object, discpp::RateLimitBucketType::CHANNEL, {});
     * ```
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     * @param[in] The cpr response body.
     *
     * @return discpp::JsonObject
     */
	extern std::unique_ptr<discpp::JsonObject> SendPatchRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body = {});

    /**
     * @brief Sends a delete request to a url.
     *
     * ```cpp
     *      discpp::JsonObject response = discpp::SendDeleteRequest(url, discpp::DefaultHeaders(), object, discpp::RateLimitBucketType::CHANNEL);
     * ```
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     *
     * @return discpp::JsonObject
     */
	extern std::unique_ptr<discpp::JsonObject> SendDeleteRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket);

    /**
     * @brief Gets the default headers to communicate with the discpp servers.
     *
     * ```cpp
     *      cpr::Header default_headers = discpp::DefaultHeaders({});
     * ```
     *
     * @param[in] add The headers to add to the default ones.
     *
     * @return discpp::JsonObject
     */
    cpr::Header DefaultHeaders(const cpr::Header& add = {});

    /**
     * @brief Check if a string starts with another string.
     *
     * ```cpp
     *      bool does_start_with = discpp::StartsWith("$test_command", "$");
     * ```
     *
     * @param[in] string The string to check if it starts with.
     * @param[in] prefix The prefix to check if the string starts with it.
     *
     * @return bool
     */
	bool StartsWith(const std::string& string, const std::string& prefix);

    /**
     * @brief Split a string into a vector.
     *
     * ```cpp
     *      std::vector<std::string> argument_vec = discpp::SplitString(message.content, " ");
     * ```
     *
     * @param[in] str The string to split
     * @param[in] delimiter The delimiter to split the string at.
     *
     * @return std::vector<std::string>
     */
	std::vector<std::string> SplitString(const std::string& str, const std::string& delimter);

    /**
     * @brief Combine a vector into a string with spaces between each element.
     *
     * ```cpp
     *      std::string combined = discpp::CombineVectorWithSpaces(arguments, 1);
     * ```
     *
     * @param[in] vector The vector to combine.
     * @param[in] delmiter The delimiter to combine the vector by.
     * @param[in] offset The vector element offset to start at (if you wanted to skip the first 2 elements, set this offset to "2").
     *
     * @return std::string
     */
    std::string CombineStringVector(const std::vector<std::string>& v, const std::string& delimiter = " ", const int& offset = 0);

    /**
     * @brief Reads an entire file.
     *
     * ```cpp
     *      std::string file_contents = discpp::ReadEntireFile(file);
     * ```
     *
     * @param[in] file The file to read.
     *
     * @return std::string
     */
	std::string ReadEntireFile(std::ifstream& file);

    /**
     * @brief Encode Base64.
     *
     * ```cpp
     *		std::string encodedStuff = Base64Encode("text");
     * ```
     *
     * @param[in] text The text to encode.
     *
     * @return std::string
     */
	std::string Base64Encode(const std::string& text);

    /**
     * @brief Replace all occurences of sub strings
     *
     * ```cpp
     *      std::string raw_text = ReplaceAll("discpp text", " ", "_");
     * ```
     *
     * @param[in] string The string to escape.
     *
     * @return std::string
     */
	std::string ReplaceAll(const std::string& data, const std::string& to_search, const std::string& replace_str);

    /**
     * @brief Escape strings for discpp json endpoints and cpr body
     *
     * ```cpp
     *      std::string raw_text = "{\"content\":\"" + EscapeString(text) + (tts ? "\",\"tts\":\"true\"" : "\"") + "}";
     *		cpr::Body body = cpr::Body(raw_text);
     *		std::unique_ptr<discpp::JsonObject> result = SendPostRequest(Endpoint("/channels/%/messages", id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
     * ```
     *
     * @param[in] string The string to escape.
     *
     * @return std::string
     */
	std::string EscapeString(const std::string& string);

	time_t TimeFromDiscord(const std::string& time);
	time_t TimeFromSnowflake(const Snowflake& snow);
    std::string FormatTime(const time_t& time, const std::string& format = "%F @ %r %Z");
	std::string URIEncode(const std::string& str);
    void SplitAvatarHash(const std::string& hash, uint64_t out[2]);
    std::string CombineAvatarHash(const uint64_t in[2]);
}

#endif
