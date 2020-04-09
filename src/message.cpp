#include "message.h"
#include "utils.h"

namespace discord {
	Message::Message(snowflake id) : discord::DiscordObject(id) {
		/**
		 * @brief Constructs a discord::Message object from an id.
		 *
		 * This only stores the id and that will be the only thing valid.
		 *
		 * ```cpp
		 *      discord::Message message(583251190591258624);
		 * ```
		 *
		 * @param[in] id The id of the message.
		 *
		 * @return discord::Member, this is a constructor.
		 */
	}

	Message::Message(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::Message object by parsing json
		 *
		 * ```cpp
		 *      discord::Message message(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the message.
		 *
		 * @return discord::Message, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		channel = (json.contains("channel_id")) ? discord::Channel(json["channel_id"].get<snowflake>()) : discord::Channel();
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

			mentions.push_back(discord::Member(new_member_json, guild.id));
		}
		for (auto& mentioned_role : json["mentioned_roles"]) {
			mentioned_roles.push_back(discord::Role(mentioned_role.get<snowflake>()));
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

		std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%:%/@me", channel.id, id, emoji.name, emoji.id);
		nlohmann::json result = SendPutRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void Message::RemoveBotReaction(discord::Emoji emoji) {
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

		std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%:%/@me", channel.id, id, emoji.name, emoji.id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	void Message::RemoveReaction(discord::User user, discord::Emoji emoji) {
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

		std::string endpoint = Endpoint("/channels/%/messages/%/reactions/%:%/%", channel.id, id, emoji.name, emoji.id, user.id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	std::vector<discord::User> Message::GetReactorsOfEmoji(discord::Emoji emoji, int amount) {
		/**
		 * @brief Get reactors of a specific emoji.
		 *
		 * ```cpp
		 *      std::vector<discord::User> reactors = message.GetReactorOfEmoji(emoji, 50);
		 * ```
		 *
		 * @param[in] emoji The emoji to get reactors of.
		 * @param[in] amount The amount of users to get.
		 *
		 * @return std::vector<discord::User>
		 */

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
		/**
		 * @brief Get reactors of a specific emoji of the specific method.
		 *
		 * ```cpp
		 *      std::vector<discord::User> reactors = message.GetReactorOfEmoji(emoji, 50, reaction_method);
		 * ```
		 *
		 * @param[in] emoji The emoji to get reactors of.
		 * @param[in] amount The amount of users to get.
		 * @param[in] method The method the users reacted by.
		 *
		 * @return std::vector<discord::User>
		 */

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
		/**
		 * @brief Clear message reactions.
		 *
		 * ```cpp
		 *      message.ClearReactions();
		 * ```
		 *
		 * @return void
		 */

		std::string endpoint = Endpoint("/channels/%/messages/%/reactions", channel.id, id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	discord::Message Message::EditMessage(std::string text) {
		/**
		 * @brief Edit the message's text.
		 *
		 * ```cpp
		 *      discord::Message edited_text_message = message.EditMessage("This is edited text");
		 * ```
		 *
		 * @param[in] text The new message text.
		 *
		 * @return discord::Message
		 */

		std::string endpoint = Endpoint("/channels/%/messages/%", channel.id, id);
		cpr::Body body("{\"content\": \"" + EscapeString(text) + "\"}");
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL);

		*this = { result };
		return *this;
	}

	discord::Message Message::EditMessage(discord::EmbedBuilder embed) {
		/**
		 * @brief Edit the message's embed.
		 *
		 * ```cpp
		 *      discord::Message edited_embed_message = message.EditMessage(new_embed);
		 * ```
		 *
		 * @param[in] embed The new embed.
		 *
		 * @return discord::Message
		 */

		std::string endpoint = Endpoint("/channels/%/messages/%", channel.id, id);
		cpr::Body body(Format("{\"embed\": %}", embed.ToJson()));
		nlohmann::json result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

		*this = { result };
		return *this;
	}

	discord::Message Message::EditMessage(int flags) {
		/**
		 * @brief Edit the message's embed.
		 *
		 * ```cpp
		 *      discord::Message edited_embed_message = message.EditMessage(1 << 0);
		 * ```
		 *
		 * @param[in] flags The message flags.
		 *
		 * @return discord::Message
		 */

		std::string endpoint = Endpoint("/channels/%/messages/%", channel.id, id);
		cpr::Body body(Format("{\"flags\": %}", flags));
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

		std::string endpoint = Endpoint("/channels/%/messages/%", channel.id, id);
		nlohmann::json result = SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		
		*this = discord::Message();
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

		nlohmann::json result = SendPutRequest(Endpoint("/channels/%/pins/%", channel.id, id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
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

		nlohmann::json result = SendDeleteRequest(Endpoint("/channels/%/pins/%", channel.id, id), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}