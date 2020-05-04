#include "embed_builder.h"
#include "color.h"
#include "utils.h"

#include <discpp/client.h>

namespace discpp {
	EmbedBuilder::EmbedBuilder() : embed_json(nlohmann::json({})) { }

	EmbedBuilder::EmbedBuilder(std::string title, std::string description, discpp::Color color) {
		/**
		 * @brief Constructs a discpp::EmbedBuilder object with a title, description, and color.
		 *
		 * ```cpp
		 *      discpp::EmbedBuilder embed("Banned players", "This is a list of banned players from this guild", 0xffbb00);
		 * ```
		 *
		 * @param[in] title The title of the embed.
		 * @param[in] description The description of the embed.
		 * @param[in] color The color of the embed.
		 *
		 * @return discpp::EmbedBuilder, this is a constructor.
		 */

		SetTitle(EscapeString(title));
		SetDescription(description);
		SetColor(color);
	}

	EmbedBuilder::EmbedBuilder(nlohmann::json json) : embed_json(json) {
		/**
		 * @brief Constructs a discpp::EmbedBuilder object from json.
		 *
		 * ```cpp
		 *      discpp::EmbedBuilder embed(json);
		 * ```
		 *
		 * @param[in] json The json to parse the embed from.
		 *
		 * @return discpp::EmbedBuilder, this is a constructor.
		 */
	}

	EmbedBuilder& EmbedBuilder::SetTitle(std::string title) {
		/**
		 * @brief Set the title of the embed.
		 *
		 * ```cpp
		 *      embed.SetTitle("Banned players");
		 * ```
		 *
		 * @param[in] title The title of the embed.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		if (title.size() < 0 || title.size() > 256) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed title can only be 0-256 characters!");
			throw std::runtime_error("Embed title can only be 0-256 characters");
		}
		embed_json["title"] = EscapeString(title);
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetType(std::string type) {
		/**
		 * @brief Set the type of the embed.
		 *
		 * ```cpp
		 *      embed.SetType("rich");
		 * ```
		 *
		 * @param[in] type The type of this embed
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["type"] = EscapeString(type);
		return *this;
	}
	EmbedBuilder& EmbedBuilder::SetDescription(std::string description) {
		/**
		 * @brief Set the description of the embed.
		 *
		 * ```cpp
		 *      embed.SetDescription("This is a description");
		 * ```
		 *
		 * @param[in] description The description for the embed.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		if (description.size() < 0 || description.size() > 2048) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed descriptions can only be 0-2048 characters!");
			throw std::runtime_error("Embed descriptions can only be 0-2048 characters!");
		}
		embed_json["description"] = EscapeString(description);
		return *this;
	}
	
	EmbedBuilder& EmbedBuilder::SetUrl(std::string url) {
		/**
		 * @brief Sets the URL of the embed.
		 *
		 * ```cpp
		 *      embed.SetUrl("https://www.google.com/");
		 * ```
		 *
		 * @param[in] url The URL of the embed.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["url"] = EscapeString(url);
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetTimestamp(std::string timestamp) {
		/**
		 * @brief Set the timestamp for the embed.
		 *
		 * ```cpp
		 *      embed.SetTimestamp("2020-01-20 15:48");
		 * ```
		 *
		 * @param[in] timestamp The timestamp of the embed.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["timestamp"] = timestamp;
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetColor(Color color) {
		/**
		 * @brief Set the color of the embed.
		 *
		 * ```cpp
		 *      embed.SetColor(0xffbb00);
		 * ```
		 *
		 * @param[in] color The color to set the embeds to.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["color"] = color.color_hex;
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetFooter(std::string text, std::string icon_url) {
		/**
		 * @brief Set the footer of the embed.
		 *
		 * ```cpp
		 *      embed.SetFooter("This is a footer", "https://upload.wikimedia.org/wikipedia/commons/thumb/c/c7/Bing_logo_%282016%29.svg/640px-Bing_logo_%282016%29.svg.png");
		 * ```
		 *
		 * @param[in] text The text of the footer.
		 * @param[in] icon_url The icon url.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["footer"] = nlohmann::json({});
		if (text.size() > 2048) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed footer text can only be up to 0-2048 characters!");
			throw std::runtime_error("Embed footer text can only be up to 0-2048 characters!");
		}
		embed_json["footer"]["text"] = text;
		if (!icon_url.empty()) {
			embed_json["footer"]["icon_url"] = icon_url;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetImage(std::string url, int height, int width) {
		/**
		 * @brief Set the image of the embed.
		 *
		 * ```cpp
		 *      embed.SetImage("https://upload.wikimedia.org/wikipedia/commons/thumb/c/c7/Bing_logo_%282016%29.svg/640px-Bing_logo_%282016%29.svg.png", 640, 259);
		 * ```
		 *
		 * @param[in] url The URL of the image.
		 * @param[in] height The height of the image.
		 * @param[in] widgth The width of the image.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["image"] = nlohmann::json({});
		embed_json["image"]["url"] = EscapeString(url);
		if (height != -1) {
			embed_json["image"]["height"] = height;
		}
		if (width != -1) {
			embed_json["image"]["width"] = width;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetThumbnail(std::string url, int height, int width) {
		/**
		 * @brief Set the thumbnail of the embed.
		 *
		 * ```cpp
		 *      embed.SetThumbnail("url", 500, 500);
		 * ```
		 *
		 * @param[in] url The URL for the thumbnail.
		 * @param[in] height The height of the thumbnail.
		 * @param[in] width The width of the thumbnail.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["thumbnail"] = nlohmann::json({});
		embed_json["thumbnail"]["url"] = EscapeString(url);
		if (height != -1) {
			embed_json["thumbnail"]["height"] = height;
		}
		if (width != -1) {
			embed_json["thumbnail"]["width"] = width;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetVideo(std::string url, int height, int width) {
		/**
		 * @brief Set the video of the embed.
		 *
		 * ```cpp
		 *      embed.SetVideo("url", 500, 500);
		 * ```
		 *
		 * @param[in] url The video URL.
		 * @param[in] height The height of the video.
		 * @param[in] width The width of the video.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["video"] = nlohmann::json({});
		embed_json["video"]["url"] = EscapeString(url);
		if (height != -1) {
			embed_json["video"]["height"] = height;
		}
		if (width != -1) {
			embed_json["video"]["width"] = width;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetProvider(std::string name, std::string url) {
		/**
		 * @brief Set the provider of the embed.
		 *
		 * ```cpp
		 *      embed.SetProvider("Google", "https://www.google.com/");
		 * ```
		 *
		 * @param[in] name The name of the provider of the embed.
		 * @param[in] url The URL of the provider.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["provider"] = nlohmann::json({});
		embed_json["provider"]["name"] = EscapeString(name);
		embed_json["provider"]["url"] = EscapeString(url);
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetAuthor(std::string name, std::string url, std::string icon_url) {
		/**
		 * @brief Set the author of the embed.
		 *
		 * ```cpp
		 *      embed.SetAuthor("SeanOMik", url, icon_url);
		 * ```
		 *
		 * @param[in] name The name of the author.
		 * @param[in] url The URL of the author.
		 * @param[in] icon_url The URL of the icon.
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		embed_json["author"] = nlohmann::json({});
		if (name.size() > 256) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed author names can only be up to 0-256 characters!");
			throw std::runtime_error("Embed author names can only be up to 0-256 characters");
		}
		embed_json["author"]["name"] = EscapeString(name);

		if (!url.empty()) {
			embed_json["author"]["url"] = EscapeString(url);
		}
		if (!icon_url.empty()) {
			embed_json["author"]["icon_url"] = EscapeString(icon_url);
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::AddField(std::string name, std::string value, bool is_inline) {
		/**
		 * @brief Add a field to the embed.
		 *
		 * ```cpp
		 *      embed.AddField("This is a field title", "This is a big long description", false);
		 * ```
		 *
		 * @param[in] name The name/title of the field.
		 * @param[in] value The value/description of the field
		 *
		 * @return discpp::EmbedBuilder, just returns an object of this.
		 */

		if (name.empty()) {
			globals::client_instance->logger->Error(LogTextColor::RED + "You can not have an empty or null field name!");
			throw std::runtime_error("You can not have an empty or null field title!");
		} else if (value.empty()) {
			globals::client_instance->logger->Error(LogTextColor::RED + "You can not have an empty or null field value!");
			throw std::runtime_error("You can not have an empty or null field value!");
		}

		if (!embed_json.contains("fields")) {
			embed_json["fields"] = nlohmann::json::array();
		} else if (embed_json["fields"].size() > 25) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embeds can only have 25 field objects!");
			throw std::runtime_error("Embeds can only have 25 field objects!");
		}

