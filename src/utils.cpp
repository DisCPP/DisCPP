#include "utils.h"
#include "client.h"
#include "client_config.h"
#include "exceptions.h"
#include "http_client.h"

#include <cstdlib>
#include <numeric>
#include <iomanip>
#include <sstream>

#include <rapidjson/writer.h>

#include <ixwebsocket/IXHttpClient.h>
#include <ixwebsocket/IXNetSystem.h>

std::string discpp::GetOsName() {
	#ifdef _WIN64
		return "Windows 64-bit";
	#elif _WIN32
		return "Windows 32-bit";
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
	ReplaceAll(tmp, to_search, replace_str);
	return tmp;
}

void discpp::ReplaceAll(std::string& data, const std::string& to_search, const std::string& replace_str) {
    // Get the first occurrence
    size_t pos = data.find(to_search);

    // Repeat till end is reached
    while(pos != std::string::npos) {
        // Replace this occurrence of Sub String
        data.replace(pos, to_search.size(), replace_str);
        // Get the next occurrence from the current position
        pos = data.find(to_search, pos + replace_str.size());
    }
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

int discpp::WaitForRateLimits(discpp::Client* client, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket) {

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
		    if (client) {
                client->logger->Error(LogTextColor::RED + "RateLimitBucketType is invalid!");
            }
			throw std::runtime_error("RateLimitBucketType is invalid!");
			break;
		}
	}

	if (rlmt->remaining_limit == 0) {
		double millisecond_time = rlmt->ratelimit_reset * 1000 - time(NULL) * 1000;

		if (millisecond_time > client->config.milli_sec_max_ratelimit) {
            throw exceptions::RatelimitTooLong("Ratelimit hit the max ratelimit", (int) millisecond_time);
		}

		if (millisecond_time > 0) {
		    if (client) {
                client->logger->Debug("Rate limit wait time: " + std::to_string(millisecond_time) + " milliseconds");
            }
			std::this_thread::sleep_for(std::chrono::milliseconds((int) millisecond_time));
		}
	}
	return 0;
}

bool HeaderContains(const std::map<std::string, std::string, discpp::CaseInsensitiveLess> &headers, const std::string& key) {
    return headers.count(key) > 0;
}

void discpp::HandleRateLimits(const std::map<std::string, std::string, discpp::CaseInsensitiveLess> &headers, const Snowflake& object, const RateLimitBucketType& ratelimit_bucket) {
	RateLimit* obj = nullptr;
	if (HeaderContains(headers, "x-ratelimit-global")) {
		obj = &global_ratelimit;
	} else if (HeaderContains(headers, "x-ratelimit-limit")) {
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

	auto limit_it = headers.find("x-ratelimit-limit");
    auto remaining_it = headers.find("x-ratelimit-limit");
    auto reset_it = headers.find("x-ratelimit-limit");

    if (limit_it != headers.end() && remaining_it != headers.end() && reset_it != headers.end()) {
        obj->limit = std::stoi(limit_it->second);
        obj->remaining_limit = std::stoi(remaining_it->second);
        obj->ratelimit_reset = std::stod(reset_it->second);
    }

    /*obj->limit = std::stoi(headers["x-ratelimit-limit"]);
    obj->remaining_limit = std::stoi(headers["x-ratelimit-remaining"]);
    obj->ratelimit_reset = std::stod(headers["x-ratelimit-reset"]);*/
}

time_t discpp::TimeFromDiscord(const std::string &time) {
    std::tm tm = {};
    std::istringstream ss(time);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::mktime(&tm);
}

[[deprecated]] [[maybe_unused]] time_t discpp::TimeFromSnowflake(const Snowflake& snow) {
    constexpr static uint64_t discord_epoch = 1420070400000;
    return (( (uint64_t) snow >> (unsigned int) 22) + discord_epoch) / 1000;
}

std::string discpp::FormatTime(const time_t& time, const std::string& format) {
    struct tm now{};
#if defined(__STDC_LIB_EXT1__) || !defined(__linux__)
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
    const auto* p_src = (const unsigned char *) str.c_str();
    const size_t SRC_LEN = str.length();
    auto* const p_start = new unsigned char[SRC_LEN * 3];
    unsigned char* p_end = p_start;
    const unsigned char* const SRC_END = p_src + SRC_LEN;

    for (; p_src < SRC_END; ++p_src) {
        if (SAFE[*p_src]) {
            *p_end++ = *p_src;
        } else {
            // escape this char
            *p_end++ = '%';
            *p_end++ = DEC2HEX[*p_src >> 4];
            *p_end++ = DEC2HEX[*p_src & 0x0F];
        }
    }

    std::string sResult((char *)p_start, (char *)p_end);
    delete [] p_start;
    return sResult;
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
