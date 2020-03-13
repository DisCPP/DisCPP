#ifndef DISCORDPP_GUILD_BAN_REMOVE_EVENT_H
#define DISCORDPP_GUILD_BAN_REMOVE_EVENT_H

#include "event.h"
#include "guild.h"
#include "user.h"

#include <nlohmann/json.hpp>

namespace discord {
	class GuildBanRemoveEvent : public Event {
	public:
		inline GuildBanRemoveEvent(nlohmann::json json) : guild(discord::Guild(json["guild_id"].get<snowflake>())), user(discord::User(json["user"])) {}
		inline GuildBanRemoveEvent(discord::Channel channel, discord::User user) : channel(channel), user(user) {}

		discord::Guild guild;
		discord::User user;
	};
}

#endif