#include "attachment.h"
#include "utils.h"

namespace discord {
	discord::Attachment::Attachment(nlohmann::json json) {
		/**
		 * @brief Contructs a discord::Attachment object from json.
		 *
		 * ```cpp
		 *		discord::Attachment attachment(json);
		 * ```
		 *
		 * @param[in] json The json that makes up this object.
		 *
		 * @return dsicord::Attachment, this is a constructor.
		 *

		*/

		id = json["id"].get<snowflake>();
		filename = json["filename"].get<snowflake>();
		size = json["size"].get<int>();
		url = json["url"];
		height = GetDataSafely<int>(json, "height");
		width = GetDataSafely<int>(json, "width");
	}
}