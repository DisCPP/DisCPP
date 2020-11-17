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

#if __cplusplus <= 201703L
		bool DiscordObject::operator!=(DiscordObject& other) const {
            return this->id != other.id;
        }

        bool DiscordObject::operator!=(Snowflake& other) const {
            return this->id != other;
        }
#endif

    discpp::Client* DiscordObject::GetClient() const {
        return discpp::Client::GetInstance(client_instance_id);
    }
}