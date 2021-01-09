#ifndef DISCPP_CLIENT_CONFIG_H
#define DISCPP_CLIENT_CONFIG_H

#include "intents.h"
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
		int shard_amount;
		int milli_sec_max_ratelimit;
		std::string logger_path;
		uint64_t intents;

        /**
         * @brief Creates a ClientConfig object.
         *
         * If you decide to not shard, you need to leave the field `shard_amount` at 1 or nothing will work.
         *
         * @return ClientConfig, its a constructor.
         */
		ClientConfig(std::vector<std::string> prefixes, uint64_t intents = discpp::GatewayIntents::DEFAULT, TokenType type = TokenType::BOT, int shard_amount = 1, int logger_flags = (unsigned int)logger_flags::ERROR_SEVERITY | (unsigned int)logger_flags::WARNING_SEVERITY | (unsigned int)logger_flags::INFO_SEVERITY, int message_cache_size = 5000, std::string logger_path = "", int milli_sec_max_ratelimit = 60000)
			: prefixes(std::move(prefixes)), intents(intents), type(type), shard_amount(shard_amount), logger_flags(logger_flags), message_cache_size(message_cache_size), logger_path(logger_path), milli_sec_max_ratelimit(milli_sec_max_ratelimit) {}
	};
}
#endif