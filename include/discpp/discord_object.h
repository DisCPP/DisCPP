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

		bool operator==(const DiscordObject& other) const;
		bool operator==(const discpp::Snowflake& other) const;

#if __cplusplus <= 201703L || defined(__GNUC__)
		bool operator!=(const DiscordObject& other) const;
		bool operator!=(const discpp::Snowflake& other) const;
#endif
	private:
	    uint8_t client_instance_id;
    protected:
        discpp::Client* GetClient() const;
	};
}

#endif