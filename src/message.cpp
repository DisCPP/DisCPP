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
		content = json["content"];
		timestamp = json["timestamp"];
		edited_timestamp = GetDataSafely<std::string>(json, "edited_timestamp");
		tts = json["tts"].get<bool>();
		mention_everyone = json["mention_everyone"].get<bool>();
		for (auto& mention : json["mentions"]) {
			mentions.push_back(discord::Member(mention));
		}
		for (auto& mentioned_role : json["mentioned_roles"]) {
			mentioned_roles.push_back(discord::Role(mentioned_role));
		}
		if (json.contains("mention_channels")) {
			for (auto& mention_channel : json["mention_channels"]) {
				mention_channels.push_back(discord::Channel(mention_channel));
			}
		}
		// std::vector<discord::Attachment> attachments;
		// std::vector<discord::Embed> embeds;
		if (json.contains("reactions")) {
			for (auto& reaction : json["reactions"]) {
				reactions.push_back(discord::Reaction(reaction));
			}
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

	std::vector<discord::User> Message::GetReactorsOfEmoji(discord::Emoji emoji, int amount) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.name + ":" + std::to_string(emoji.id));
		cpr::Body body("{\"limit\": " + std::to_string(amount) + "}");
		nlohmann::json result = SendGetRequest(endpoint, DefaultHeaders(), {}, body);
		
		std::vector<discord::User> users;
		for (auto& user : result) {
			users.push_back(discord::User(user));
		}

		return users;
	}

	std::vector<discord::User> Message::GetReactorsOfEmoji(discord::Emoji emoji, discord::User user, GetReactionsMethod method) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.name + ":" + std::to_string(emoji.id));
		std::string method_str = (method == GetReactionsMethod::BEFORE_USER) ? "before" : "after";
		cpr::Body body("{\"" + method_str + "\": " + std::to_string(user.id) + "}");
		nlohmann::json result = SendGetRequest(endpoint, DefaultHeaders(), {}, body);
		
		std::vector<discord::User> users;
		for (auto& user : result) {
			users.push_back(discord::User(user));
		}

		return users;
	}

	void Message::ClearReactions() {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions");
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders());
	}

	discord::Message Message::EditMessage(std::string text) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id));
		cpr::Body body("{\"content\": \"" + text + "\"}");
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), body);

		std::cout << result << std::endl;

		discord::Message message(result);

		*this = message;
		return *this;
	}

	void Message::DeleteMessage() {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id));
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders());
		
		*this = discord::Message();
	}

	void Message::PinMessage() {
		nlohmann::json result = SendPutRequest(Endpoint("/channels/" + std::to_string(channel.id) + "/pins/" + std::to_string(id)), DefaultHeaders(), {});
	}

	void Message::UnpinMessage() {
		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/" + std::to_string(channel.id) + "/pins/" + std::to_string(id)), DefaultHeaders());
	}
}