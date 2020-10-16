#ifndef DISCPP_ATTACHMENT_H
#define DISCPP_ATTACHMENT_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <rapidjson/document.h>

#include "discord_object.h"

namespace discpp {
	class Attachment : DiscordObject {
	public:
		Attachment() = default;
        /**
         * @brief Contructs a discpp::Attachment object from json.
         *
         * ```cpp
         *		discpp::Attachment attachment(json);
         * ```
         *
         * @param[in] json The json that makes up this object.
         *
         * @return discord::Attachment, this is a constructor.
         *
         .*/
		Attachment(rapidjson::Document& json);

		[[nodiscard]] std::string GetFormattedTime(CommonTimeFormat format_type = CommonTimeFormat::DEFAULT, const std::string& format_str = "", bool localtime = false) const noexcept {
		    return this->id.GetFormattedTimestamp(format_type, format_str, localtime);
		}
		[[nodiscard]] time_t GetRawTime() const noexcept {
		    return this->id.GetRawTime();
		}

		Snowflake id; /**< id for the current attachment. .*/
		std::string filename; /**< filename for the current attachment. .*/
		int size; /**< size of the current attachment. .*/
		std::string url; /**< url of the current attachment. .*/
		int height; /**< height of the current attachment. .*/
		int width; /**< width of the current attachment. .*/
	};
}

#endif