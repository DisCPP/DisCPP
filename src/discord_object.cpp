#include "discord_object.h"
#include "client.h"

namespace discpp {
    DiscordObject::DiscordObject(discpp::Client *client) : client(client) {

    }

	DiscordObject::DiscordObject(discpp::Client *client, const Snowflake& id) : client(client), id(id) {

	}

	bool DiscordObject::operator==(DiscordObject& other) const {
		return this->id == other.id;
	}

	bool DiscordObject::operator==(Snowflake& other) const {
		return this->id == other;
	}

	bool discpp::DiscordObject::operator!=(DiscordObject& other) const {
		return this->id != other.id;
	}

	bool discpp::DiscordObject::operator!=(Snowflake& other) const {
		return this->id != other;
	}
}