#ifndef DISCORDPP_USER_H
#define DISCORDPP_USER_H

#include "discord_object.h"
#include "utils.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Channel;
	class GuildIntegration;

	enum ConnectionVisibility : int {
		NONE = 0,
		EVERYONE = 1
	};

	class Connection {
	public:
		std::string id;
		std::string name;
		std::string type;
		bool revoked;
		std::vector<GuildIntegration> integrations;
		bool verified;
		bool friend_sync;
		bool show_activity;
		ConnectionVisibility visibility;

		Connection() = default;
		Connection(nlohmann::json json);
	};

	class User : DiscordObject {
	public:
		User() = default;
		User(snowflake id);
		User(nlohmann::json json);

		discord::Channel CreateDM();
		std::vector<Connection> GetUserConnections();

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