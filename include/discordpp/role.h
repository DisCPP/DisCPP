#ifndef DISCORDPP_ROLE_H
#define DISCORDPP_ROLE_H

#include "discord_object.h"
#include "permission.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Guild;

	class Role : public DiscordObject {
	public:
		Role() = default;
		Role(snowflake role_id, discord::Guild guild);
		Role(nlohmann::json json);

		snowflake id;  /**< Id of the current role */
		std::string name; /**< Name of the current role */
		int color; /**< Color of the current role */
		bool hoist; /**< Whether or not the role displays in member list */
		int position; /**< Position of the current role */
		Permissions permissions; /**< PermissionOverwrites for the current role */
		bool managed;
		bool mentionable; /**< Whether or not you can @ mention the current role */
	};
}

#endif