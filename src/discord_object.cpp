#include "discord_object.h"

namespace discpp {
	DiscordObject::DiscordObject(const snowflake& id) : id(id) {

	}

	bool DiscordObject::operator==(DiscordObject& other) const {
		return this->id == other.id;
	}

	bool DiscordObject::operator==(snowflake& other) const {
		return this->id == other;
	}

	bool discpp::DiscordObject::operator!=(DiscordObject& other) const {
		return this->id != other.id;
	}

	bool discpp::DiscordObject::operator!=(snowflake& other) const {
		return this->id != other;
	}
}