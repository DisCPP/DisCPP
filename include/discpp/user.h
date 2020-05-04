#ifndef DISCPP_USER_H
#define DISCPP_USER_H

#include "discord_object.h"
#include "utils.h"
#include "activity.h"
#include <nlohmann/json.hpp>

namespace discpp {
	enum ImageType : int { AUTO, WEBP, PNG, JPEG, GIF };
	class Channel;
	class Integration;

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
		std::vector<Integration> integrations;
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

		discpp::Channel CreateDM();
		std::vector<Connection> GetUserConnections();
		std::string GetAvatarURL(ImageType imgType = ImageType::AUTO);

		std::string username; /**< The user's username, not unique across the platform. */
		std::string discriminator; /**< The user's 4-digit discord-tag. */
		std::string avatar; /**< The user's avatar hash. */
		bool bot; /**< Whether the user belongs to an OAuth2 application. */
		bool system; /**< Whether the user is an Official Discord System user (part of the urgent message system). */
		bool mfa_enabled; /**< Whether the user has two factor enabled on their account.*/
		std::string locale; /**< The user's chosen language option. */
		bool verified; /**< Whether the email on this account has been verified. */
		std::string email; /**< The user's email. */
		int flags; /**< The flags on a user's account. */
		discpp::specials::NitroSubscription premium_type; /**< The type of Nitro subscription on a user's account. */
		int public_flags;
		std::string created_at; /**< The creation date of the current user's account. */
		std::string mention; /**< The @ mention of the current user Ex: <@150312037426135041>. */
	};
}

#endif
