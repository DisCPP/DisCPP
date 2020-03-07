#include "emoji.h"
#include "guild.h"
#include "utils.h"
#include "user.h"

namespace discord {
	Emoji::Emoji(std::string name, snowflake id) : name(name), id(id) {
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

	Emoji::Emoji(discord::Guild guild, snowflake id) {
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

		auto emoji = std::find_if(guild.emojis.begin(), guild.emojis.end(), [id](discord::Emoji a) { return id == a.id; });

		if (emoji != guild.emojis.end()) {
			*this = *emoji;
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
}