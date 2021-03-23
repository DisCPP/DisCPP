#ifndef DISCPP_OBJECT_H
#define DISCPP_OBJECT_H

#include "snowflake.h"

#include <cstdint>
#include <iostream>
namespace discpp {
    class Client;

	class DiscordObject {
	public:
        DiscordObject() = default;
		DiscordObject(discpp::Client* client);
		DiscordObject(discpp::Client* client, const discpp::Snowflake& id);

		discpp::Snowflake id = 0;

		bool operator==(DiscordObject& other) const;
		bool operator==(discpp::Snowflake& other) const;

#if __cplusplus <= 201703L || defined(__GNUC__)
		bool operator!=(DiscordObject& other) const;
		bool operator!=(discpp::Snowflake& other) const;
#endif
	protected:
	    uint8_t client_instance_id;

        discpp::Client* GetClient() const;
	};
}

#endif