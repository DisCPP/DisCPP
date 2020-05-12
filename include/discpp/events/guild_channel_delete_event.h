#ifndef DISCPP_GUILD_CHANNEL_DELETE_EVENT_H
#define DISCPP_GUILD_CHANNEL_DELETE_EVENT_H

#include "../event.h"
#include "../channel.h"

#include <nlohmann/json.hpp>

namespace discpp {
	class GuildChannelDeleteEvent : public Event {
	public:
		inline GuildChannelDeleteEvent(std::shared_ptr<discpp::GuildChannel> channel) : channel(channel) {}

        std::shared_ptr<discpp::GuildChannel> channel;
	};
}

#endif