#ifndef DISCPP_UTILS_H
#define DISCPP_UTILS_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>

#include "discord_object.h"

#include <unordered_map>
#include <climits>

#include <ixwebsocket/IXHttp.h>

namespace discpp {
	class Client;
	class Role;

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

	enum ImageSize : int { x128 = 128, x256 = 256, x512 = 512, x1024 = 1024 };

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
		return "https://discordapp.com/api/v8" + tmp;
	}

	template <typename type>
	inline type GetFromVector(const std::vector<type>& vector, const type& item) {
		auto new_item = std::find_if(vector.begin(), vector.end(), [](type a, type b) { return a == b; });

		if (new_item != vector.end()) return new_item;
		return nullptr;
	}

	inline discpp::Snowflake GetIDSafely(rapidjson::Document& json, const char* value_name) {
        rapidjson::Value::ConstMemberIterator itr = json.FindMember(value_name);
        if (itr != json.MemberEnd()) {
            if (!json[value_name].IsNull()) {
                rapidjson::Document t_doc;
                t_doc.CopyFrom(json[value_name], t_doc.GetAllocator());

                return Snowflake(std::string(t_doc.GetString()));
            }
        }

        return 0;
	}

    template<typename T>
    inline T GetDataSafely(const rapidjson::Document & json, const char* value_name) {
        rapidjson::Value::ConstMemberIterator itr = json.FindMember(value_name);
        if (itr != json.MemberEnd()) {
            if (!json[value_name].IsNull()) {
                rapidjson::Document t_doc;
                t_doc.CopyFrom(json[value_name], t_doc.GetAllocator());

                return t_doc.Get<T>();
            }
        }

        return T();
    }

    template<class T>
    inline T ConstructDiscppObjectFromID(const rapidjson::Document& doc, const char* value_name, T default_val) {
        rapidjson::Value::ConstMemberIterator itr = doc.FindMember(value_name);
        if (itr != doc.MemberEnd()) {
            if (!doc[value_name].IsNull()) {
                rapidjson::Document t_doc;
                t_doc.CopyFrom(doc[value_name], t_doc.GetAllocator());

                return T(Snowflake(t_doc.GetString()));
            }
        }

        return default_val;
    }

    template<class T>
    inline T ConstructDiscppObjectFromJson(const rapidjson::Document& doc, const char* value_name, T default_val) {
        rapidjson::Value::ConstMemberIterator itr = doc.FindMember(value_name);
        if (itr != doc.MemberEnd()) {
            if (!doc[value_name].IsNull()) {
                rapidjson::Document t_doc;
                t_doc.CopyFrom(doc[value_name], t_doc.GetAllocator());

                return T(t_doc);
            }
        }

        return default_val;
    }

	template<class T>
	inline T ConstructDiscppObjectFromJson(discpp::Client* client, const rapidjson::Document& doc, const char* value_name, T default_val) {
        rapidjson::Value::ConstMemberIterator itr = doc.FindMember(value_name);
        if (itr != doc.MemberEnd()) {
            if (!doc[value_name].IsNull()) {
                rapidjson::Document t_doc;
                t_doc.CopyFrom(doc[value_name], t_doc.GetAllocator());

                return T(client, t_doc);
            }
        }

        return default_val;
	}

	void IterateThroughNotNullJson(rapidjson::Document& json, const std::function<void(rapidjson::Document&)>& func);
    bool ContainsNotNull(rapidjson::Document& json, const char * value_name);
    std::string DumpJson(rapidjson::Document& json);
    std::string DumpJson(rapidjson::Value& json);
    std::unique_ptr<rapidjson::Document> GetDocumentInsideJson(rapidjson::Document &json, const char* value_name);

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
     * @param[in] object The object id.
     * @param[in] ratelimit_bucket The rate limit bucket.
     *
     * @return int
     */
	int WaitForRateLimits(discpp::Client* client, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket);

    /**
     * @brief Handle rate limites
     *
     * @param[in] header The headers to get the wait rate limit from.
     * @param[in] object The object id.
     * @param[in] ratelimit_bucket The rate limit bucket.
     *
     * @return int
     */
	void HandleRateLimits(ix::WebSocketHttpHeaders headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket);
	// End of rate limits

    /**
     * @brief Handles a response from the discpp servers.
     *
     * @param[in] reponse The cpr response from the servers.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     *
     * @return rapidjson::Document
     */
	extern std::unique_ptr<rapidjson::Document> HandleResponse(discpp::Client* client, ix::HttpResponsePtr response, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket);

    /**
     * @brief Sends a get request to a url.
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     * @param[in] The cpr response body.
     *
     * @return rapidjson::Document
     */
    extern std::unique_ptr<rapidjson::Document> SendGetRequest(discpp::Client* client, std::string url, ix::WebSocketHttpHeaders headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {});

    /**
     * @brief Sends a post request to a url.
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     * @param[in] The cpr response body.
     *
     * @return rapidjson::Document
     */
    extern std::unique_ptr<rapidjson::Document> SendPostRequest(discpp::Client* client, std::string url, ix::WebSocketHttpHeaders headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {});

    /**
     * @brief Sends a put request to a url.
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     * @param[in] The cpr response body.
     *
     * @return rapidjson::Document
     */
    extern std::unique_ptr<rapidjson::Document> SendPutRequest(discpp::Client* client, std::string url, ix::WebSocketHttpHeaders headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {});

    /**
     * @brief Sends a patch request to a url.
     *
     * @param[in] url The url to create a request to.
     * @param[in] headers The http header.
     * @param[in] object The object id to handle the ratelimits for.
     * @param[in] ratelimit_bucket The rate limit bucket.
     * @param[in] The cpr response body.
     *
     * @return rapidjson::Document
     */
    extern std::unique_ptr<rapidjson::Document> SendPatchRequest(discpp::Client* client, std::string url, ix::WebSocketHttpHeaders headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {});

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
    extern std::unique_ptr<rapidjson::Document> SendDeleteRequest(discpp::Client* client, std::string url, ix::WebSocketHttpHeaders headers, discpp::Snowflake object, RateLimitBucketType ratelimit_bucket);

    /**
     * @brief Gets the default headers to communicate with the discpp servers.
     *
     * @param[in] add The headers to add to the default ones.
     *
     * @return ix::WebSocketHttpHeaders
     */
    ix::WebSocketHttpHeaders DefaultHeaders(discpp::Client* client, ix::WebSocketHttpHeaders add = {});

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
    std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter);

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
    std::string CombineStringVector(const std::vector<std::string>& vector, const std::string& delimiter = " ", const int& offset = 0);

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
     * @param[in] data The string to replace things inside of.
     * @param[in] to_search The string to search for and replace.
     * @param[in] replace_str The string we're gonna replace `to_search` with.
     *
     * @return std::string
     */
	std::string ReplaceAll(const std::string& data, const std::string& to_search, const std::string& replace_str);

    /**
     * @brief Replace all occurences of sub strings
     *
     * ```cpp
     *      std::string str = "discpp text";
     *      ReplaceAll(str, " ", "_");
     * ```
     *
     * @param[out] data The string to replace things inside of.
     * @param[in] to_search The string to search for and replace.
     * @param[in] replace_str The string we're gonna replace `to_search` with.
     *
     * @return void
     */
    void ReplaceAll(std::string& data, const std::string& to_search, const std::string& replace_str);

    /**
     * @brief Escape strings for discpp json endpoints and cpr body
     *
     * ```cpp
     *      std::string raw_text = "{\"content\":\"" + EscapeString(text) + (tts ? "\",\"tts\":\"true\"" : "\"") + "}";
     *		std::string body = cpr::Body(raw_text);
     *		std::unique_ptr<rapidjson::Document> result = SendPostRequest(client, Endpoint("/channels/%/messages", id), DefaultHeaders(client, { { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);
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
