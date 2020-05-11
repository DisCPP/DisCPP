#include "embed_builder.h"
#include "color.h"
#include "utils.h"

#include <discpp/client.h>

namespace discpp {
	EmbedBuilder::EmbedBuilder() {
	    //embed_json = std::move(rapidjson::Document(rapidjson::kObjectType));
	    rapidjson::Document doc(rapidjson::kObjectType);
	    embed_json = std::make_shared<rapidjson::Document>(std::move(doc));
	}

	EmbedBuilder::EmbedBuilder(std::string title, std::string description, discpp::Color color) : EmbedBuilder() {
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

	EmbedBuilder::EmbedBuilder(rapidjson::Document& json) {
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

        rapidjson::Document doc(rapidjson::kObjectType);
        embed_json = std::make_shared<rapidjson::Document>(std::move(doc));
		embed_json->CopyFrom(json, embed_json->GetAllocator());
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
		auto& allocator = embed_json->GetAllocator();

		embed_json->AddMember("title", EscapeString(title), allocator);

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

        embed_json->AddMember("type", EscapeString(type), embed_json->GetAllocator());

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

        embed_json->AddMember("description", EscapeString(description), embed_json->GetAllocator());

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

        embed_json->AddMember("url", EscapeString(url), embed_json->GetAllocator());

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

        embed_json->AddMember("timestamp", EscapeString(timestamp), embed_json->GetAllocator());

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

        embed_json->AddMember("color", color.color_hex, embed_json->GetAllocator());

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

		if (text.size() > 2048) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed footer text can only be up to 0-2048 characters!");
			throw std::runtime_error("Embed footer text can only be up to 0-2048 characters!");
		}

		rapidjson::Value footer(rapidjson::kObjectType);
		footer.AddMember("text", text, embed_json->GetAllocator());
		if (!icon_url.empty()) {
            footer.AddMember("icon_url", icon_url, embed_json->GetAllocator());
		}

		embed_json->AddMember("footer", footer, embed_json->GetAllocator());

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

        rapidjson::Value image(rapidjson::kObjectType);
        image.AddMember("url", url, embed_json->GetAllocator());
		if (height != -1) {
            image.AddMember("height", height, embed_json->GetAllocator());
		}
		if (width != -1) {
            image.AddMember("width", width, embed_json->GetAllocator());
		}
		embed_json->AddMember("image", image, embed_json->GetAllocator());

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

		rapidjson::Value thumbnail(rapidjson::kObjectType);
        thumbnail.AddMember("url", url, embed_json->GetAllocator());
        if (height != -1) {
            thumbnail.AddMember("height", height, embed_json->GetAllocator());
        }
        if (width != -1) {
            thumbnail.AddMember("width", width, embed_json->GetAllocator());
        }
        embed_json->AddMember("thumbnail", thumbnail, embed_json->GetAllocator());

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

        rapidjson::Value video(rapidjson::kObjectType);
        video.AddMember("url", url, embed_json->GetAllocator());
        if (height != -1) {
            video.AddMember("height", height, embed_json->GetAllocator());
        }
        if (width != -1) {
            video.AddMember("width", width, embed_json->GetAllocator());
        }
        embed_json->AddMember("video", video, embed_json->GetAllocator());

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

        rapidjson::Value provider(rapidjson::kObjectType);
        provider.AddMember("name", name, embed_json->GetAllocator());
        provider.AddMember("url", url, embed_json->GetAllocator());

        embed_json->AddMember("provider", provider, embed_json->GetAllocator());

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

		if (name.size() > 256) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed author names can only be up to 0-256 characters!");
			throw std::runtime_error("Embed author names can only be up to 0-256 characters");
		}

        rapidjson::Value author(rapidjson::kObjectType);
        author.AddMember("name", EscapeString(name), embed_json->GetAllocator());

		if (!url.empty()) {
            author.AddMember("url", url, embed_json->GetAllocator());
		}
		if (!icon_url.empty()) {
            author.AddMember("icon_url", icon_url, embed_json->GetAllocator());
		}
        embed_json->AddMember("author", author, embed_json->GetAllocator());

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

		if (ContainsNotNull(*embed_json, "fields")) {
            rapidjson::Value& fields = (*embed_json)["fields"];

		    if (fields.Size() > 25) {
		        globals::client_instance->logger->Error(LogTextColor::RED + "Embeds can only have 25 field objects!");
		        throw std::runtime_error("Embeds can only have 25 field objects!");
		    }
		} else {
		    rapidjson::Value fields(rapidjson::kArrayType);
            embed_json->AddMember("fields", fields, embed_json->GetAllocator());
		}

		if (name.size() > 256) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed field names can only be up to 1-256 characters!");
			throw std::runtime_error("Embed field names can only be up to 1-256 characters!");
		}

		if (value.size() > 1024) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed field values can only be up to 1-1024 characters!");
			throw std::runtime_error("Embed field values can only be up to 1-1024 characters!");
		}

        rapidjson::Value field(rapidjson::kObjectType);
		field.AddMember("name", EscapeString(name), embed_json->GetAllocator());
        field.AddMember("value", EscapeString(value), embed_json->GetAllocator());
        field.AddMember("inline", is_inline, embed_json->GetAllocator());

        (*embed_json)["fields"].GetArray().PushBack(field, embed_json->GetAllocator());

		return *this;
	}

    rapidjson::Document EmbedBuilder::ToJson() {
		/**
		 * @brief Convert the embed to json.
		 *
		 * ``cpp
		 *      embed.ToJson();
		 * ```
		 *
		 * @return rapidjson::Document
		 */


		return std::move(*embed_json);
	}

    std::string EmbedBuilder::GetDescription() {
        return (*embed_json)["description"].GetString();
    }

    std::string EmbedBuilder::GetTitle() {
        return (*embed_json)["title"].GetString();
    }

    std::string EmbedBuilder::GetUrl() {
        return (*embed_json)["url"].GetString();
    }

    std::string EmbedBuilder::GetTimestamp() {
        return (*embed_json)["timestamp"].GetString();
    }

    Color EmbedBuilder::GetColor() {
        return Color((*embed_json)["color"].GetInt());
    }

    std::pair<std::string, std::string> EmbedBuilder::GetFooter() {
        return std::make_pair<std::string, std::string>((*embed_json)["footer"]["text"].GetString(), (*embed_json)["footer"]["icon_url"].GetString());
    }

    std::pair<std::string, std::string> EmbedBuilder::GetProvider() {
        return std::make_pair<std::string, std::string>((*embed_json)["provider"]["name"].GetString(), (*embed_json)["provider"]["url"].GetString());
    }
}