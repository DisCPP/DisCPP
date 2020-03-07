#include "utils.h"
#include "bot.h"

#include <cstring>

std::string discord::GetOsName() {
	/**
	 * @brief Get the OS name this application is running on.
	 *
	 * ```cpp
	 *      std::string os_name = discord::GetOSName();
	 * ```
	 *
	 * @return std::string
	 */

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
	/**
	 * @brief Handles a response from the discord servers.
	 *
	 * ```cpp
	 *      nlohmann::json response = discord::HandleResponse(cpr_response, object, discord::RateLimitBucketType::CHANNEL);
	 * ```
	 *
	 * @param[in] reponse The cpr response from the servers.
	 * @param[in] object The object id to handle the ratelimits for.
	 * @param[in] ratelimit_bucket The rate limit bucket.
	 *
	 * @return nlohmann::json
	 */

	HandleRateLimits(response.header, object, ratelimit_bucket);
	return nlohmann::json::parse((!response.text.empty()) ? response.text : "{}");
}

nlohmann::json discord::SendGetRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket, cpr::Body body) {
	/**
	 * @brief Sends a get request to a url.
	 *
	 * ```cpp
	 *      nlohmann::json response = discord::SendGetRequest(url, discord::DefaultHeaders(), object, discord::RateLimitBucketType::CHANNEL, {});
	 * ```
	 *
	 * @param[in] url The url to create a request to.
	 * @param[in] headers The http header.
	 * @param[in] object The object id to handle the ratelimits for.
	 * @param[in] ratelimit_bucket The rate limit bucket.
	 * @param[in] The cpr response body.
	 *
	 * @return nlohmann::json
	 */

	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Get(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

nlohmann::json discord::SendPostRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket, cpr::Body body) {
	/**
	 * @brief Sends a post request to a url.
	 *
	 * ```cpp
	 *      nlohmann::json response = discord::SendPostRequest(url, discord::DefaultHeaders(), object, discord::RateLimitBucketType::CHANNEL, {});
	 * ```
	 *
	 * @param[in] url The url to create a request to.
	 * @param[in] headers The http header.
	 * @param[in] object The object id to handle the ratelimits for.
	 * @param[in] ratelimit_bucket The rate limit bucket.
	 * @param[in] The cpr response body.
	 *
	 * @return nlohmann::json
	 */

	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Post(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

nlohmann::json discord::SendPutRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket, cpr::Body body) {
	/**
	 * @brief Sends a put request to a url.
	 *
	 * ```cpp
	 *      nlohmann::Json response = discord::SendPutRequest(url, discord::DefaultHeaders(), object, discord::RateLimitBucketType::CHANNEL, {});
	 * ```
	 *
	 * @param[in] url The url to create a request to.
	 * @param[in] headers The http header.
	 * @param[in] object The object id to handle the ratelimits for.
	 * @param[in] ratelimit_bucket The rate limit bucket.
	 * @param[in] The cpr response body.
	 *
	 * @return nlohmann::json
	 */

	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Put(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

nlohmann::json discord::SendPatchRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket, cpr::Body body) {
	/**
	 * @brief Sends a patch request to a url.
	 *
	 * ```cpp
	 *      nlohmann::json response = discord::SendPatchRequest(url, discord::DefaultHeaders(), object, discord::RateLimitBucketType::CHANNEL, {});
	 * ```
	 *
	 * @param[in] url The url to create a request to.
	 * @param[in] headers The http header.
	 * @param[in] object The object id to handle the ratelimits for.
	 * @param[in] ratelimit_bucket The rate limit bucket.
	 * @param[in] The cpr response body.
	 *
	 * @return nlohmann::json
	 */

	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Patch(cpr::Url{ url }, headers, body);
	return HandleResponse(result, object, ratelimit_bucket);
}

nlohmann::json discord::SendDeleteRequest(std::string url, cpr::Header headers, snowflake object, RateLimitBucketType ratelimit_bucket) {
	/**
	 * @brief Sends a delete request to a url.
	 *
	 * ```cpp
	 *      nlohmann::json response = discord::SendDeleteRequest(url, discord::DefaultHeaders(), object, discord::RateLimitBucketType::CHANNEL);
	 * ```
	 *
	 * @param[in] url The url to create a request to.
	 * @param[in] headers The http header.
	 * @param[in] object The object id to handle the ratelimits for.
	 * @param[in] ratelimit_bucket The rate limit bucket.
	 *
	 * @return nlohmann::json
	 */

	WaitForRateLimits(object, ratelimit_bucket);
	cpr::Response result = cpr::Delete(cpr::Url{ url }, headers);
	return HandleResponse(result, object, ratelimit_bucket);
}

cpr::Header discord::DefaultHeaders(cpr::Header add) {
	/**
	 * @brief Gets the default headers to communicate with the discord servers.
	 *
	 * ```cpp
	 *      cpr::Header default_headers = discord::DefaultHeaders({});
	 * ```
	 *
	 * @param[in] add The headers to add to the default ones.
	 *
	 * @return nlohmann::json
	 */

	cpr::Header headers = { { "Authorization", Format("Bot %", discord::globals::bot_instance->token) },
							{ "User-Agent", "DiscordBot (https://github.com/seanomik/discordpp, v0.0.0)" } };
	for (auto head : add) {
		headers.insert(headers.end(), head);
	}

	return headers;
}

bool discord::StartsWith(std::string string, std::string prefix) {
	/**
	 * @brief Check if a string starts with another string.
	 *
	 * ```cpp
	 *      bool does_start_with = discord::StartsWith("$test_command", "$");
	 * ```
	 *
	 * @param[in] string The string to check if it starts with.
	 * @param[in] prefix The prefix to check if the string starts with it.
	 *
	 * @return bool
	 */

	return string.substr(0, prefix.size()) == prefix;
}

std::vector<std::string> discord::SplitString(std::string str, char delimiter) {
	/**
	 * @brief Split a string into a vector.
	 *
	 * ```cpp
	 *      std::vector<std::string> argument_vec = discord::SplitString(message.content, ' ');
	 * ```
	 *
	 * @param[in] str The string to split
	 * @param[in] delimiter The delimiter to split the string at.
	 *
	 * @return std::vector<std::string>
	 */

	std::vector<std::string> out;
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delimiter, end)) != std::string::npos) {
		end = str.find(delimiter, start);
		out.push_back(str.substr(start, end - start));
	}

	return out;
}

