#include "message.h"
#include "client.h"

namespace discpp {
	Message::Message(snowflake id) : discpp::DiscordObject(id) {
		/**
		 * @brief Constructs a discpp::Message object from an id.
		 *
		 * ```cpp
		 *      discpp::Message message(583251190591258624);
		 * ```
		 *
		 * @param[in] id The id of the message.
		 *
		 * @return discpp::Member, this is a constructor.
		 */

		auto message = discpp::globals::client_instance->messages.find(id);

		if (message != discpp::globals::client_instance->messages.end()) {
			*this = message->second;
		}
	}

	Message::Message(nlohmann::json json) {
		/**
		 * @brief Constructs a discpp::Message object by parsing json
		 *
		 * ```cpp
		 *      discpp::Message message(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the message.
		 *
		 * @return discpp::Message, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		channel = (json.contains("channel_id")) ? discpp::Channel(json["channel_id"].get<snowflake>()) : discpp::Channel();
		if (json.contains("guild_id")) {
			guild = discpp::Guild(json["guild_id"].get<snowflake>());
		}
		author = (json.contains("author")) ? discpp::User(json["author"]) : discpp::User();
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

			mentions.push_back(discpp::Member(new_member_json, guild.id));
		}
		for (auto& mentioned_role : json["mentioned_roles"]) {
			mentioned_roles.push_back(discpp::Role(mentioned_role.get<snowflake>()));
		}
		if (json.contains("mention_channels")) {
			for (auto& json_chan : json["mention_channels"]) {
				discpp::GuildChannel channel(json_chan["id"].get<snowflake>());
				channel.type = json_chan["type"].get<ChannelType>();
				channel.guild_id = json_chan["guild_id"].get<snowflake>();
				channel.name = json_chan["name"];

				mention_channels.push_back(channel);
			}
		}
		if (json.contains("attachments")) {
			for (auto& attachment : json["attachments"]) {
				attachments.push_back(discpp::Attachment(attachment));
			}
		}
		if (json.contains("embeds")) {
			for (auto& embed : json["embeds"]) {
				embeds.push_back(discpp::EmbedBuilder(embed));
			}
		}
		if (json.contains("reactions")) {
			for (auto& reaction : json["reactions"]) {
				reactions.push_back(discpp::Reaction(reaction));
			}
		}
		pinned = GetDataSafely<bool>(json, "pinned");
		webhook_id = GetDataSafely<snowflake>(json, "webhook_id");
		type = GetDataSafely<int>(json, "type");
		if (json.contains("activity")) {
			activity = discpp::MessageActivity(json["activity"]);
		}
		if (json.contains("application")) {
			application = discpp::MessageApplication(json["application"]);
		}
		if (json.contains("message_reference")) {
			message_reference = discpp::MessageReference(json["message_reference"]);
		}
		flags = GetDataSafely<int>(json, "flags");
	}

	void Message::AddReaction(discpp::Emoji emoji) {
		/**
		 * @brief Add a reaction to the message.
		 *
		 * ```cpp
		 *      message.AddReaction(emoji);
		 * ```
		 *
		 * @param[in] emoji The emoji to react with.
		 *
		 * @return void
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id + "/reactions/" + emoji.ToURL() + "/@me");
		nlohmann::json result = SendPutRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL);
	}

	void Message::RemoveBotReaction(discpp::Emoji emoji) {
		/**
		 * @brief Remove a bot reaction from the message.
		 *
		 * ```cpp
		 *      message.RemoveBotReaction(emoji);
		 * ```
		 *
		 * @param[in] emoji The emoji to remove a bot reaction of.
		 *
		 * @return void
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id + "/reactions/" + emoji.ToURL() + "/@me");
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL);
	}

	void Message::RemoveReaction(discpp::User user, discpp::Emoji emoji) {
		/**
		 * @brief Removes a user's reaction from the message.
		 *
		 * ```cpp
		 *      message.RemoveReaction(user, emoji);
		 * ```
		 *
		 * @param[in] user The user's reaction to remove.
		 * @param[in] emoji The emoji to remove the reaction to remove.
		 *
		 * @return void
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id + "/reactions/" + emoji.ToURL() + "/" + user.id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL);
	}

	std::vector<discpp::User> Message::GetReactorsOfEmoji(discpp::Emoji emoji, int amount) {
		/**
		 * @brief Get reactors of a specific emoji.
		 *
		 * ```cpp
		 *      std::vector<discpp::User> reactors = message.GetReactorOfEmoji(emoji, 50);
		 * ```
		 *
		 * @param[in] emoji The emoji to get reactors of.
		 * @param[in] amount The amount of users to get.
		 *
		 * @return std::vector<discpp::User>
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id + "/reactions/" + emoji.ToURL());
		cpr::Body body("{\"limit\": " + std::to_string(amount) + "}");
		nlohmann::json result = SendGetRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL, body);
		
		std::vector<discpp::User> users;
		for (auto& user : result) {
			users.push_back(discpp::User(user));
		}

		return users;
	}

	std::vector<discpp::User> Message::GetReactorsOfEmoji(discpp::Emoji emoji, discpp::User user, GetReactionsMethod method) {
		/**
		 * @brief Get reactors of a specific emoji of the specific method.
		 *
		 * ```cpp
		 *      std::vector<discpp::User> reactors = message.GetReactorOfEmoji(emoji, 50, reaction_method);
		 * ```
		 *
		 * @param[in] emoji The emoji to get reactors of.
		 * @param[in] amount The amount of users to get.
		 * @param[in] method The method the users reacted by.
		 *
		 * @return std::vector<discpp::User>
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id + "/reactions/" + emoji.ToURL());
		std::string method_str = (method == GetReactionsMethod::BEFORE_USER) ? "before" : "after";
		cpr::Body body("{\"" + method_str + "\": " + user.id + "}");
		nlohmann::json result = SendGetRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL, body);
		
		std::vector<discpp::User> users;
		for (auto& user : result) {
			users.push_back(discpp::User(user));
		}

		return users;
	}

	void Message::ClearReactions() {
		/**
		 * @brief Clear message reactions.
		 *
		 * ```cpp
		 *      message.ClearReactions();
		 * ```
		 *
		 * @return void
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id + "/reactions");
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL);
	}

	discpp::Message Message::EditMessage(std::string text) {
		/**
		 * @brief Edit the message's text.
		 *
		 * ```cpp
		 *      discpp::Message edited_text_message = message.EditMessage("This is edited text");
		 * ```
		 *
		 * @param[in] text The new message text.
		 *
		 * @return discpp::Message
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id);
		cpr::Body body("{\"content\": \"" + EscapeString(text) + "\"}");
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL);

		*this = { result };
		return *this;
	}

	discpp::Message Message::EditMessage(discpp::EmbedBuilder embed) {
		/**
		 * @brief Edit the message's embed.
		 *
		 * ```cpp
		 *      discpp::Message edited_embed_message = message.EditMessage(new_embed);
		 * ```
		 *
		 * @param[in] embed The new embed.
		 *
		 * @return discpp::Message
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id);
		cpr::Body body("{\"embed\": " + embed.ToJson().dump() + "}");
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		*this = { result };
		return *this;
	}

	discpp::Message Message::EditMessage(int flags) {
		/**
		 * @brief Edit the message's embed.
		 *
		 * ```cpp
		 *      discpp::Message edited_embed_message = message.EditMessage(1 << 0);
		 * ```
		 *
		 * @param[in] flags The message flags.
		 *
		 * @return discpp::Message
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id);
		cpr::Body body("{\"flags\": " + std::to_string(flags) + "}");
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		*this = { result };
		return *this;
	}

	void Message::DeleteMessage() {
		/**
		 * @brief Delete this message.
		 *
		 * ```cpp
		 *      message.DeleteMessage();
		 * ```
		 *
		 * @return void
		 */

		std::string endpoint = Endpoint("/channels/" + channel.id + "/messages/" + id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		
		*this = discpp::Message();
	}

	void Message::PinMessage() {
		/**
		 * @brief Pin the message to the channel.
		 *
		 * ```cpp
		 *      message.PinMessage();
		 * ```
		 *
		 * @return void
		 */

		nlohmann::json result = SendPutRequest(Endpoint("/channels/" + channel.id + "/pins/" + id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void Message::UnpinMessage() {
		/**
		 * @brief Unpin this message.
		 *
		 * ```cpp
		 *      message.UnpinMessage();
		 * ```
		 *
		 * @return void
		 */

		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/" + channel.id + "/pins/" + id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}