#include "message.h"
#include "utils.h"

namespace discord {
	Message::Message(snowflake id) : discord::DiscordObject(id) {
	
	}

	Message::Message(nlohmann::json json) {
		id = json["id"].get<snowflake>();
		channel = discord::Channel(json["channel_id"].get<snowflake>());
		if (json.contains("guild_id")) {
			guild = discord::Guild(json["guild_id"].get<snowflake>());
		}
		author = (json.contains("author")) ? discord::User(json["author"]) : discord::User();
		content = GetDataSafely<std::string>(json, "content");
		timestamp = GetDataSafely<std::string>(json, "timestamp");
		edited_timestamp = GetDataSafely<std::string>(json, "edited_timestamp");
		tts = GetDataSafely<bool>(json, "tts");
		mention_everyone = GetDataSafely<bool>(json, "mention_everyone");
		for (auto& mention : json["mentions"]) { // This has a weird layout, thats why theres so much json stuff. 
												 // The API docs says this type is an, "array of user objects, with an additional partial member field"
			nlohmann::json new_member_json = {
				{"user", {
					{"id", mention["id"]},
					{"username", mention["username"]},
					{"discriminator", mention["discriminator"]},
					{"avatar", mention["avatar"]}}
				},
				{"deaf", mention["member"]["deaf"]},
				{"hoisted_role", mention["member"]["hoisted_role"]},
				{"joined_at", mention["member"]["joined_at"]},
				{"mute", mention["member"]["mute"]},
				{"roles", mention["member"]["roles"]}
			};

			mentions.push_back(discord::Member(new_member_json));
		}
		for (auto& mentioned_role : json["mentioned_roles"]) {
			mentioned_roles.push_back(mentioned_role.get<snowflake>());
		}
		if (json.contains("mention_channels")) {
			for (auto& mention_channel : json["mention_channels"]) {
				mention_channels.push_back(discord::Channel(mention_channel["id"].get<snowflake>()));
			}
		}
		if (json.contains("attachments")) {
			for (auto& attachment : json["attachments"]) {
				attachments.push_back(discord::Attachment(attachment));
			}
		}
		if (json.contains("embeds")) {
			for (auto& embed : json["embeds"]) {
				embeds.push_back(discord::EmbedBuilder(embed));
			}
		}
		if (json.contains("reactions")) {
			for (auto& reaction : json["reactions"]) {
				reactions.push_back(discord::Reaction(reaction));
			}
		}
		pinned = GetDataSafely<bool>(json, "pinned");
		webhook_id = GetDataSafely<snowflake>(json, "webhook_id");
		type = GetDataSafely<int>(json, "type");
		if (json.contains("activity")) {
			activity = discord::MessageActivity(json["activity"]);
		}
		if (json.contains("application")) {
			application = discord::MessageApplication(json["application"]);
		}
		if (json.contains("message_reference")) {
			message_reference = discord::MessageReference(json["message_reference"]);
		}
		flags = GetDataSafely<int>(json, "flags");
	}

	void Message::AddReaction(discord::Emoji emoji) {
		std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%:%/@me", channel.id, id, emoji.name, emoji.id);
		nlohmann::json result = SendPutRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void Message::RemoveBotReaction(discord::Emoji emoji) {
		std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%:%/@me", channel.id, id, emoji.name, emoji.id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void Message::RemoveReaction(discord::User user, discord::Emoji emoji) {
		std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%:%/%", channel.id, id, emoji.name, emoji.id, user.id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	std::vector<discord::User> Message::GetReactorsOfEmoji(discord::Emoji emoji, int amount) {
		std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%:%", channel.id, id, emoji.name, emoji.id);
		cpr::Body body("{\"limit\": " + std::to_string(amount) + "}");
		nlohmann::json result = SendGetRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL, body);
		
		std::vector<discord::User> users;
		for (auto& user : result) {
			users.push_back(discord::User(user));
		}

		return users;
	}

	std::vector<discord::User> Message::GetReactorsOfEmoji(discord::Emoji emoji, discord::User user, GetReactionsMethod method) {
		std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%:%", channel.id, id, emoji.name, emoji.id);
		std::string method_str = (method == GetReactionsMethod::BEFORE_USER) ? "before" : "after";
		cpr::Body body("{\"" + method_str + "\": " + user.id + "}");
		nlohmann::json result = SendGetRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL, body);
		
		std::vector<discord::User> users;
		for (auto& user : result) {
			users.push_back(discord::User(user));
		}

		return users;
	}

	void Message::ClearReactions() {
		std::string endpoint = Endpoint("/channels/%/messages/%/reactions", channel.id, id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	discord::Message Message::EditMessage(std::string text) {
		std::string endpoint = Endpoint("/channels/%/messages/%", channel.id, id);
		cpr::Body body("{\"content\": \"" + text + "\"}");
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL);

		*this = { result };
		return *this;
	}

	discord::Message Message::EditMessage(discord::EmbedBuilder embed) {
		std::string endpoint = Endpoint("/channels/%/messages/%", channel.id, id);
		cpr::Body body(Format("{\"embed\": %}", embed.ToJson()));
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		*this = { result };
		return *this;
	}

	discord::Message Message::EditMessage(int flags) {
		std::string endpoint = Endpoint("/channels/%/messages/%", channel.id, id);
		cpr::Body body(Format("{\"flags\": %}", flags));
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		*this = { result };
		return *this;
	}

	void Message::DeleteMessage() {
		std::string endpoint = Endpoint("/channels/%/messages/%", channel.id, id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		
		*this = discord::Message();
	}

	void Message::PinMessage() {
		nlohmann::json result = SendPutRequest(Endpoint("/channels/%/pins/%", channel.id, id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void Message::UnpinMessage() {
		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/%/pins/%", channel.id, id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}