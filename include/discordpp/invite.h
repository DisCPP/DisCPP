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

		std::string code;
		discord::Guild guild;
		discord::Channel channel;
		discord::User target_user;
		int target_user_type;
		int approximate_presence_count;
		int approximate_member_count;
	};
}

#endif