#ifndef DISCORDPP_GUILD_BAN_ADD_EVENT_H
#define DISCORDPP_GUILD_BAN_ADD_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildBanAddEvent : public Event {
	public:
		inline GuildBanAddEvent(discord::Guild guild, discord::User user) : guild(guild), user(user) {}

		discord::Guild guild;
		discord::User user;
	};
}

#endif