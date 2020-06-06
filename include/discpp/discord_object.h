#ifndef DISCPP_OBJECT_H
#define DISCPP_OBJECT_H

#include "Snowflake.h"

#include <cstdint>
#include <iostream>


namespace discpp {

	class DiscordObject {
	public:
		DiscordObject() = default;
		DiscordObject(const discpp::Snowflake& id);

		discpp::Snowflake id = 0;

		bool operator==(DiscordObject& other) const;
		bool operator==(discpp::Snowflake& other) const;

		bool operator!=(DiscordObject& other) const;
		bool operator!=(discpp::Snowflake& other) const;
	};
}

#endif