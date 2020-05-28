#ifndef DISCPP_UTILS_H
#define DISCPP_UTILS_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include "discord_object.h"

#include <rapidjson/document.h>

#include <cpr/cpr.h>

#include <unordered_map>
#include <climits>

namespace discpp {
	class Client;
	class Role;

	namespace globals {
		inline discpp::Client* client_instance;
	}

	namespace specials {
		enum class NitroSubscription : int {
			NO_NITRO = 0,
			NITRO_CLASSIC = 1,
			NITRO = 2
		};

		enum class NitroTier : int {
			NO_TIER = 0,
			TIER_1 = 1,
			TIER_2 = 2,
			TIER_3 = 3
		};

		enum VerificationLevel : int {
			NO_VERIFICATION = 0,
			LOW = 1,
			MEDIUM = 2,
			HIGH = 3,
			VERY_HIGH = 4
		};

		enum DefaultMessageNotificationLevel : int {
			ALL_MESSAGES = 0,
			ONLY_MENTIONS = 1
		};

		enum class ExplicitContentFilterLevel : int {
			DISABLED = 0,
			MEMBERS_WITHOUT_ROLES = 1,
			ALL_MEMBERS = 2
		};

		enum class MFALevel : int {
			NO_MFA = 0,
			ELEVATED = 1
		};
	}

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

    discpp::snowflake SnowflakeFromString(const std::string& str);

	inline discpp::snowflake GetIDSafely(rapidjson::Document& json, const char* value_name) {
        rapidjson::Value::ConstMemberIterator itr = json.FindMember(value_name);
        if (itr != json.MemberEnd()) {
            if (!json[value_name].IsNull()) {
                rapidjson::Document t_doc;
                t_doc.CopyFrom(json[value_name], t_doc.GetAllocator());

                return SnowflakeFromString(std::string(t_doc.GetString()));
            }
        }

        return 0;
	}

    template<typename T>
    inline T GetDataSafely(rapidjson::Document & json, const char* value_name) {
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
    inline T ConstructDiscppObjectFromID(rapidjson::Document& doc, const char* value_name, T default_val) {
        rapidjson::Value::ConstMemberIterator itr = doc.FindMember(value_name);
        if (itr != doc.MemberEnd()) {
            if (!doc[value_name].IsNull()) {
                rapidjson::Document t_doc;
                t_doc.CopyFrom(doc[value_name], t_doc.GetAllocator());

                return T(SnowflakeFromString(t_doc.GetString()));
            }
        }

        return default_val;
    }

	template<class T>
	inline T ConstructDiscppObjectFromJson(rapidjson::Document& doc, const char* value_name, T default_val) {
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

	void IterateThroughNotNullJson(rapidjson::Document& json, const std::function<void(rapidjson::Document&)>& func);
    bool ContainsNotNull(rapidjson::Document& json, const char * value_name);
    std::string DumpJson(rapidjson::Document& json);
    std::string DumpJson(rapidjson::Value& json);
    rapidjson::Document GetDocumentInsideJson(rapidjson::Document &json, const char* value_name);

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

	inline std::unordered_map<snowflake, RateLimit> guild_ratelimit;
	inline std::unordered_map<snowflake, RateLimit> channel_ratelimit;
	inline std::unordered_map<snowflake, RateLimit> webhook_ratelimit;
	inline RateLimit global_ratelimit;

	int WaitForRateLimits(const snowflake& object, const RateLimitBucketType& ratelimit_bucket);
	void HandleRateLimits(cpr::Header& header, const snowflake& object, const RateLimitBucketType& ratelimit_bucket);
	// End of rate limits

	extern rapidjson::Document HandleResponse(cpr::Response& response, const snowflake& object, const RateLimitBucketType& ratelimit_bucket);
	extern rapidjson::Document SendGetRequest(const std::string& url, const cpr::Header& headers, const snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body = {});
	extern rapidjson::Document SendPostRequest(const std::string& url, const cpr::Header& headers, const snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body = {});
	extern rapidjson::Document SendPutRequest(const std::string& url, const cpr::Header& headers, const snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body = {});
	extern rapidjson::Document SendPatchRequest(const std::string& url, const cpr::Header& headers, const snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body = {});
	extern rapidjson::Document SendDeleteRequest(const std::string& url, const cpr::Header& headers, const snowflake& object, const RateLimitBucketType& ratelimit_bucket);

	cpr::Header DefaultHeaders(const cpr::Header& add = {});
	bool StartsWith(const std::string& string, const std::string& prefix);
	std::vector<std::string> SplitString(const std::string& str, const std::string& delimter);
	std::string CombineStringVector(const std::vector<std::string>& v, const std::string& delimiter = " ", const int& offset = 0);
	std::string ReadEntireFile(std::ifstream& file);
	std::string Base64Encode(const std::string& text);
	std::string ReplaceAll(const std::string& data, const std::string& to_search, const std::string& replace_str);
	std::string EscapeString(const std::string& string);
	time_t TimeFromSnowflake(const snowflake& snow);
	std::string FormatTimeFromSnowflake(const snowflake& snow);
	std::string URIEncode(const std::string& str);
}

#endif
