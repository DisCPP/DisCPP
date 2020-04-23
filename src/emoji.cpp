#include "emoji.h"
#include "guild.h"

namespace discord {
	Emoji::Emoji(std::string name, snowflake id) : name(EscapeString(name)), DiscordObject(id) {
		/**
		 * @brief Constructs a discord::Emoji object with a name and id.
		 *
		 * ```cpp
		 *      discord::Emoji emoji("no_x", 657246994997444614);
		 * ```
		 *
		 * @param[in] name The name of the emoji
		 * @param[in] id The id of the emoji
		 *
		 * @return discord::Emoji, this is a constructor.
		 */
	}

	Emoji::Emoji(discord::Guild guild, snowflake id) : DiscordObject(id) {
		/**
		 * @brief Constructs a discord::Emoji object using a guild object and id.
		 *
		 * This constructor searches the emoji cache in the guild object to get an emoji object.
		 *
		 * ```cpp
		 *      discord::Emoji emoji(guild, 657246994997444614);
		 * ```
		 *
		 * @param[in] guild The guild that has this emoji.
		 * @param[in] id The id of the emoji.
		 *
		 * @return discord::Emoji, this is a constructor.
		 */

		std::unordered_map<snowflake, Emoji>::iterator it = guild.emojis.find(id);
		if (it != guild.emojis.end()) {
			*this = it->second;
		}
	}

	Emoji::Emoji(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::Emoji object by parsing json.
		 *
		 * ```cpp
		 *      discord::Emoji emoji(json);
		 * ```
		 *
		 * @param[in] json The json that makes up of emoji object.
		 *
		 * @return discord::Emoji, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		name = GetDataSafely<std::string>(json, "name");
		if (json.contains("roles")) {
			for (nlohmann::json role : json["roles"]) {
				roles.push_back(role);
			}
		}
		if (json.contains("user")) {
			user = discord::User(json["user"]);
		}
		require_colons = GetDataSafely<bool>(json, "require_colons");
		managed = GetDataSafely<bool>(json, "managed");
		animated = GetDataSafely<bool>(json, "animated");
	}

	Emoji::Emoji(std::wstring w_unicode) : unicode(w_unicode) {
		/**
		 * @brief Constructs a discord::Emoji object with a std::wstring unicode representation.
		 *
		 * ```cpp
		 *      discord::Emoji emoji(L"\u0030");
		 * ```
		 *
		 * @param[in] w_unicode The std::wstring unicode representation of this emoji.
		 *
		 * @return discord::Emoji, this is a constructor.
		 */
	}

    Emoji::Emoji(std::string s_unicode) {
        /**
         * @brief Constructs a discord::Emoji object with a std::string unicode representation.
         *
         * ```cpp
         *      discord::Emoji emoji("\u0030");
         * ```
         *
         * @param[in] s_unicode The std::string unicode representation of this emoji.
         *
         * @return discord::Emoji, this is a constructor.
         */
        auto converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();
        this->unicode = converter.from_bytes(s_unicode);
    }
}