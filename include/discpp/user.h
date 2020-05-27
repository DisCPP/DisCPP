#ifndef DISCPP_USER_H
#define DISCPP_USER_H

#include "discord_object.h"
#include "utils.h"

namespace discpp {
	enum ImageType : int { AUTO, WEBP, PNG, JPEG, GIF };
	class Channel;
	class Integration;

	class User : public DiscordObject {
	private:
        char flags;
        unsigned short discriminator;
	public:
        enum class ConnectionVisibility : int {
            NONE = 0,
            EVERYONE = 1
        };

        class Connection {
        public:
            std::string id;
            std::string name;
            std::string type;
            bool revoked;
            std::vector<Integration> integrations;
            bool verified;
            bool friend_sync;
            bool show_activity;
            ConnectionVisibility visibility;

            Connection() = default;
            Connection(rapidjson::Document& json);
        };

		User() = default;
		User(snowflake id);
		User(rapidjson::Document& json);

		discpp::Channel CreateDM();
		std::string GetAvatarURL(ImageType imgType = ImageType::AUTO) const;
		std::string CreatedAt();
		std::string CreateMention();
		std::string GetDiscriminator() const;

		bool IsBot();
		bool IsSystemUser();

		std::string username; /**< The user's username, not unique across the platform. */
		std::string avatar; /**< The user's avatar hash. */
		std::string locale; /**< The user's chosen language option. */
		bool verified; /**< Whether the email on this account has been verified. */
		std::string email; /**< The user's email. */
		discpp::specials::NitroSubscription premium_type; /**< The type of Nitro subscription on a user's account. */
		int public_flags;
	};
}

#endif
