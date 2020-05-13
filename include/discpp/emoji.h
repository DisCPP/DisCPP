#ifndef DISCPP_EMOJI_H
#define DISCPP_EMOJI_H

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "discord_object.h"
#include "user.h"
#include "role.h"
#include "utils.h"



//#include <cpprest/uri.h>

#include <locale>
#include <codecvt>
#include <string>

namespace discpp {
	class Guild;
	class User;

	class Emoji {
	public:
		Emoji() = default;
		Emoji(std::string name, snowflake id);
		Emoji(discpp::Guild& guild, snowflake id);
		Emoji(rapidjson::Document& json);
		Emoji(std::wstring w_unicode);
		Emoji(std::string s_unicode);

        bool operator==(Emoji& other) const {
            auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();

            // If the other's name is empty but mine is not then it must have unicode.
            if (other.name.empty() && !name.empty() && unicode.empty()) {
                return wstr_converter.to_bytes(other.unicode) == name;
            } else if (!other.name.empty() && name.empty() && !unicode.empty()) {
                return wstr_converter.to_bytes(unicode) == other.name;
            } else if (other.id != 0 && id != 0 && !other.name.empty() && !name.empty()) {
                return other.id == id && other.name == name;
            }

            return false; // Hopefully this never runs.
        }

        std::string ToString() {
            /**
			 * @brief Gets a string representation of this emoji for sending through messages.
			 *
			 * ```cpp
			 *      ctx.Send(emoji.ToString() + " Failed to make request!");
			 * ```
			 *
			 * @return std::string
			 */

            auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();

            if (name.empty() && id == 0) {
                return wstr_converter.to_bytes(unicode);
            } else {
                return name;
            }
        }

		std::string ToURL() {
			/**
			 * @brief Gets a URL representation of this emoji for adding reactions.
			 *
			 * This URI encodes the emoji and returns the string result.
			 *
			 * ```cpp
			 *      std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%/@me", channel.id, id, emoji);
			 * ```
			 *
			 * @return std::string
			 */

            auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();

			std::wstring emoji;
			if (name.empty()) {
				emoji = unicode;
			} else {
				emoji = wstr_converter.from_bytes(name) + L":" + wstr_converter.from_bytes(id);
			}

			return URIEncode(wstr_converter.to_bytes(emoji));
		}

		discpp::snowflake id; /**< ID of the current emoji */
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