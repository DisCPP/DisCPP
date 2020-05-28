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

	EmbedBuilder::EmbedBuilder(const std::string& title, const std::string& description, const discpp::Color& color) : EmbedBuilder() {
		SetTitle(EscapeString(title));
		SetDescription(description);
		SetColor(color);
	}

	EmbedBuilder::EmbedBuilder(rapidjson::Document& json) {
        rapidjson::Document doc(rapidjson::kObjectType);
        embed_json = std::make_shared<rapidjson::Document>(std::move(doc));
		embed_json->CopyFrom(json, embed_json->GetAllocator());
	}

	EmbedBuilder& EmbedBuilder::SetTitle(const std::string& title) {
		if (title.size() < 0 || title.size() > 256) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed title can only be 0-256 characters!");
			throw std::runtime_error("Embed title can only be 0-256 characters");
		}
		auto& allocator = embed_json->GetAllocator();

		if (ContainsNotNull(*embed_json, "title")) {
            (*embed_json)["title"].SetNull();
            (*embed_json)["title"].SetString(EscapeString(title).c_str(), embed_json->GetAllocator());
		} else {
		    embed_json->AddMember("title", EscapeString(title), embed_json->GetAllocator());
		}


		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetType(const std::string& type) {
        if (ContainsNotNull(*embed_json, "type")) {
            (*embed_json)["type"].SetNull();
            (*embed_json)["type"].SetString(EscapeString(type).c_str(), embed_json->GetAllocator());
        } else {
            embed_json->AddMember("type", EscapeString(type), embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetDescription(const std::string& description) {
		if (description.size() < 0 || description.size() > 2048) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed descriptions can only be 0-2048 characters!");
			throw std::runtime_error("Embed descriptions can only be 0-2048 characters!");
		}

		if (ContainsNotNull(*embed_json, "description")) {
            (*embed_json)["description"].SetNull();
            (*embed_json)["description"].SetString(EscapeString(description).c_str(), embed_json->GetAllocator());
        } else {
            embed_json->AddMember("description", EscapeString(description), embed_json->GetAllocator());
        }

		return *this;
	}
	
	EmbedBuilder& EmbedBuilder::SetUrl(const std::string& url) {
        if (ContainsNotNull(*embed_json, "url")) {
            (*embed_json)["url"].SetNull();
            (*embed_json)["url"].SetString(url.c_str(), embed_json->GetAllocator());
        } else {
            embed_json->AddMember("url", url, embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetTimestamp(const std::string& timestamp) {

        if (ContainsNotNull(*embed_json, "timestamp")) {
            (*embed_json)["timestamp"].SetNull();
            (*embed_json)["timestamp"].SetString(EscapeString(timestamp).c_str(), embed_json->GetAllocator());
        } else {
            embed_json->AddMember("timestamp", EscapeString(timestamp), embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetColor(const Color& color) {
        if (ContainsNotNull(*embed_json, "timestamp")) {
            (*embed_json)["color"].SetNull();
            (*embed_json)["color"].SetInt(color.color_hex);
        } else {
            embed_json->AddMember("color", color.color_hex, embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetFooter(const std::string& text, const std::string& icon_url) {
		if (text.size() > 2048) {
			globals::client_instance->logger->Error(LogTextColor::RED + "Embed footer text can only be up to 0-2048 characters!");
			throw std::runtime_error("Embed footer text can only be up to 0-2048 characters!");
		}

		rapidjson::Document footer(rapidjson::kObjectType);
		footer.AddMember("text", text, embed_json->GetAllocator());
		if (!icon_url.empty()) {
            footer.AddMember("icon_url", icon_url, embed_json->GetAllocator());
		}

        if (ContainsNotNull(*embed_json, "timestamp")) {
            (*embed_json)["footer"].SetNull();
            (*embed_json)["footer"].CopyFrom(footer, embed_json->GetAllocator());
        } else {
            embed_json->AddMember("footer", footer, embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetImage(const std::string& url, const int& height, const int& width) {
        rapidjson::Value image(rapidjson::kObjectType);
        image.AddMember("url", url, embed_json->GetAllocator());
		if (height != -1) {
            image.AddMember("height", height, embed_json->GetAllocator());
		}
		if (width != -1) {
            image.AddMember("width", width, embed_json->GetAllocator());
		}

        if (ContainsNotNull(*embed_json, "image")) {
            (*embed_json)["image"].SetNull();
            (*embed_json)["image"].CopyFrom(image, embed_json->GetAllocator());
        } else {
            embed_json->AddMember("image", image, embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetThumbnail(const std::string& url, const int& height, const int& width) {

		rapidjson::Value thumbnail(rapidjson::kObjectType);
        thumbnail.AddMember("url", url, embed_json->GetAllocator());
        if (height != -1) {
            thumbnail.AddMember("height", height, embed_json->GetAllocator());
        }
        if (width != -1) {
            thumbnail.AddMember("width", width, embed_json->GetAllocator());
        }

        if (ContainsNotNull(*embed_json, "thumbnail")) {
            (*embed_json)["thumbnail"].SetNull();
            (*embed_json)["thumbnail"].CopyFrom(thumbnail, embed_json->GetAllocator());
        } else {
            embed_json->AddMember("thumbnail", thumbnail, embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetVideo(const std::string& url, const int& height, const int& width) {
        rapidjson::Value video(rapidjson::kObjectType);
        video.AddMember("url", url, embed_json->GetAllocator());
        if (height != -1) {
            video.AddMember("height", height, embed_json->GetAllocator());
        }
        if (width != -1) {
            video.AddMember("width", width, embed_json->GetAllocator());
        }

        if (ContainsNotNull(*embed_json, "video")) {
            (*embed_json)["video"].SetNull();
            (*embed_json)["video"].CopyFrom(video, embed_json->GetAllocator());
        } else {
            embed_json->AddMember("video", video, embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetProvider(const std::string& name, const std::string& url) {
        rapidjson::Value provider(rapidjson::kObjectType);
        provider.AddMember("name", name, embed_json->GetAllocator());
        provider.AddMember("url", url, embed_json->GetAllocator());

        if (ContainsNotNull(*embed_json, "provider")) {
            (*embed_json)["provider"].SetNull();
            (*embed_json)["provider"].CopyFrom(provider, embed_json->GetAllocator());
        } else {
            embed_json->AddMember("provider", provider, embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::SetAuthor(const std::string& name, const std::string& url, const std::string& icon_url) {
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

        if (ContainsNotNull(*embed_json, "author")) {
            (*embed_json)["author"].SetNull();
            (*embed_json)["author"].CopyFrom(author, embed_json->GetAllocator());
        } else {
            embed_json->AddMember("author", author, embed_json->GetAllocator());
        }

		return *this;
	}

	EmbedBuilder& EmbedBuilder::AddField(const std::string& name, const std::string& value, const bool& is_inline) {
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