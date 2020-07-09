#include "utils.h"
#include "client.h"
#include "client_config.h"
#include "exceptions.h"

#include <stdlib.h>
#include <numeric>
#include <iomanip>

#include <rapidjson/writer.h>

std::string discpp::GetOsName() {
	#ifdef _WIN32
		return "Windows 32-bit";
	#elif _WIN64
		return "Windows 64-bit";
        #elif __APPLE__ || __MACH__
                return "Mac OSX";
        #elif __linux__
            return "Linux";
        #elif __FreeBSD__
            return "FreeBSD";
        #elif __unix || __unix__
            return "Unix";
        #else
            return "Other";
	#endif
}

// @TODO: Test if the json document type returned is what its supposed to be, like an array or object.
std::unique_ptr<rapidjson::Document> discpp::HandleResponse(cpr::Response& response, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket) {
    if (globals::client_instance != nullptr) {
        globals::client_instance->logger->Debug("Received requested payload: " + response.text);
    }

    auto tmp = std::make_unique<rapidjson::Document>();
    if (!response.text.empty() && response.text[0] == '[' && response.text[response.text.size() - 1] == ']') {
        tmp->SetArray();
    } else {
        tmp->SetObject();
    }

    if (response.error) {
        globals::client_instance->logger->Error(LogTextColor::RED + "Hit error: " + response.error.message + " (Code: " + std::to_string((int)response.error.code) + ")");
    }

    // Handle http response codes and throw an exception if it failed.
    if (response.status_code != 200 && response.status_code != 201 && response.status_code != 204) {
        std::string response_msg;
        switch (response.status_code) {
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

        throw exceptions::http::HTTPResponseException(response.status_code, response_msg);
    }

	HandleRateLimits(response.header, object, ratelimit_bucket);
	tmp->Parse((!response.text.empty() ? response.text.c_str() : "{}"));

	// Check if we were returned a json error and throw an exception if so.
	if (!tmp->IsNull() && tmp->IsObject() && ContainsNotNull(*tmp, "code")) {
        discpp::ThrowException(*tmp);
    }

    // This shows an error in inteliisense for some reason but compiles fine.
	return tmp;
}

std::string CprBodyToString(const cpr::Body& body) {
	if (body.str().empty()) {
		return "Empty";
	}

	return body;
}

std::unique_ptr<rapidjson::Document> discpp::SendGetRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body) {
    if (globals::client_instance != nullptr) {
        globals::client_instance->logger->Debug("Sending get request, URL: " + url + ", body: " + CprBodyToString(body));
    }
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Get(cpr::Url{ url }, headers, body);

    return HandleResponse(result, object, ratelimit_bucket);
}

