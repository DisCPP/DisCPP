#ifndef DISCPP_IMAGE_H
#define DISCPP_IMAGE_H

#include "discord_object.h"



namespace discpp {
	class Image : public DiscordObject {
	public:
		Image() = default;
		Image(std::ifstream* image, const std::string& location);

		std::string ToDataURI();
	private:
		std::ifstream* image;
		std::string location;
	};
}

#endif