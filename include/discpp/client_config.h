#ifndef DISCPP_CLIENT_CONFIG_H
#define DISCPP_CLIENT_CONFIG_H

#include "log.h"
#include "client.h"
#include <string>
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
		ClientConfig(std::vector<std::string> prefixes, TokenType type = TokenType::BOT, int logger_flags = logger_flags::ERROR_SEVERITY | logger_flags::WARNING_SEVERITY, int message_cache_size = 5000, std::string logger_path = "")
			: prefixes(prefixes), type(type), logger_flags(logger_flags), message_cache_size(message_cache_size), logger_path(logger_path) {}
	};
}
#endif