std::string discord::CombineVectorWithSpaces(std::vector<std::string> vector, int offset) {
	/**
	 * @brief Combine a vector into a string with spaces between each element.
	 *
	 * ```cpp
	 *      std::string combined = discord::CombineVectorWithSpaces(arguments, 1);
	 * ```
	 *
	 * @param[in] vector The vector to combine.
	 * @param[in] offset The vector element offset to start at (if you wanted to skip the first 2 elements, set this offset to "2").
	 *
	 * @return std::string
	 */

	return std::accumulate(vector.begin() + offset, vector.end(), std::string(""), [](std::string s0, std::string const& s1) { return s0 += " " + s1; }).substr(1);
}

std::string discord::ReadEntireFile(std::ifstream& file) {
	/**
	 * @brief Reads an entire file.
	 *
	 * ```cpp
	 *      std::string file_contents = discord::ReadEntireFile(file);
	 * ```
	 *
	 * @param[in] file The file to read.
	 *
	 * @return std::string
	 */

	return std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
}

int discord::WaitForRateLimits(snowflake object, RateLimitBucketType ratelimit_bucket) {
	/**
	 * @brief Wait for rate limits.
	 *
	 * ```cpp
	 *      discord::WaitForRateLimits(message.id, discord::RateLimitBucketType::CHANNEL);
	 * ```
	 *
	 * @param[in] object The object id.
	 * @param[in] ratelimit_bucket The rate limit bucket.
	 *
	 * @return int
	 */

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
	/**
	 * @brief Check if a cpr::Header contains a specific key.
	 *
	 * ```cpp
	 *      bool contains = discord::HeaderContains(headers, "auth");
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

void discord::HandleRateLimits(cpr::Header header, snowflake object, RateLimitBucketType ratelimit_bucket) {
	/**
	 * @brief Handle rate limites
	 *
	 * ```cpp
	 *      discord::HandleRateLimits(header, id, bucket);
	 * ```
	 *
	 * @param[in] header The headers to get the wait rate limit from.
	 * @param[in] object The object id.
	 * @param[in] ratelimit_bucket The rate limit bucket.
	 *
	 * @return int
	 */

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
