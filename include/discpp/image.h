#ifndef DISCPP_IMAGE_H
#define DISCPP_IMAGE_H

#include <utility>

#include "discord_object.h"



namespace discpp {
	class Image : public DiscordObject {
	public:
		Image() = default;

        /**
         * @brief Constructs a discpp::Image object from the id.
         *
         * You must supply the location of `*file` due to us needing to find out the file extension.
         *
         * ```cpp
         *      discpp::Image image(&file, "test.png");
         * ```
         *
         * @param[in] image The image.
         * @param[in] location The image location.
         *
         * @return discpp::Image, this is a constructor.
         */
		Image(std::ifstream* image, std::string location) : image(image), location(std::move(location)) {}

        /**
         * @brief Converts the image to the Data URI scheme.
         *
         * ```cpp
         *      discpp::Image image(&file, "test.txt");
         *		std::string data_uri = image.ToDataURI();
         * ```
         *
         * @return std::string
         */
		std::string ToDataURI();
	private:
		std::ifstream* image;
		std::string location;
	};
}

#endif