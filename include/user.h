#ifndef DISCORDPP_USER_H
#define DISCORDPP_USER_H

#include "discord_object.h"
#include "utils.h"

#include <nlohmann/json.hpp>

namespace discord {
	class User : DiscordObject {
	public:
		User() = default;
		User(snowflake id);
		User(nlohmann::json json);

		snowflake id;
		std::string username;
		std::string discriminator;
		std::string avatar;
		bool bot;
		bool system;
		bool mfa_enabled;
		std::string locale;
		bool verified;
		std::string email;
		int flags;
		discord::specials::NitroSubscription premium_type;
	};
}

#endif