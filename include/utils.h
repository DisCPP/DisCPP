#ifndef DISCORDPP_UTILS_H
#define DISCORDPP_UTILS_H

#include <nlohmann/json.hpp>

#include <cpr/cpr.h>

namespace discord{
	class Bot;

	std::string GetOsName() {
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

	template <typename S>
	inline void FormatSlice(std::string const& input_str,
		std::stringstream& output_str, int& start_index,
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
		assert(sizeof...(args) == std::count(str.begin(), str.end(), '%') &&
			"Amount of % does not match amount of arguments");
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

	nlohmann::json SendGetRequest(std::string url, cpr::Header headers, cpr::Parameters parameters) {
		auto r = cpr::Get(cpr::Url{ url }, headers, parameters);

		return nlohmann::json::parse(r.text);
	}
}

#endif