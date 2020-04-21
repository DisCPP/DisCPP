#ifndef DISCORDPP_INVITE_H
#define DISCORDPP_INVITE_H

#include "discord_object.h"
#include "guild.h"
#include "channel.h"
#include "user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Invite : public DiscordObject {
	public:
		Invite() = default;
		Invite(nlohmann::json json);

		std::string code; /**< URL Code */
		discord::Guild guild; /**< Guild the current invite points to */
		discord::Channel channel; /**< Channel the current invite points to */
		discord::User target_user;
		int target_user_type;
		int approximate_presence_count; /**< Approximate count of all currently online members in the guild the invite points to */
		int approximate_member_count; /**< Approximate count of all members in the guild the invite points to */
	};
}

#endif