#include "attachment.h"
#include "utils.h"

namespace discpp {
	discpp::Attachment::Attachment(rapidjson::Document& json) {
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

		id = SnowflakeFromString(json["id"].GetString());
		filename = json["filename"].GetString();
		size = json["size"].GetInt();
		url = json["url"].GetString();
		height = GetDataSafely<int>(json, "height");
		width = GetDataSafely<int>(json, "width");
	}
}