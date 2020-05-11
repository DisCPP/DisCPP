#include "emoji.h"
#include "guild.h"

namespace discpp {
	Emoji::Emoji(std::string name, snowflake id) : name(EscapeString(name)), id(id) {
		/**
		 * @brief Constructs a discpp::Emoji object with a name and id.
		 *
		 * ```cpp
		 *      discpp::Emoji emoji("no_x", 657246994997444614);
		 * ```
		 *
		 * @param[in] name The name of the emoji
		 * @param[in] id The id of the emoji
		 *
		 * @return discpp::Emoji, this is a constructor.
		 */
	}

	Emoji::Emoji(discpp::Guild& guild, snowflake id) : id(id) {
		/**
		 * @brief Constructs a discpp::Emoji object using a guild object and id.
		 *
		 * This constructor searches the emoji cache in the guild object to get an emoji object.
		 *
		 * ```cpp
		 *      discpp::Emoji emoji(guild, 657246994997444614);
		 * ```
		 *
		 * @param[in] guild The guild that has this emoji.
		 * @param[in] id The id of the emoji.
		 *
		 * @return discpp::Emoji, this is a constructor.
		 */

		std::unordered_map<snowflake, Emoji>::iterator it = guild.emojis.find(id);
		if (it != guild.emojis.end()) {
			*this = it->second;
		}
	}

	Emoji::Emoji(rapidjson::Document& json) {
		/**
		 * @brief Constructs a discpp::Emoji object by parsing json.
		 *
		 * ```cpp
		 *      discpp::Emoji emoji(json);
		 * ```
		 *
		 * @param[in] json The json that makes up of emoji object.
		 *
		 * @return discpp::Emoji, this is a constructor.
		 */

		id = json["id"].GetString();
		name = json["name"].GetString();
		rapidjson::Value::ConstMemberIterator itr = json.FindMember("roles");
		
		if (itr != json.MemberEnd()) {
			for (auto& role : json["roles"].GetArray()) {
				rapidjson::Document role_json;
				role_json.CopyFrom(role, role_json.GetAllocator());
				roles.push_back(discpp::Role(role_json));
			}
		}

		itr = json.FindMember("user");
		if (itr != json.MemberEnd()) {
			rapidjson::Document user_json;
			user_json.CopyFrom(json["user"], user_json.GetAllocator());
			user = discpp::User(user_json);
		}

		require_colons = json["require_colons"].GetBool();
		managed = json["managed"].GetBool();
		animated = json["animated"].GetBool();
	}

	Emoji::Emoji(std::wstring w_unicode) : unicode(w_unicode) {
		/**
		 * @brief Constructs a discpp::Emoji object with a std::wstring unicode representation.
		 *
		 * ```cpp
		 *      discpp::Emoji emoji( (std::wstring) L"\u0030");
		 * ```
		 *
		 * @param[in] w_unicode The std::wstring unicode representation of this emoji.
		 *
		 * @return discpp::Emoji, this is a constructor.
		 */
	}

    Emoji::Emoji(std::string s_unicode) {
        /**
         * @brief Constructs a discpp::Emoji object with a std::string unicode representation.
         *
         * ```cpp
         *      discpp::Emoji emoji( (std::string) "\u0030");
         * ```
         *
         * @param[in] s_unicode The std::string unicode representation of this emoji.
         *
         * @return discpp::Emoji, this is a constructor.
         */
        auto converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();
        this->unicode = converter.from_bytes(s_unicode);
    }
}