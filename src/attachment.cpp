#include "attachment.h"
#include "utils.h"

namespace discpp {
	discpp::Attachment::Attachment(rapidjson::Document& json) {
		id = SnowflakeFromString(json["id"].GetString());
		filename = json["filename"].GetString();
		size = json["size"].GetInt();
		url = json["url"].GetString();
		height = GetDataSafely<int>(json, "height");
		width = GetDataSafely<int>(json, "width");
	}
}