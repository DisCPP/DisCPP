#include "emoji.h"
#include "guild.h"
#include "user.h"

namespace discpp {
	Emoji::Emoji(const discpp::Guild& guild, const Snowflake& id) : id(id) {
		auto it = guild.emojis.find(id);
		if (it != guild.emojis.end()) {
			*this = it->second;
		}
	}

	Emoji::Emoji(rapidjson::Document& json) {
		id = GetIDSafely(json, "id");
		name = GetDataSafely<std::string>(json, "name");
		if (ContainsNotNull(json, "roles")) {
			for (auto& role : json["roles"].GetArray()) {
				rapidjson::Document role_json;
				role_json.CopyFrom(role, role_json.GetAllocator());
				roles.emplace_back(SnowflakeFromString(role.GetString()));
			}
		}
		if (ContainsNotNull(json, "user")) {
			rapidjson::Document user_json;
			user_json.CopyFrom(json["user"], user_json.GetAllocator());
			creator = std::make_shared<discpp::User>(discpp::User(user_json));
		}
		require_colons = GetDataSafely<bool>(json, "require_colons");
        managed = GetDataSafely<bool>(json, "managed");
        animated = GetDataSafely<bool>(json, "animated");
	}

    Emoji::Emoji(const std::string& s_unicode) {
#ifdef WIN32
        wchar_t thick_emoji[MAX_PATH];
        if (!MultiByteToWideChar(CP_UTF8, MB_COMPOSITE, s_unicode.c_str(), -1, thick_emoji, MAX_PATH)) {
            throw std::runtime_error("Failed to convert emoji to string!");
        } else {
            this->unicode = thick_emoji;
        }*/
#else
        auto converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();
        this->unicode = converter.from_bytes(s_unicode);
#endif
    }
}