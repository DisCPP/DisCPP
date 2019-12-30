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

nlohmann::json discord::SendGetRequest(std::string url, cpr::Header headers, cpr::Parameters parameters, cpr::Body body) {
	auto result = cpr::Get(cpr::Url{ url }, headers, parameters, body);
	return nlohmann::json::parse((!result.text.empty()) ? result.text : "{}");
}

nlohmann::json discord::SendPostRequest(std::string url, cpr::Header headers, cpr::Parameters parameters, cpr::Body body) {
	auto result = cpr::Post(cpr::Url{ url }, headers, parameters, body);
	return nlohmann::json::parse( (!result.text.empty()) ? result.text : "{}");
}

nlohmann::json discord::SendPutRequest(std::string url, cpr::Header headers, cpr::Body body) {
	auto result = cpr::Put(cpr::Url{ url }, headers, body);
	return nlohmann::json::parse((!result.text.empty()) ? result.text : "{}");
}

nlohmann::json discord::SendPatchRequest(std::string url, cpr::Header headers, cpr::Body body) {
	auto result = cpr::Patch(cpr::Url{ url }, headers, body);
	return nlohmann::json::parse((!result.text.empty()) ? result.text : "{}");
}

nlohmann::json discord::SendDeleteRequest(std::string url, cpr::Header headers) {
	auto result = cpr::Delete(cpr::Url{ url }, headers);
	return nlohmann::json::parse((!result.text.empty()) ? result.text : "{}");
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