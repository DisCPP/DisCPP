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

	Emoji::Emoji(const discpp::JsonObject& json) {
		id = json.GetIDSafely("id");
		name = json.Get<std::string>("name");
		if (json.ContainsNotNull("roles")) {
		    json["roles"].IterateThrough([&](const discpp::JsonObject& role_json)->bool {
                roles.emplace_back(Snowflake(role_json.GetString()));
                return true;
		    });
		}
		if (json.ContainsNotNull("user")) {
			creator = std::make_shared<discpp::User>(json["user"]);
		}
		require_colons = json.Get<bool>("require_colons");
        managed = json.Get<bool>("managed");
        animated = json.Get<bool>("animated");
	}

    Emoji::Emoji(const std::string& s_unicode) {
#ifdef WIN32
        wchar_t thick_emoji[MAX_PATH];
        if (!MultiByteToWideChar(CP_UTF8, MB_COMPOSITE, s_unicode.c_str(), -1, thick_emoji, MAX_PATH)) {
            throw std::runtime_error("Failed to convert emoji to string!");
        } else {
            this->unicode = thick_emoji;
        }
#else
        auto converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>();
        this->unicode = converter.from_bytes(s_unicode);
#endif
    }
}