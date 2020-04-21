#ifndef DISCORDPP_USER_H
#define DISCORDPP_USER_H

#include "discord_object.h"
#include "utils.h"
#include "activity.h"
#include <nlohmann/json.hpp>

namespace discord {
	enum ImageType : int { AUTO, WEBP, PNG, JPEG, GIF };
	class Channel;
	class GuildIntegration;

	enum class ConnectionVisibility : int {
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

	class User : public DiscordObject {
	public:
		User() = default;
		User(snowflake id);
		User(nlohmann::json json);

		discord::Channel CreateDM();
		std::vector<Connection> GetUserConnections();
		std::string GetAvatarURL(ImageType imgType = ImageType::AUTO);
		//snowflake id;
		std::string username; /**< Username of the current user */
		std::string discriminator; /**< Discriminator of the current user */
		std::string avatar; /**< Hashed avatar of the current user */
		bool bot; /**< Whether or not the current user is a bot */
		bool system; /**< Whether or not the current user is part of discord's system (Clyde) */
		bool mfa_enabled; /**< Whether or not the current user has MFA enabled (only applies for the current user logged in) */
		std::string locale; /**< Locale of the current user (only applies for the current user logged in) */
		bool verified; /**< Whether or not the current bot user is verified */
		std::string email; /**< Email of the current user (only applies for the current user logged in) */
		int flags;
		discord::specials::NitroSubscription premium_type; /**< The status of discord nitro for the current user */
		std::string created_at; /**< The creation date of the current user's account */
		std::string mention; /**< The @ mention of the current user Ex: <@150312037426135041> */
	};
}

#endif
