#include "embed_builder.h"
#include "utils.h"

namespace discord {
	EmbedBuilder::EmbedBuilder() : embed_json(nlohmann::json({})) { }

	EmbedBuilder::EmbedBuilder(std::string title, std::string description, int color) {
		SetTitle(title);
		SetDescription(description);
		SetColor(color);
	}

	EmbedBuilder::EmbedBuilder(nlohmann::json json) : embed_json(json) {}

	EmbedBuilder& EmbedBuilder::SetTitle(std::string title) {
		embed_json["title"] = title;
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetType(std::string type) {
		embed_json["type"] = type;
		return *this;
	}
	EmbedBuilder& EmbedBuilder::SetDescription(std::string description) {
		embed_json["description"] = description;
		return *this;
	}
	
	EmbedBuilder& EmbedBuilder::SetUrl(std::string url) {
		embed_json["url"] = url;
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetTimestamp(std::string timestamp) {
		embed_json["timestamp"] = timestamp;
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetColor(int color) {
		embed_json["color"] = color;
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetFooter(std::string text, std::string icon_url) {
		embed_json["footer"] = nlohmann::json({});
		embed_json["footer"]["text"] = text;
		if (!icon_url.empty()) {
			embed_json["footer"]["icon_url"] = icon_url;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetImage(std::string url, int height, int width) {
		embed_json["image"] = nlohmann::json({});
		embed_json["image"]["url"] = url;
		if (height != -1) {
			embed_json["image"]["height"] = height;
		}
		if (width != -1) {
			embed_json["image"]["width"] = width;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetThumbnail(std::string url, int height, int width) {
		embed_json["thumbnail"] = nlohmann::json({});
		embed_json["thumbnail"]["url"] = url;
		if (height != -1) {
			embed_json["thumbnail"]["height"] = height;
		}
		if (width != -1) {
			embed_json["thumbnail"]["width"] = width;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetVideo(std::string url, int height, int width) {
		embed_json["video"] = nlohmann::json({});
		embed_json["video"]["url"] = url;
		if (height != -1) {
			embed_json["video"]["height"] = height;
		}
		if (width != -1) {
			embed_json["video"]["width"] = width;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetProvider(std::string name, std::string url) {
		embed_json["provider"] = nlohmann::json({});
		embed_json["provider"]["name"] = name;
		embed_json["provider"]["url"] = url;
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetAuthor(std::string name, std::string url, std::string icon_url) {
		embed_json["author"] = nlohmann::json({});
		embed_json["author"]["name"] = name;
		if (!url.empty()) {
			embed_json["author"]["url"] = url;
		}
		if (!icon_url.empty()) {
			embed_json["author"]["icon_url"] = icon_url;
		}
		return *this;
	}

	EmbedBuilder& EmbedBuilder::AddField(std::string name, std::string value, bool is_inline) {
		if (!embed_json.contains("fields")) {
			embed_json["fields"] = nlohmann::json::array();
		}
		std::string string_json = Format("{\"name\": \"%\", \"value\": \"%\", \"inline\": %}", name, value, is_inline);
		embed_json["fields"].push_back(nlohmann::json::parse(string_json));
		return *this;
	}

	nlohmann::json EmbedBuilder::ToJson() {
		return embed_json;
	}

	EmbedBuilder::operator nlohmann::json() {
		return embed_json;
	}
}