#ifndef DISCPP_CLIENT_CONFIG_H
#define DISCPP_CLIENT_CONFIG_H

#include "log.h"
#include <string>
#include <vector>

namespace discpp {
	class ClientConfig {
	public:
		std::vector<std::string> prefixes;
		TokenType type;
		int logger_flags;
		int messageCacheSize;
		std::string logger_path;
		ClientConfig(std::vector<std::string> prefixes, TokenType type = TokenType::BOT, int logger_flags = logger_flags::ERROR_SEVERITY | logger_flags::WARNING_SEVERITY, int messageCacheSize = 5000, std::string logger_path = "")
			: prefixes(prefixes), type(type), logger_flags(logger_flags), messageCacheSize(messageCacheSize), logger_path(logger_path) {}
	};
}
#endif