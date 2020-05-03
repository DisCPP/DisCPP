#ifndef DISCPP_EMOJI_H
#define DISCPP_EMOJI_H

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "discord_object.h"
#include "user.h"
#include "role.h"

#include <nlohmann/json.hpp>

//#include <cpprest/uri.h>

#include <locale>
#include <codecvt>
#include <string>

namespace discpp {
	class Guild;
	class User;

	class Emoji {
	private:
	    // Safe characters for URI
        char SAFE[256] = {
            /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
            /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

            /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
            /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
            /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
            /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

            /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

            /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
            /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
        };

        std::string EncodeURI(std::string str) {
            const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
            const unsigned char * pSrc = (const unsigned char *) str.c_str();
            const int SRC_LEN = str.length();
            unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
            unsigned char * pEnd = pStart;
            const unsigned char * const SRC_END = pSrc + SRC_LEN;

            for (; pSrc < SRC_END; ++pSrc) {
                if (SAFE[*pSrc]) {
                    *pEnd++ = *pSrc;
                } else {
                    // escape this char
                    *pEnd++ = '%';
                    *pEnd++ = DEC2HEX[*pSrc >> 4];
                    *pEnd++ = DEC2HEX[*pSrc & 0x0F];
                }
            }

            std::string sResult((char *)pStart, (char *)pEnd);
            delete [] pStart;
            return sResult;
        }
	public:
		Emoji() = default;
		Emoji(std::string name, snowflake id);
		Emoji(discpp::Guild guild, snowflake id);
		Emoji(nlohmann::json json);
		Emoji(std::wstring w_unicode);
		Emoji(std::string s_unicode);

        bool operator==(Emoji& other) const {
            auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();

            // If the other's name is empty but mine is not then it must have unicode.
            if (other.name.empty() && !name.empty() && unicode.empty()) {
                return wstr_converter.to_bytes(other.unicode) == name;
            } else if (!other.name.empty() && name.empty() && !unicode.empty()) {
                return wstr_converter.to_bytes(unicode) == other.name;
            } else if (!other.id.empty() && !id.empty() && !other.name.empty() && !name.empty()) {
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

            if (name.empty() && id.empty()) {
                return wstr_converter.to_bytes(unicode);
            } else// if (!name.empty() && id.empty()) {
            {
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

			return EncodeURI(wstr_converter.to_bytes(emoji));
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