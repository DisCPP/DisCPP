#include "attachment.h"
#include "utils.h"

namespace discpp {
	discpp::Attachment::Attachment(const discpp::JsonObject& json) {
		id = SnowflakeFromString(json["id"].GetString());
		filename = json["filename"].GetString();
		size = json["size"].GetInt();
		url = json["url"].GetString();
		height = json.Get<int>("height");
		width = json.Get<int>("width");
	}
}