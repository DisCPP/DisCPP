#ifndef DISCPP_GUILD_ROLE_UPDATE_EVENT_H
#define DISCPP_GUILD_ROLE_UPDATE_EVENT_H

#include "../event.h"
#include "../role.h"



namespace discpp {
	class GuildRoleUpdateEvent : public Event {
	public:
		inline GuildRoleUpdateEvent(Shard& shard, discpp::Role role) : Event(shard), role(role) {}

		discpp::Role role;

        virtual int GetEventType() const override {
            return 18;
        }
	};
}

#endif