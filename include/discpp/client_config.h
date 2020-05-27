#ifndef DISCPP_CLIENT_CONFIG_H
#define DISCPP_CLIENT_CONFIG_H

#include "log.h"
#include <string>
#include <utility>
#include <vector>

namespace discpp {
    enum class TokenType {
        USER,
        BOT
    };

	class ClientConfig {
	public:
		std::vector<std::string> prefixes;
		TokenType type;
		int logger_flags;
		int message_cache_size;
		std::string logger_path;
		ClientConfig(std::vector<std::string> prefixes, TokenType type = TokenType::BOT, int logger_flags = (unsigned int)logger_flags::ERROR_SEVERITY | (unsigned int)logger_flags::WARNING_SEVERITY, int message_cache_size = 5000, std::string logger_path = "")
			: prefixes(std::move(prefixes)), type(type), logger_flags(logger_flags), message_cache_size(message_cache_size), logger_path(std::move(logger_path)) {}
	};
}
#endif