#ifndef DISCPP_GUILD_ROLE_CREATE_EVENT_H
#define DISCPP_GUILD_ROLE_CREATE_EVENT_H

#include "../event.h"
#include "../role.h"



namespace discpp {
	class GuildRoleCreateEvent : public Event {
	public:
		inline GuildRoleCreateEvent(Shard& shard, discpp::Role role) : Event(shard), role(role) {}

		discpp::Role role;

        virtual int GetEventType() const override {
            return 16;
        }
	};
}

#endif