#include <discpp/log.h>
#include "image.h"
#include "utils.h"
#include "client.h"

namespace discpp {
	std::string GetFileExtension(std::string file_name) {
		return file_name.substr(file_name.find_last_of('.') + 1);
	}

	Image::Image(std::ifstream* image, std::string location) : image(image), location(location) {
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
	}

	std::string Image::ToDataURI() {
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

		if (image->is_open()) {
			std::stringstream buffer;
			buffer << image->rdbuf();

			std::string ext = GetFileExtension(location);
			std::string data_uri_ext = "";
			if (ext == "jpg" || ext == "jpeg") {
				data_uri_ext = "jpeg";
			} else if (ext == "png") {
				data_uri_ext = "png";
			} else if (ext == "gif") {
				data_uri_ext = "gif";
			} else {
				globals::client_instance->logger->Error(LogTextColor::RED + "The file extension, \"" + ext + "\" is not supported by Discord!");
				throw std::runtime_error("The file extension, \"" + ext + "\" is not supported by Discord!");
			}

			return "data:image/" + data_uri_ext + ";base64," + Base64Encode(buffer.str());
		} else {
			globals::client_instance->logger->Error(LogTextColor::RED + "Failed to open image!");
			throw std::runtime_error("Failed to open image!");
		}
	}
}