		if (name.size() > 256) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed field names can only be up to 1-256 characters!");
			throw std::runtime_error("Embed field names can only be up to 1-256 characters!");
		}

		if (value.size() > 1024) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed field values can only be up to 1-1024 characters!");
			throw std::runtime_error("Embed field values can only be up to 1-1024 characters!");
		}

		nlohmann::json field = nlohmann::json({
				{"name", EscapeString(name)},
				{"value", EscapeString(value)},
				{"inline", is_inline}
			});
		embed_json["fields"].push_back(field);
		return *this;
	}

	nlohmann::json EmbedBuilder::ToJson() {
		/**
		 * @brief Convert the embed to json.
		 *
		 * ```cpp
		 *      embed.ToJson();
		 * ```
		 *
		 * @return nlohmann::json
		 */

		return embed_json;
	}

	EmbedBuilder::operator nlohmann::json() {
		return embed_json;
	}

    std::string EmbedBuilder::GetDescription() {
        return embed_json["description"];
    }

    std::string EmbedBuilder::GetTitle() {
        return embed_json["title"];
    }

    std::string EmbedBuilder::GetUrl() {
        return embed_json["url"];
    }

    std::string EmbedBuilder::GetTimestamp() {
        return embed_json["timestamp"];
    }

    Color EmbedBuilder::GetColor() {
        return Color();
    }

    std::pair<std::string, std::string> EmbedBuilder::GetFooter() {
        return std::make_pair<std::string, std::string>(embed_json["footer"]["text"], embed_json["footer"]["icon_url"]);
    }

    std::pair<std::string, std::string> EmbedBuilder::GetProvider() {
        return std::make_pair<std::string, std::string>(embed_json["provider"]["name"], embed_json["provider"]["url"]);
    }
}