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
		if (title.size() < 0 || title.size() > 256) {
			throw std::runtime_error("Embed title can only be 0-256 characters");
		}
		embed_json["title"] = title;
		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetType(std::string type) {
		embed_json["type"] = type;
		return *this;
	}
	EmbedBuilder& EmbedBuilder::SetDescription(std::string description) {
		if (description.size() < 0 || description.size() > 2048) {
			throw std::runtime_error("Embed descriptions can only be 0-2048 characters");
		}
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
		if (text.size() > 2048) {
			throw std::runtime_error("Embed footer text can only be up to 0-2048 characters");
		}
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
		if (name.size() > 256) {
			throw std::runtime_error("Embed author names can only be up to 0-256 characters");
		}
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
		} else if (embed_json["fields"].size() > 25) {
			throw std::runtime_error("Embeds can only have 25 field objects");
		}

		if (name.size() > 256) {
			throw std::runtime_error("Embed field names can only be up to 0-256 characters");
		}

		if (value.size() > 1024) {
			throw std::runtime_error("Embed field values can only be up to 0-1024 characters");
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