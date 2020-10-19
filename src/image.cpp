#include "image.h"
#include "log.h"
#include "utils.h"
#include "client.h"

#include <sstream>

namespace discpp {
	std::string GetFileExtension(const std::string& file_name) {
		return file_name.substr(file_name.find_last_of('.') + 1);
	}

	std::string Image::ToDataURI() {
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
				throw std::runtime_error("The file extension, \"" + ext + "\" is not supported by Discord!");
			}

			return "data:image/" + data_uri_ext + ";base64," + Base64Encode(buffer.str());
		} else {
			throw std::runtime_error("Failed to open image!");
		}
	}
}