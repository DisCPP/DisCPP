#ifndef DISCORDPP_UTILS_H
#define DISCORDPP_UTILS_H

#include "discord_object.h"

#include <nlohmann/json.hpp>

#include <cpr/cpr.h>

namespace discord {
	class Bot;

	namespace globals {
		//class Bot* bot_instance;
		inline discord::Bot* bot_instance;
	}

	namespace specials {
		enum NitroSubscription : int {
			NO_NITRO = 0,
			NITRO_CLASSIC = 1,
			NITRO = 2
		};

		enum NitroTier : int {
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

		enum ExplicitContentFilterLevel : int {
			DISABLED = 0,
			MEMBERS_WITHOUT_ROLES = 1,
			ALL_MEMBERS = 2
		};

		enum MFALevel : int {
			NO_MFA = 0,
			ELEVATED = 1
		};
	}

	std::string GetOsName();

	template <typename S>
	inline void FormatSlice(std::string const& input_str, std::stringstream& output_str, int& start_index,
		S var) {
		long unsigned int index = input_str.find('%', start_index);
		if (index == std::string::npos) {
			return;
		}
		output_str << input_str.substr(start_index, index - start_index) << var;
		start_index = index + 1;
	}

	template <typename... T>
	inline std::string Format(std::string const& str, T... args) {
		assert(sizeof...(args) == std::count(str.begin(), str.end(), '%') && "Amount of % does not match amount of arguments");
		std::stringstream output_str;
		int start_index = 0;
		((FormatSlice(str, output_str, start_index, std::forward<T>(args))), ...);
		output_str << str.substr(start_index, str.length());
		return output_str.str();
	}

	template <typename... Tys>
	inline std::string Endpoint(std::string endpoint_format, Tys&&... args) {
		endpoint_format = endpoint_format[0] == '/' ? endpoint_format : '/' + endpoint_format;
		return Format(std::string("https://discordapp.com/api/v6") + endpoint_format, std::forward<Tys>(args)...);
	}

	template <typename type>
	inline type GetFromVector(std::vector<type> vector, type item) {
		auto new_item = std::find_if(vector.begin(), vector.end(), [](type a, type b) { return a == b; });

		if (new_item != vector.end()) return new_item;
		return nullptr;
	}

	nlohmann::json SendGetRequest(std::string url, cpr::Header headers, cpr::Parameters parameters, cpr::Body body);
	nlohmann::json SendPostRequest(std::string url, cpr::Header headers, cpr::Parameters parameters, cpr::Body body);
	nlohmann::json SendPutRequest(std::string url, cpr::Header headers, cpr::Body body);
	nlohmann::json SendPatchRequest(std::string url, cpr::Header headers, cpr::Body body);
	nlohmann::json SendDeleteRequest(std::string url, cpr::Header headers);
	cpr::Header DefaultHeaders(cpr::Header add = {});
	bool StartsWith(std::string string, std::string prefix);
	std::vector<std::string> SplitString(std::string str, char delimter);
	std::string CombineVectorWithSpaces(std::vector<std::string> vector, int offset = 0);

	template<typename T>
	inline T GetDataSafely(nlohmann::json json, std::string value_name) {
		return (json.contains(value_name) && json[value_name] != nullptr) ? json[value_name].get<T>() : T();
	}
}

#endif