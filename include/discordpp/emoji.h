#ifndef DISCORDPP_EMOJI_H
#define DISCORDPP_EMOJI_H

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "discord_object.h"
#include "user.h"
#include "role.h"

#include <nlohmann/json.hpp>

#include <cpprest/uri.h>

#include <locale>
#include <codecvt>
#include <string>

namespace discord {
	class Guild;
	class User;

	class Emoji : public DiscordObject {
	private:
		std::wstring WStringFromString(std::string string) {
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(string);
		}

	public:
		Emoji() = default;
		Emoji(std::string name, snowflake id);
		Emoji(discord::Guild guild, snowflake id);
		Emoji(nlohmann::json json);
		Emoji(std::wstring unicode);

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

			emoji = web::uri::encode_uri(emoji);

			return converter.to_bytes(emoji);
		}

		//snowflake id;
		std::string name;
		std::wstring unicode;
		std::vector<discord::Role> roles;
		discord::User user;
		bool require_colons;
		bool managed;
		bool animated;
	};
}

#endif