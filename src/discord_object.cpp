#include "discord_object.h"

namespace discord {
	DiscordObject::DiscordObject(snowflake id) : id(id) {

	}

	bool DiscordObject::operator==(DiscordObject& other) const {
		return this->id == other.id;
	}

	bool DiscordObject::operator==(DiscordObject other) const {
		return this->id == other.id;
	}

	bool DiscordObject::operator==(snowflake& other) const {
		return this->id == other;
	}
}