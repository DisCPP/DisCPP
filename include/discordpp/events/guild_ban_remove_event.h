#ifndef DISCORDPP_GUILD_BAN_REMOVE_EVENT_H
#define DISCORDPP_GUILD_BAN_REMOVE_EVENT_H

#include "../event.h"
#include "../guild.h"
#include "../user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildBanRemoveEvent : public Event {
	public:
		inline GuildBanRemoveEvent(discord::Guild guild, discord::User user) : guild(guild), user(user) {}

		discord::Guild guild;
		discord::User user;
	};
}

#endif