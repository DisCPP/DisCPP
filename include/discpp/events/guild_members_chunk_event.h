#ifndef DISCPP_GUILD_MEMBERS_CHUNK_EVENT_H
#define DISCPP_GUILD_MEMBERS_CHUNK_EVENT_H

#include "../event.h"

namespace discpp {
	class GuildMembersChunkEvent : public Event {
	public:
		GuildMembersChunkEvent() = default;
	};
}

#endif