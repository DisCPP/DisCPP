#ifndef DISCPP_EMBED_BUILDER_H
#define DISCPP_EMBED_BUILDER_H

#include "discord_object.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Color;

	class EmbedBuilder {
	public:
		EmbedBuilder();
		EmbedBuilder(std::string title, std::string description, Color color);
		EmbedBuilder(nlohmann::json json);

		EmbedBuilder& SetTitle(std::string title);
		EmbedBuilder& SetType(std::string type);
		EmbedBuilder& SetDescription(std::string description);
		EmbedBuilder& SetUrl(std::string url);
		EmbedBuilder& SetTimestamp(std::string timestamp);
		EmbedBuilder& SetColor(Color color);
		EmbedBuilder& SetFooter(std::string text, std::string icon_url = "");
		EmbedBuilder& SetImage(std::string url, int height = -1, int width = -1);
		EmbedBuilder& SetThumbnail(std::string url, int height = -1, int width = -1);
		EmbedBuilder& SetVideo(std::string url, int height = -1, int width = -1);
		EmbedBuilder& SetProvider(std::string name, std::string url);
		EmbedBuilder& SetAuthor(std::string name, std::string url = "", std::string icon_url = "");
		EmbedBuilder& AddField(std::string name, std::string value, bool is_inline = false);

		std::string GetDescription();
		std::string GetTitle();
		std::string GetUrl();
		std::string GetTimestamp();
		Color GetColor();
		std::pair<std::string, std::string> GetFooter();
        std::pair<std::string, std::string> GetProvider();

		nlohmann::json ToJson();
		operator nlohmann::json();
		
	private:
		nlohmann::json embed_json;
	};
}

#endif