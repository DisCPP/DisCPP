#ifndef DISCORDPP_IMAGE_H
#define DISCORDPP_IMAGE_H

#include "discord_object.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Image : DiscordObject {
	public:
		Image() = default;
		Image(std::ifstream* image, std::string location);

		std::string ToDataURI();
	private:
		std::ifstream* image;
		std::string location;
	};
}

#endif