#include "..\include\message.h"
#include "message.h"
#include "utils.h"

namespace discord {
	Message::Message(snowflake id) : discord::DiscordObject(id) {
	
	}

	Message::Message(nlohmann::json json) {
		id = ToSnowflake(json["id"]);
		channel = discord::Channel(ToSnowflake(json["channel_id"]));
		guild = discord::Guild(GetSnowflakeSafely(json, "guild_id"));
		author = discord::User(json["author"]);
		//member = (GetDataSafely<std::string>(json, "member") != "") ? discord::Member(json["member"]) : discord::Member();
		content = json["content"];
		timestamp = json["timestamp"];
		edited_timestamp = GetDataSafely<std::string>(json, "edited_timestamp");
		tts = json["tts"].get<bool>();
		mention_everyone = json["mention_everyone"].get<bool>();

		for (auto& mention : json["mentions"]) {
			mentions.push_back(discord::Member(mention));
		}

		pinned = json["pinned"].get<bool>();
		webhook_id = GetSnowflakeSafely(json, "webhook_id");
		type = json["type"].get<int>();
		flags = json["flags"].get<int>();
	}

	void Message::AddReaction(discord::Emoji emoji) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.name + ":" + std::to_string(emoji.id) + "/@me");
		nlohmann::json result = SendPutRequest(endpoint, DefaultHeaders(), {});
	}

	void Message::RemoveBotReaction(discord::Emoji emoji) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.name + ":" + std::to_string(emoji.id) + "/@me");
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders());
	}

	void Message::RemoveReaction(discord::User user, discord::Emoji emoji) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.name + ":" + std::to_string(emoji.id) + "/" + std::to_string(user.id));
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders());
	}

	std::vector<discord::User> Message::GetReactants() {

	}
}