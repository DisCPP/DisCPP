#ifndef DISCPP_GUILD_MEMBERS_CHUNK_EVENT_H
#define DISCPP_GUILD_MEMBERS_CHUNK_EVENT_H

#include "../event.h"

#include <utility>

namespace discpp {
	class GuildMembersChunkEvent : public Event {
	public:
	    GuildMembersChunkEvent() = default;
        GuildMembersChunkEvent(std::shared_ptr<discpp::Guild> guild, std::unordered_map<discpp::Snowflake, discpp::Member> members, int chunk_index, int chunk_count, std::vector<discpp::Presence> presences, std::string nonce) : guild(std::move(guild)), members(std::move(members)), chunk_index(chunk_index), chunk_count(chunk_count), presences(std::move(presences)), nonce(std::move(nonce)) {};

        std::shared_ptr<discpp::Guild> guild;
		std::unordered_map<discpp::Snowflake, discpp::Member> members;
		int chunk_index;
		int chunk_count;
		std::vector<discpp::Presence> presences;
		std::string nonce;
	};
}

#endif