#ifndef DISCPP_EMBED_BUILDER_H
#define DISCPP_EMBED_BUILDER_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include "discord_object.h"
#include <memory>
#include <rapidjson/document.h>

namespace discpp {
	class Color;

	class EmbedBuilder {
	public:
		EmbedBuilder();
		EmbedBuilder(const std::string& title, const std::string& description, const Color& color);
		EmbedBuilder(rapidjson::Document& json);

		EmbedBuilder& SetTitle(const std::string& title);
		EmbedBuilder& SetType(const std::string& type);
		EmbedBuilder& SetDescription(const std::string& description);
		EmbedBuilder& SetUrl(const std::string& url);
		EmbedBuilder& SetTimestamp(const std::string& timestamp);
		EmbedBuilder& SetColor(const Color& color);
		EmbedBuilder& SetFooter(const std::string& text, const std::string& icon_url = "");
		EmbedBuilder& SetImage(const std::string& url, const int& height = -1, const int& width = -1);
		EmbedBuilder& SetThumbnail(const std::string& url, const int& height = -1, const int& width = -1);
		EmbedBuilder& SetVideo(const std::string& url, const int& height = -1, const int& width = -1);
		EmbedBuilder& SetProvider(const std::string& name, const std::string& url);
		EmbedBuilder& SetAuthor(const std::string& name, const std::string& url = "", const std::string& icon_url = "");
		EmbedBuilder& AddField(const std::string& name, const std::string& value, const bool& is_inline = false);

		std::string GetDescription();
		std::string GetTitle();
		std::string GetUrl();
		std::string GetTimestamp();
		Color GetColor();
		std::pair<std::string, std::string> GetFooter();
        std::pair<std::string, std::string> GetProvider();

        rapidjson::Document ToJson();
	private:
        std::shared_ptr<rapidjson::Document> embed_json;
	};
}

#endif