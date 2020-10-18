#ifndef DISCPP_GUILD_ROLE_DELETE_EVENT_H
#define DISCPP_GUILD_ROLE_DELETE_EVENT_H

#include "../event.h"
#include "../role.h"



namespace discpp {
	class GuildRoleDeleteEvent : public Event {
	public:
		inline GuildRoleDeleteEvent(Shard& shard, discpp::Role role) : Event(shard), role(role) {}

		discpp::Role role;

        virtual int GetEventType() const override {
            return 17;
        }
	};
}

#endif