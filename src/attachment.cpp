#include "attachment.h"
#include "utils.h"

namespace discord {
	discord::Attachment::Attachment(nlohmann::json json) {
		id = json["id"].get<snowflake>();
		filename = json["filename"].get<snowflake>();
		size = json["size"].get<int>();
		url = json["url"];
		height = GetDataSafely<int>(json, "height");
		width = GetDataSafely<int>(json, "width");
	}
}