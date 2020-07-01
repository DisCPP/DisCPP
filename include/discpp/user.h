#ifndef DISCPP_USER_H
#define DISCPP_USER_H

#include "discord_object.h"
#include "utils.h"

namespace discpp {
	class Channel;
	class Integration;

	class User : public DiscordObject {
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

            /**
             * @brief Constructs a discpp::User::Connection object by parsing json.
             *
             * ```cpp
             *      discpp::User::Connection connection(json);
             * ```
             *
             * @param[in] json The json that makes up the connection object.
             *
             * @return discpp::User::Connection, this is a constructor.
             */
            Connection(rapidjson::Document& json);
        };

		User() = default;

        /**
         * @brief Constructs a discpp::User object from an id.
         *
         * This constructor searches the user cache to get a user object.
         *
         * ```cpp
         *      discpp::User user(583251190591258624);
         * ```
         *
         * @param[in] id The id of the user.
         *
         * @return discpp::User, this is a constructor.
         */
		User(const Snowflake& id);

        /**
         * @brief Constructs a discpp::User object by parsing json.
         *
         * ```cpp
         *      discpp::User user(json);
         * ```
         *
         * @param[in] json The json that makes up of user object.
         *
         * @return discpp::User, this is a constructor.
         */
		User(rapidjson::Document& json);

        /**
         * @brief Create a DM channel with this user.
         *
         * ```cpp
         *      discpp::Channel channel = user.CreateDM();
         * ```
         *
         * @return discpp::Channel
         */
		discpp::Channel CreateDM();

        /**
         * @brief Retrieve user avatar url.
         *
         * ```cpp
         *      std::string avatar_url = user.GetAvatarURL()
         * ```
         *
         * @param[in] img_type Optional parameter for type of image
         *
         * @return std::string
         */
        std::string GetAvatarURL(const ImageType& img_type = ImageType::AUTO) const;

        /**
         * @brief Gets the formatted created at time and date for this user.
         *
         * Formatted output: `%Y-%m-%d @ %H:%M:%S GMT`
         *
         * @return std::string
         */
		std::string GetFormattedCreatedAt() const;

        /**
         * @brief Gets the created at time and date for this guild.
         *
         * @return std::chrono::system_clock::time_point
         */
        std::chrono::system_clock::time_point GetCreatedAt() const;

        /**
         * @brief Creates a mention string for this user.
         *
         * ```cpp
         *      std::string mention = user.CreateMention();
         * ```
         *
         * @return std::string
         */
		std::string CreateMention();

        /**
         * @brief Gets the users discriminator as a string.
         *
         * @return std::string
         */
		std::string GetDiscriminator() const;

        /**
         * @brief Checks if the user is a bot
         *
         * @return bool
         */
		bool IsBot() const;

        /**
         * @brief Checks if the user is a discord staff user.
         *
         * @return bool
         */
		bool IsSystemUser();

		std::string username; /**< The user's username, not unique across the platform. */
		// int public_flags; // Is this ever needed?
    private:
        unsigned char flags;
        unsigned short discriminator;

        uint64_t avatar_hex[2] = {0, 0};
        bool is_avatar_gif = false;
	};
}

#endif
