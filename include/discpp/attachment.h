#ifndef DISCPP_ATTACHMENT_H
#define DISCPP_ATTACHMENT_H

#include "discord_object.h"

#include <rapidjson/document.h>

namespace discpp {
	class Attachment : DiscordObject {
	public:
		Attachment() = default;
		Attachment(rapidjson::Document& json);

		snowflake id;
		std::string filename;
		int size;
		std::string url;
		int height;
		int width;
	};
}

#endif