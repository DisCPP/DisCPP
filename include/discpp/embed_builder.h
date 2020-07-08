#ifndef DISCPP_EMBED_BUILDER_H
#define DISCPP_EMBED_BUILDER_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>

#include <memory>
#include <vector>
#include <string>

namespace discpp {
	class Color;

	class EmbedBuilder {
    friend class Channel;
	public:
		EmbedBuilder();

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
		EmbedBuilder(const std::string& title, const std::string& description, const Color& color);

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
        EmbedBuilder(rapidjson::Document& json);

        EmbedBuilder(const discpp::EmbedBuilder& embed);
        EmbedBuilder operator=(const EmbedBuilder embed) {
            return embed;
        }

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
		EmbedBuilder& SetTitle(const std::string& title);

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
		EmbedBuilder& SetType(const std::string& type);

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
        EmbedBuilder& SetDescription(const std::string& description);

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
        EmbedBuilder& SetUrl(const std::string& url);

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
        EmbedBuilder& SetTimestamp(const std::string& timestamp);

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
        EmbedBuilder& SetColor(const Color& color);

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
        EmbedBuilder& SetFooter(const std::string& text, const std::string& icon_url = "");

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
        EmbedBuilder& SetImage(const std::string& url, const int& height = -1, const int& width = -1);

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
        EmbedBuilder& SetThumbnail(const std::string& url, const int& height = -1, const int& width = -1);

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
        EmbedBuilder& SetVideo(const std::string& url, const int& height = -1, const int& width = -1);

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
        EmbedBuilder& SetProvider(const std::string& name, const std::string& url);

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
        EmbedBuilder& SetAuthor(const std::string& name, const std::string& url = "", const std::string& icon_url = "");

        /**
         * @brief Set fields of the embed.
         *
         * @param[in] Fields Think of it as: `std::vector<std::tuple<title, value, inline>>`.
         *
         * @return void
         */
        void SetFields(std::vector<std::tuple<std::string, std::string, bool>> fields);

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
        EmbedBuilder& AddField(const std::string& name, const std::string& value, const bool is_inline = false);

        /**
         * @brief Get description of the embed.
         *
         * @return std::string
         */
		std::string GetDescription() const;

        /**
         * @brief Get title of the embed.
         *
         * @return std::string
         */
		std::string GetTitle() const;

        /**
         * @brief Get url of the embed.
         *
         * @return std::string
         */
		std::string GetUrl() const;

        /**
         * @brief Get timestamp url of the embed.
         *
         * @return std::string
         */
		std::string GetTimestamp() const;

        /**
         * @brief Get fields of the embed.
         *
         * This is a vector filled with title, then value string pairs so: `std::pair<title, value>`.
         *
         * @return std::vector<std::tuple<std::string, std::string, bool>>. Think of it as: `std::vector<std::tuple<title, value, inline>>`.
         */
        std::vector<std::tuple<std::string, std::string, bool>> GetFields() const;

        /**
         * @brief Get color of the embed.
         *
         * @return discpp::Color
         */
		Color GetColor() const;

        /**
         * @brief Get footer of the embed.
         *
         * @return std::pair<std::string, std::string>. Think of it as: `std::pair<text, url>`.
         */
		std::pair<std::string, std::string> GetFooter() const;

        /**
         * @brief Get provider of the embed.
         *
         * @return std::pair<std::string, std::string>. Think of it as: `std::pair<name, url>`.
         */
        std::pair<std::string, std::string> GetProvider() const;


        /**
         * @brief Convert the embed to json.
         *
         * ``cpp
         *      embed.ToJson();
         * ```
         *
         * @return rapidjson::Document
         */
        std::unique_ptr<rapidjson::Document> ToJson() const;
	private:
        rapidjson::Document embed_json;
	};
}

#endif