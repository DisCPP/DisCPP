#ifndef DISCORDPP_OBJECT_H
#define DISCORDPP_OBJECT_H

#include <cstdint>
#include <iostream>

namespace discord {
	typedef int64_t snowflake;

	class DiscordObject {
	public:
		DiscordObject() = default;
		DiscordObject(snowflake id);

		snowflake id;

		bool operator==(DiscordObject& other) const;
		bool operator==(snowflake& other) const;
	};
}

#endif