std::unique_ptr<rapidjson::Document> discpp::SendPostRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body) {
    if (globals::client_instance != nullptr) {
        globals::client_instance->logger->Debug("Sending post request, URL: " + url + ", body: " + CprBodyToString(body));
    }
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Post(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

std::unique_ptr<rapidjson::Document> discpp::SendPutRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body) {
    if (globals::client_instance != nullptr) {
        globals::client_instance->logger->Debug("put patch request, URL: " + url + ", body: " + CprBodyToString(body));
    }
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Put(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

std::unique_ptr<rapidjson::Document> discpp::SendPatchRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket, const cpr::Body& body) {
	if (globals::client_instance != nullptr) {
        globals::client_instance->logger->Debug("Sending patch request, URL: " + url + ", body: " + CprBodyToString(body));
    }
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Patch(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

std::unique_ptr<rapidjson::Document> discpp::SendDeleteRequest(const std::string& url, const cpr::Header& headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket) {
    if (globals::client_instance != nullptr) {
        globals::client_instance->logger->Debug("Sending delete request, URL: " + url);
    }
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Delete(cpr::Url{ url }, headers);
	return HandleResponse(result, object, ratelimit_bucket);
}

cpr::Header discpp::DefaultHeaders(const cpr::Header& add) {
    cpr::Header headers = { { "User-Agent", "DiscordBot (https://github.com/seanomik/DisCPP, v0.0.0)" },
                            { "X-RateLimit-Precision", "millisecond"} };
    // Add the correct authorization header depending on the token type.
	if (globals::client_instance->config->type == TokenType::USER) {
	    headers.insert({ "Authorization", discpp::globals::client_instance->token });
	} else {
        headers.insert({ "Authorization", "Bot " + discpp::globals::client_instance->token });
	}

	for (auto head : add) {
		headers.insert(headers.end(), head);
	}

	return headers;
}

bool discpp::StartsWith(const std::string& string, const std::string& prefix) {
	return string.substr(0, prefix.size()) == prefix;
}

std::vector<std::string> discpp::SplitString(const std::string& str, const std::string& delimiter) {
    size_t pos = 0;
    std::vector<std::string> tokens;
    std::string token, tmp = str;
    while ((pos = tmp.find(delimiter)) != std::string::npos) {
        token = tmp.substr(0, pos);

        // If the string is not empty then add it to the vector.
        if (!token.empty()) {
            tokens.push_back(token);
        }

        tmp.erase(0, pos + delimiter.length());
    }

    // Push back the last token from the string.
    size_t last_token = tmp.find_last_of(delimiter);
    tokens.push_back(tmp.substr(last_token + 1));

    // If the vector is empty, then just return a vector filled with the given string.
    if (tokens.empty()) {
        return { tmp };
    }

	return tokens;
}

std::string discpp::CombineStringVector(const std::vector<std::string>& vector, const std::string& delimiter, const int& offset) {
	if (vector.size() == 0) return "";

	return std::accumulate(vector.begin() + offset, vector.end(), std::string(""), [delimiter](std::string s0, std::string const& s1) { return s0 += delimiter + s1; }).substr(1);
}

std::string discpp::ReadEntireFile(std::ifstream& file) {
	return std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
}

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

std::string discpp::Base64Encode(const std::string& text) {
    unsigned char* buf = (unsigned char *) text.c_str();
    size_t buf_len = text.size();
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (buf_len--) {
        char_array_3[i++] = *(buf++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++) {
            ret += base64_chars[char_array_4[j]];
        }

        while ((i++ < 3)) {
            ret += '=';
        }
    }

    return ret;
}

std::string discpp::ReplaceAll(const std::string& data, const std::string& to_search, const std::string& replace_str) {
	std::string tmp = data;
    // Get the first occurrence
    size_t pos = tmp.find(to_search);

    // Repeat till end is reached
    while(pos != std::string::npos) {
        // Replace this occurrence of Sub String
        tmp.replace(pos, to_search.size(), replace_str);
        // Get the next occurrence from the current position
        pos = tmp.find(to_search, pos + replace_str.size());
    }

	return tmp;
}

std::string discpp::EscapeString(const std::string& string) {
    std::string tmp = string;
	tmp = ReplaceAll(string, "\\", "\\\\");
	tmp = ReplaceAll(string, "\"", "\\\"");
	tmp = ReplaceAll(string, "\a", "\\a");
	tmp = ReplaceAll(string, "\b", "\\b");
	tmp = ReplaceAll(string, "\f", "\\f");
	tmp = ReplaceAll(string, "\r", "\\r");
	tmp = ReplaceAll(string, "\t", "\\t");
	// \u + four-hex-digits

	return tmp;
}

int discpp::WaitForRateLimits(const Snowflake& object, const RateLimitBucketType& ratelimit_bucket) {

	RateLimit* rlmt = nullptr;

	if (global_ratelimit.remaining_limit == 0) {
		rlmt = &global_ratelimit;
	}
	else {
		switch (ratelimit_bucket) {
		case RateLimitBucketType::CHANNEL:
			rlmt = &channel_ratelimit[object];
			break;
		case RateLimitBucketType::GUILD:
			rlmt = &guild_ratelimit[object];
			break;
		case RateLimitBucketType::WEBHOOK:
			rlmt = &webhook_ratelimit[object];
			break;
		case RateLimitBucketType::GLOBAL:
			rlmt = &global_ratelimit;
			break;
		default:
			globals::client_instance->logger->Error(LogTextColor::RED + "RateLimitBucketType is invalid!");
			throw std::runtime_error("RateLimitBucketType is invalid!");
			break;
		}
	}

	if (rlmt->remaining_limit == 0) {
		double milisecond_time = rlmt->ratelimit_reset * 1000 - time(NULL) * 1000;

		if (milisecond_time > 0) {
			globals::client_instance->logger->Debug("Rate limit wait time: " + std::to_string(milisecond_time) + " milliseconds");
			std::this_thread::sleep_for(std::chrono::milliseconds((int)milisecond_time));
		}
	}
	return 0;
}

bool HeaderContains(const cpr::Header& header, const std::string& key) {
	/**
	 * @brief Check if a cpr::Header contains a specific key.
	 *
	 * ```cpp
	 *      bool contains = discpp::HeaderContains(headers, "auth");
	 * ```
	 *
	 * @param[in] header The headers to see if the key is contained in.
	 * @param[in] key The key to check if the headers contain.
	 *
	 * @return bool
	 */

	for (auto head : header) {
		if (head.first == key) return true;
	}
	return false;
}

void discpp::HandleRateLimits(cpr::Header& header, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket) {
	RateLimit* obj = nullptr;
	if (HeaderContains(header, "x-ratelimit-global")) {
		obj = &global_ratelimit;
	} else if (HeaderContains(header, "x-ratelimit-limit")) {
		switch (ratelimit_bucket) {
		    case RateLimitBucketType::CHANNEL:
		        obj = &channel_ratelimit[object];
		        break;
		    case RateLimitBucketType::GUILD:
		        obj = &guild_ratelimit[object];
		        break;
		    case RateLimitBucketType::WEBHOOK:
		        obj = &webhook_ratelimit[object];
		        break;
		    case RateLimitBucketType::GLOBAL:
		        obj = &global_ratelimit;
		        break;
		    default:
		        throw std::runtime_error("Invalid RateLimitBucketType!");
		        break;
		}
	} else {
		return;
	}

	obj->limit = std::stoi(header["x-ratelimit-limit"]);
	obj->remaining_limit = std::stoi(header["x-ratelimit-remaining"]);
	obj->ratelimit_reset = std::stod(header["x-ratelimit-reset"]);
}

time_t discpp::TimeFromDiscord(const std::string &time) {
    int year, month, day, hour, minute;
    int timezone_hr = 0, timezone_min = 0;
    float second;
    if (6 < sscanf(time.c_str(), "%d-%d-%dT%d:%d:%f%d:%d", &year, &month, &day, &hour, &minute, &second, &timezone_hr, &timezone_min)) {
        if (timezone_hr < 0) {
            timezone_min = -timezone_min;
        }

        hour += timezone_hr;
        minute += timezone_hr;

        struct tm t{};
        t.tm_year = year - 1900;
        t.tm_mon = month - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = minute;
        t.tm_sec = (int) second;

        time_t utc_time = mktime(&t);
        struct tm utc_buf;

#ifndef __linux__
        localtime_s(&utc_buf, &utc_time);
#else
        utc_buf = *localtime(&utc_time);
#endif

        discpp::globals::client_instance->logger->Debug("Parsed time: " + FormatTime(utc_time));

        return utc_time;
    }

    throw std::runtime_error("Failed to parse time");
}

time_t discpp::TimeFromSnowflake(const Snowflake& snow) {
    constexpr static uint64_t discord_epoch = 1420070400000;
    return ((snow >> 22) + discord_epoch) / 1000;
}

std::string discpp::FormatTime(const time_t& time, const std::string& format) {
    struct tm now{};
#ifndef __linux__
    localtime_s(&now, &time);
#else
    now = *localtime(&time);
#endif

    char buffer[256];
    strftime(buffer, sizeof(buffer), format.c_str(), &now);

    return buffer;
}

bool discpp::ContainsNotNull(rapidjson::Document &json, const char *value_name) {
    rapidjson::Value::ConstMemberIterator itr = json.FindMember(value_name);
    if (itr != json.MemberEnd()) {
        return !json[value_name].IsNull();
    }

    return false;
}

void discpp::IterateThroughNotNullJson(rapidjson::Document &json, const std::function<void(rapidjson::Document&)>& func) {
    for (auto const& object : json.GetArray()) {
        if (!object.IsNull()) {
            rapidjson::Document object_json;
            object_json.CopyFrom(object, object_json.GetAllocator());

            func(object_json);
        }
    }
}

std::unique_ptr<rapidjson::Document> discpp::GetDocumentInsideJson(rapidjson::Document &json, const char* value_name) {
    auto inside_json = std::make_unique<rapidjson::Document>(json.GetType());
    inside_json->CopyFrom(json[value_name], inside_json->GetAllocator());

	return inside_json;
}

std::string discpp::DumpJson(rapidjson::Document &json) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
	std::string tmp = buffer.GetString();

    return tmp;
}

std::string discpp::DumpJson(rapidjson::Value &json) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    std::string tmp = buffer.GetString();

    return tmp;
}

// Safe characters for URIEncode
char SAFE[256] = {
        /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
        /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

        /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
        /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
        /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
        /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

        /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

        /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

std::string discpp::URIEncode(const std::string& str) {
    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    const unsigned char * pSrc = (const unsigned char *) str.c_str();
    const size_t SRC_LEN = str.length();
    unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char * pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc) {
        if (SAFE[*pSrc]) {
            *pEnd++ = *pSrc;
        } else {
            // escape this char
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
    }

    std::string sResult((char *)pStart, (char *)pEnd);
    delete [] pStart;
    return sResult;
}

discpp::Snowflake discpp::SnowflakeFromString(const std::string& str) {
    return std::stoll(str, nullptr, 10);
}

void discpp::SplitAvatarHash(const std::string &hash, uint64_t out[2]) {
    out[0] = std::stoull(hash.substr(0, 16), nullptr, 16);
    out[1] = std::stoull(hash.substr(16), nullptr, 16);
}

std::string discpp::CombineAvatarHash(const uint64_t in[2]) {
    std::stringstream stream;
    stream << std::setw(16) << std::setfill('0') << std::hex << in[0];
    stream << std::setw(16) << std::setfill('0') << std::hex << in[1];

    return stream.str();
}
