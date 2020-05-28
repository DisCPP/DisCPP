#ifndef DISCPP_DM_CHANNEL_DELETE_EVENT_H
#define DISCPP_DM_CHANNEL_DELETE_EVENT_H

#include "../event.h"
#include "../channel.h"



namespace discpp {
	class DMChannelDeleteEvent : public Event {
	public:
		inline DMChannelDeleteEvent(discpp::DMChannel channel) : channel(channel) {}

		discpp::DMChannel channel;
	};
}

#endif