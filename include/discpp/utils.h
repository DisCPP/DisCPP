#ifndef DISCPP_UTILS_H
#define DISCPP_UTILS_H

#define RAPIDJSON_HAS_STDSTRING 1

#include "discord_object.h"

#include <rapidjson/document.h>

#include <cpr/cpr.h>

#include <unordered_map>
#include <climits>
#include <ixwebsocket/IXHttp.h>

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

	inline std::string Endpoint(std::string endpoint_format) {
		endpoint_format = endpoint_format[0] == '/' ? endpoint_format : '/' + endpoint_format;
		return "https://discordapp.com/api/v6" + endpoint_format;
	}

	template <typename type>
	inline type GetFromVector(std::vector<type> vector, type item) {
		auto new_item = std::find_if(vector.begin(), vector.end(), [](type a, type b) { return a == b; });

		if (new_item != vector.end()) return new_item;
		return nullptr;
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

                return T(t_doc.GetString());
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

	void IterateThroughNotNullJson(rapidjson::Document& json, std::function<void(rapidjson::Document&)> func);
    bool ContainsNotNull(rapidjson::Document& json, char * value_name);
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

	inline int WaitForRateLimits(snowflake object, RateLimitBucketType ratelimit_bucket);
    inline void HandleRateLimits(ix::WebSocketHttpHeaders header, snowflake object, RateLimitBucketType ratelimit_bucket);
	// End of rate limits

    extern rapidjson::Document HandleResponse(ix::HttpResponsePtr response, snowflake object, RateLimitBucketType ratelimit_bucket);
	extern rapidjson::Document SendGetRequest(std::string url, ix::WebSocketHttpHeaders headers, snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {});
    extern rapidjson::Document SendPostRequest(std::string url, ix::WebSocketHttpHeaders headers, snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {});
    extern rapidjson::Document SendPutRequest(std::string url, ix::WebSocketHttpHeaders headers, snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {});
    extern rapidjson::Document SendPatchRequest(std::string url, ix::WebSocketHttpHeaders headers, snowflake object, RateLimitBucketType ratelimit_bucket, std::string body = {});
    extern rapidjson::Document SendDeleteRequest(std::string url, ix::WebSocketHttpHeaders headers, snowflake object, RateLimitBucketType ratelimit_bucket);

	ix::WebSocketHttpHeaders DefaultHeaders(ix::WebSocketHttpHeaders add = {});
	bool StartsWith(std::string string, std::string prefix);
	std::vector<std::string> SplitString(std::string str, std::string delimter);
	std::string CombineStringVector(std::vector<std::string> v, std::string delimiter = " ", int offset = 0);
	std::string ReadEntireFile(std::ifstream& file);
	std::string Base64Encode(std::string text);
	std::string ReplaceAll(std::string data, std::string to_search, std::string replace_str);
	std::string EscapeString(std::string string);
	time_t TimeFromSnowflake(snowflake snow);
	std::string FormatTimeFromSnowflake(snowflake snow);
}

#endif
