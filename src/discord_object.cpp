#include "discord_object.h"
#include "client.h"

namespace discpp {
    DiscordObject::DiscordObject(discpp::Client *client) {
        if (client) {
            this->client_instance_id = client->GetInstanceID();
        }
    }

	DiscordObject::DiscordObject(discpp::Client *client, const Snowflake& id) : id(id) {
        if (client) {
            this->client_instance_id = client->GetInstanceID();
        }
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

    discpp::Client* DiscordObject::GetClient() const {
        return discpp::Client::GetInstance(client_instance_id);
    }
}