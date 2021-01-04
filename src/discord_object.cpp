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

	bool DiscordObject::operator==(const DiscordObject& other) const {
		return this->id == other.id;
	}

	bool DiscordObject::operator==(const Snowflake& other) const {
		return this->id == other;
	}

#if __cplusplus <= 201703L || defined(__GNUC__)
    bool DiscordObject::operator!=(const DiscordObject& other) const {
        return this->id != other.id;
    }

    bool DiscordObject::operator!=(const Snowflake& other) const {
        return this->id != other;
    }
#endif

    discpp::Client* DiscordObject::GetClient() const {
        return discpp::Client::GetInstance(client_instance_id);
    }
}