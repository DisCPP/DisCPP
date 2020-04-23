#ifndef DISCPP_ATTACHMENT_H
#define DISCPP_ATTACHMENT_H

#include "discord_object.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Attachment : DiscordObject {
	public:
		Attachment() = default;
		Attachment(nlohmann::json json);

		snowflake id;
		std::string filename;
		int size;
		std::string url;
		int height;
		int width;
	};
}

#endif