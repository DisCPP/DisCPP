#ifndef DISCPP_EMOJI_H
#define DISCPP_EMOJI_H

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "discord_object.h"
#include "user.h"
#include "role.h"

#include <nlohmann/json.hpp>

#include <cpprest/uri.h>

#include <locale>
#include <codecvt>
#include <string>

namespace discpp {
	class Guild;
	class User;

	class Emoji : public DiscordObject {
	public:
		Emoji() = default;
		Emoji(std::string name, snowflake id);
		Emoji(discpp::Guild guild, snowflake id);
		Emoji(nlohmann::json json);
		Emoji(std::wstring w_unicode);
		Emoji(std::string s_unicode);

		std::string ToString() {
			/**
			 * @brief Gets a string representation of this emoji, if its for messages or reactions.
			 *
			 * ```cpp
			 *      std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%/@me", channel.id, id, emoji);
			 * ```
			 *
			 * @return std::string
			 */

			auto converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();

			std::wstring emoji;
			if (name.empty()) {
				emoji = unicode;
			} else {
				emoji = converter.from_bytes(name) + L":" + converter.from_bytes(id);
			}

#ifdef _WIN32
			emoji = web::uri::encode_uri(emoji);
			return converter.to_bytes(emoji);
#else
            std::string emoji_str = converter.to_bytes(emoji);
            emoji_str = web::uri::encode_uri(emoji_str);
            return emoji_str;
#endif
		}

		//snowflake id;
		std::string name; /**< Name of the current emoji */
		std::wstring unicode; /**< Unicode representation of the current emoji */
		std::vector<discpp::Role> roles; /**< Roles */
		discpp::User user;
		bool require_colons;
		bool managed;
		bool animated; /**< Whether or not the current emoji is animated */
	};
}

#endif