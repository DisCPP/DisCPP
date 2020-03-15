#ifndef DISCORDPP_GUILD_MEMBERS_CHUNK_EVENT_H
#define DISCORDPP_GUILD_MEMBERS_CHUNK_EVENT_H

#include "../event.h"

namespace discord {
	class GuildMembersChunkEvent : public Event {
	public:
		GuildMembersChunkEvent() = default;
	};
}

#endif