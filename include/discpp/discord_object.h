#ifndef DISCPP_OBJECT_H
#define DISCPP_OBJECT_H

#include <cstdint>
#include <iostream>

namespace discpp {
	typedef uint64_t snowflake;

	class DiscordObject {
	public:
		DiscordObject() = default;
		DiscordObject(snowflake id);

		snowflake id;

		bool operator==(DiscordObject& other) const;
		bool operator==(snowflake& other) const;

		bool operator!=(DiscordObject& other) const;
		bool operator!=(snowflake& other) const;
	};
}

#endif