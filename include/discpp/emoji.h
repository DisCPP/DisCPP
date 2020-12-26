#ifndef DISCPP_EMOJI_H
#define DISCPP_EMOJI_H

#ifdef WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#else
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <codecvt>
#endif

#include <codecvt>

#include "discord_object.h"
#include "utils.h"

#include <locale>
#include <string>
#include <utility>

namespace discpp {
	class Guild;
	class User;

	class Emoji {
	public:
		Emoji() = default;

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
		Emoji(const std::string& name, const Snowflake& id) : name(EscapeString(name)), id(id) {}

        /**
         * @brief Constructs a discpp::Emoji object using a guild object and id.
         *
         * This constructor searches the emoji cache in the guild object to get an emoji object.
         *
         * If you set `can_request` to true, and the emoji is not found in cache, then we will request
         * the emoji from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
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
        Emoji(std::shared_ptr<discpp::Guild> guild, const Snowflake& id, bool can_request = false);

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
        Emoji(discpp::Client* client, rapidjson::Document& json);

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
		Emoji(std::wstring w_unicode) : unicode(std::move(w_unicode)) {}

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
		Emoji(const std::string& s_unicode);

        bool operator==(const Emoji& other) const {
#ifdef WIN32
            // Check if the other emoji doesn't have a name but does have unicode convert it to string and compare to this emoji name.
            if (other.name.empty() && !other.unicode.empty() && !name.empty() && unicode.empty()) {
                char ansi_emoji[MAX_PATH];
                if (!WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, other.unicode.c_str(), -1, ansi_emoji, MAX_PATH, nullptr, nullptr)) {
                    throw std::runtime_error("Failed to convert emoji to string!");
                } else {
                    return std::string(ansi_emoji) == name;
                }
            }
            // Check if this emoji doesn't have a name but does have unicode convert it to string and compare to the other emoji name.
            else if (!other.name.empty() && other.unicode.empty() && name.empty() && !unicode.empty()) {
                char ansi_emoji[MAX_PATH];
                if (!WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, unicode.c_str(), -1, ansi_emoji, MAX_PATH, nullptr, nullptr)) {
                    throw std::runtime_error("Failed to convert emoji to string!");
                } else {
                    return std::string(ansi_emoji) == other.name;
                }
            }
#else
            auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();
            // Check if the other emoji doesn't have a name but does have unicode convert it to string and compare to this emoji name.
            if (other.name.empty() && !other.unicode.empty() && !name.empty() && unicode.empty()) {
                return wstr_converter.to_bytes(other.unicode) == name;
            }
            // Check if this emoji doesn't have a name but does have unicode convert it to string and compare to the other emoji name.
            else if (!other.name.empty() && other.unicode.empty() && name.empty() && !unicode.empty()) {
                return wstr_converter.to_bytes(unicode) == other.name;
            }
#endif

            if (other.id.IsValid() && id.IsValid() && !other.name.empty() && !name.empty()) {
                return other.id == id && other.name == name;
            } else if (!other.name.empty() && !name.empty()) {
                return other.name == name;
            } else if (other.id.IsValid() && id.IsValid()) {
                return other.id == id;
            }

            return false;
        }

        /**
         * @brief Gets a string representation of this emoji for sending through messages.
         *
         * ```cpp
         *      ctx.Send(emoji.ToString() + " Failed to make request!");
         * ```
         *
         * @return std::string
         */
        std::string ToString() {
            if (name.empty() && !id.IsValid()) {
#ifdef WIN32
                char ansi_emoji[MAX_PATH];
                if (!WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, unicode.c_str(), -1, ansi_emoji, MAX_PATH, nullptr, nullptr)) {
                    throw std::runtime_error("Failed to convert emoji to string!");
                } else {
                    return ansi_emoji;
                }
#else
                auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();
                return wstr_converter.to_bytes(unicode);
#endif
            } else {
                return name;
            }
        }

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
		std::string ToURL() {
			std::wstring emoji;
			if (name.empty()) {
				emoji = unicode;
			} else {
                return URIEncode(name + ":" + (std::string) id);
			}
#ifdef WIN32
			char ansi_emoji[MAX_PATH];
			if (!WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, emoji.c_str(), -1, ansi_emoji, MAX_PATH, nullptr, nullptr)) {
			    throw std::runtime_error("Failed to convert emoji to string!");
			} else {
                return URIEncode(ansi_emoji);
			}
#else
            auto wstr_converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();
			return URIEncode(wstr_converter.to_bytes(emoji));
#endif
		}

		discpp::Snowflake id; /**< ID of the current emoji */
		std::string name; /**< Name of the current emoji */
		std::wstring unicode; /**< Unicode representation of the current emoji */
		std::vector<discpp::Snowflake> roles; /**< Roles */
		std::shared_ptr<discpp::User> creator;
		bool require_colons;
		bool managed;
		bool animated; /**< Whether or not the current emoji is animated */
	};
}

#endif