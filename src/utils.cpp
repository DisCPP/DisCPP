#include "utils.h"
#include "bot.h"

#include <cstring>

std::string discord::GetOsName() {
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

nlohmann::json discord::HandleResponse(cpr::Response response, snowflake object, RateLimitBucketType ratelimit_bucket) {
	HandleRateLimits(response.header, object, ratelimit_bucket);
	return nlohmann::json::parse((!response.text.empty()) ? response.text : "{}");
}

nlohmann::json discord::SendGetRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket, cpr::Body body) {
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Get(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

nlohmann::json discord::SendPostRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket, cpr::Body body) {
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Post(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

nlohmann::json discord::SendPutRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket, cpr::Body body) {
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Put(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

nlohmann::json discord::SendPatchRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket, cpr::Body body) {
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Patch(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

nlohmann::json discord::SendDeleteRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket) {
	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Delete(cpr::Url{ url }, headers);
	return HandleResponse(result, object, ratelimit_bucket);
}

cpr::Header discord::DefaultHeaders(cpr::Header add) {
	cpr::Header headers = { { "Authorization", Format("Bot %", discord::globals::bot_instance->token) },
							{ "User-Agent", "DiscordBot (https://github.com/seanomik/discordpp, v0.0.0)" } };
	for (auto head : add) {
		headers.insert(headers.end(), head);
	}

	return headers;
}

bool discord::StartsWith(std::string string, std::string prefix) {
	return string.substr(0, prefix.size()) == prefix;
}

std::vector<std::string> discord::SplitString(std::string str, char delimter) {
	std::vector<std::string> out;
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delimter, end)) != std::string::npos) {
		end = str.find(delimter, start);
		out.push_back(str.substr(start, end - start));
	}

	return out;
}

std::string discord::CombineVectorWithSpaces(std::vector<std::string> vector, int offset) {
	return std::accumulate(vector.begin() + offset, vector.end(), std::string(""), [](std::string s0, std::string const& s1) { return s0 += " " + s1; }).substr(1);
}

std::string discord::ReadEntireFile(std::ifstream& file) {
	return std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
}

int discord::WaitForRateLimits(snowflake object, RateLimitBucketType ratelimit_bucket) {
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
			throw std::runtime_error("RateLimitBucketType is invalid!");
			break;
		}
	}

	if (rlmt->remaining_limit == 0) {
		auto current_time = boost::posix_time::second_clock::universal_time() - rlmt->ratelimit_reset;
		while (current_time.is_negative()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			current_time = boost::posix_time::second_clock::universal_time() - rlmt->ratelimit_reset;
		}
		return current_time.seconds();
	}
	return 0;
}

bool HeaderContains(cpr::Header header, std::string key) {
	for (auto head : header) {
		if (head.first == key) return true;
	}
	return false;
}

void discord::HandleRateLimits(cpr::Header header, snowflake object, RateLimitBucketType ratelimit_bucket) {
	RateLimit* obj = nullptr;
	if (HeaderContains(header, "X-RateLimit-Global")) {
		obj = &global_ratelimit;
	} else if (HeaderContains(header, "X-RateLimit-Limit")) {
		if (ratelimit_bucket == RateLimitBucketType::CHANNEL) {
			obj = &channel_ratelimit[object];
		}
		else if (ratelimit_bucket == RateLimitBucketType::GUILD) {
			obj = &guild_ratelimit[object];
		}
		else if (ratelimit_bucket == RateLimitBucketType::WEBHOOK) {
			obj = &webhook_ratelimit[object];
		}
		else if (ratelimit_bucket == RateLimitBucketType::GLOBAL) {
			obj = &global_ratelimit;
		}
		else {
			throw std::runtime_error("RateLimitBucketType is invalid!");
		}
	}
	else {
		return;
	}

	obj->limit = std::stoi(header["X-RateLimit-Limit"]);
	obj->remaining_limit = std::stoi(header["X-RateLimit-Remaining"]);
	obj->ratelimit_reset = boost::posix_time::from_time_t(std::stoi(header["X-RateLimit-Reset"]));
}
