#ifndef DISCPP_ATTACHMENT_H
#define DISCPP_ATTACHMENT_H

#include "discord_object.h"

#include <rapidjson/document.h>

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
         */
		Attachment(rapidjson::Document& json);

		Snowflake id; /**< id for the current attachment. */
		std::string filename; /**< filename for the current attachment. */
		int size; /**< size of the current attachment. */
		std::string url; /**< url of the current attachment. */
		int height; /**< height of the current attachment. */
		int width; /**< width of the current attachment. */
	};
}

#endif