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
			*this = *message->second;
		}
	}

	Message::Message(rapidjson::Document& json) {
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

		id = GetIDSafely(json, "id");
		channel = std::make_shared<discpp::Channel>(ConstructDiscppObjectFromID(json, "channel_id", discpp::Channel()));
        guild = std::make_shared<discpp::Guild>(ConstructDiscppObjectFromID(json, "guild_id", discpp::Guild()));
		author = std::make_shared<discpp::User>(ConstructDiscppObjectFromJson(json, "author", discpp::User()));
		content = GetDataSafely<std::string>(json, "content");
		timestamp = GetDataSafely<std::string>(json, "timestamp");
		edited_timestamp = GetDataSafely<std::string>(json, "edited_timestamp");
		if (GetDataSafely<bool>(json, "tts")) {
		    bit_flags |= 0b1;
		}
		if (GetDataSafely<bool>(json, "mention_everyone")) {
		    bit_flags |= 0b10;
		}
		if (ContainsNotNull(json, "mentions")) {
            for (auto const &mention : json["mentions"].GetArray()) {
                if (!mention.IsNull()) {
                    // This has a weird layout, thats why theres so much json stuff.
                    // The API docs says this type is an, "array of user objects, with an additional partial member field"

                    rapidjson::Document mention_json;
                    mention_json.CopyFrom(mention, mention_json.GetAllocator());

                    rapidjson::Document new_member_json;
                    new_member_json.SetObject();

                    rapidjson::Document::AllocatorType &allocator = new_member_json.GetAllocator();
                    new_member_json.AddMember("deaf", mention_json["member"]["deaf"], allocator);
                    new_member_json.AddMember("deaf", mention_json["member"]["hoisted_role"], allocator);
                    new_member_json.AddMember("deaf", mention_json["member"]["joined_at"], allocator);
                    new_member_json.AddMember("deaf", mention_json["member"]["mute"], allocator);
                    new_member_json.AddMember("deaf", mention_json["member"]["roles"], allocator);

                    rapidjson::Value user(rapidjson::kObjectType);
                    user.AddMember("id", mention_json["id"], allocator);
                    user.AddMember("username", mention_json["username"], allocator);
                    user.AddMember("discriminator", mention_json["discriminator"], allocator);
                    user.AddMember("avatar", mention_json["avatar"], allocator);

                    new_member_json.AddMember("user", user, allocator);

                    std::shared_ptr<discpp::Member> tmp = guild->GetMember(SnowflakeFromString(new_member_json["user"]["id"].GetString()));
                    mentions.insert({tmp->id, tmp});
                }
            }
        }

        if (ContainsNotNull(json, "mentioned_roles")) {
            for (auto const& mentioned_role : json["mentioned_roles"].GetArray()) {
                rapidjson::Document mentioned_role_json;
                mentioned_role_json.CopyFrom(mentioned_role, mentioned_role_json.GetAllocator());

                std::shared_ptr<discpp::Role> tmp = guild->GetRole(SnowflakeFromString(mentioned_role_json.GetString()));
                mentioned_roles.insert({ tmp->id, tmp });
            }
        }

        if (ContainsNotNull(json, "mention_channels")) {
            for (auto const& mention_channel : json["mention_channels"].GetArray()) {
                rapidjson::Document mention_channel_json;
                mention_channel_json.CopyFrom(mention_channel, mention_channel_json.GetAllocator());

                /*discpp::GuildChannel channel(mention_channel["id"].GetString(), guild.id);
                channel.type = static_cast<ChannelType>(mention_channel["type"].GetInt());
                channel.guild_id = mention_channel["guild_id"].GetString();
                channel.name = mention_channel["name"].GetString();*/

                std::shared_ptr<discpp::Guild> guild = globals::client_instance->GetGuild(SnowflakeFromString(mention_channel["guild_id"].GetString()));
                std::shared_ptr<discpp::GuildChannel> channel = guild->GetChannel(SnowflakeFromString(mention_channel["id"].GetString()));

                mention_channels.insert({ channel->id, channel });
            }
        }

        if (ContainsNotNull(json, "attachments")) {
            for (auto const& attachment : json["attachments"].GetArray()) {
                rapidjson::Document attachment_json;
                attachment_json.CopyFrom(attachment, attachment_json.GetAllocator());

                attachments.push_back(discpp::Attachment(attachment_json));
            }
        }

        if (ContainsNotNull(json, "embeds")) {
            for (auto const& embed : json["embeds"].GetArray()) {
                rapidjson::Document embed_json;
                embed_json.CopyFrom(embed, embed_json.GetAllocator());

                embeds.push_back(discpp::EmbedBuilder(embed_json));
            }
        }

        if (ContainsNotNull(json, "reactions")) {
            for (auto const& reaction : json["reactions"].GetArray()) {
                rapidjson::Document reaction_json;
                reaction_json.CopyFrom(reaction, reaction_json.GetAllocator());

                discpp::Reaction tmp(reaction_json);
                reactions.push_back(tmp);
            }
        }
        if (GetDataSafely<bool>(json, "pinned")) {
            bit_flags |= 0b100;
        }
		webhook_id = GetIDSafely(json, "webhook_id");
		type = GetDataSafely<int>(json, "type");
		activity = ConstructDiscppObjectFromJson(json, "activity", discpp::MessageActivity());
        application = ConstructDiscppObjectFromJson(json, "application", discpp::MessageApplication());
        message_reference = ConstructDiscppObjectFromJson(json, "message_reference", discpp::MessageReference());
		flags = GetDataSafely<int>(json, "flags");
	}

    inline bool Message::IsTTS() {
        return (bit_flags & 0b1) == 0b1;
    }

    inline bool Message::MentionsEveryone() {
        return (bit_flags & 0b10) == 0b10;
    }

	inline bool Message::IsPinned() {
        return (bit_flags & 0b100) == 0b100;
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

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.ToURL() + "/@me");
		SendPutRequest(endpoint, DefaultHeaders(), channel->id, RateLimitBucketType::CHANNEL);
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

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.ToURL() + "/@me");
		SendDeleteRequest(endpoint, DefaultHeaders(), channel->id, RateLimitBucketType::CHANNEL);
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

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.ToURL() + "/" + std::to_string(user.id));
		SendDeleteRequest(endpoint, DefaultHeaders(), channel->id, RateLimitBucketType::CHANNEL);
	}

	std::unordered_map<discpp::snowflake, discpp::User> Message::GetReactorsOfEmoji(discpp::Emoji emoji, int amount) {
		/**
		 * @brief Get reactors of a specific emoji.
		 *
		 * You can use `std::unordered_map::find` to check if a user is contained in it with the users id.
		 *
		 * ```cpp
		 *      std::unordered_map<discpp::snowflake, discpp::User> reactors = message.GetReactorOfEmoji(emoji, 50);
		 * ```
		 *
		 * @param[in] emoji The emoji to get reactors of.
		 * @param[in] amount The amount of users to get.
		 *
		 * @return std::vector<discpp::User>
		 */

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.ToURL());
		cpr::Body body("{\"limit\": " + std::to_string(amount) + "}");
		rapidjson::Document result = SendGetRequest(endpoint, DefaultHeaders(), channel->id, RateLimitBucketType::CHANNEL, body);
		
		std::unordered_map<discpp::snowflake, discpp::User> users;
		IterateThroughNotNullJson(result, [&](rapidjson::Document& user_json) {
		    discpp::User tmp(user_json);
		    users.insert({ tmp.id, tmp });
		});

		return users;
	}

	std::unordered_map<discpp::snowflake, discpp::User> Message::GetReactorsOfEmoji(discpp::Emoji emoji, discpp::User user, GetReactionsMethod method) {
		/**
		 * @brief Get reactors of a specific emoji of the specific method.
		 *
		 * You can use `std::unordered_map::find` to check if a user is contained in it with the users id.
		 *
		 * ```cpp
		 *      std::unordered_map<discpp::snowflake, discpp::User> reactors = message.GetReactorOfEmoji(emoji, 50, reaction_method);
		 * ```
		 *
		 * @param[in] emoji The emoji to get reactors of.
		 * @param[in] amount The amount of users to get.
		 * @param[in] method The method the users reacted by.
		 *
		 * @return std::vector<discpp::User>
		 */

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id) + "/reactions/" + emoji.ToURL());
		std::string method_str = (method == GetReactionsMethod::BEFORE_USER) ? "before" : "after";
		cpr::Body body("{\"" + method_str + "\": " + std::to_string(user.id) + "}");
		rapidjson::Document result = SendGetRequest(endpoint, DefaultHeaders(), channel->id, RateLimitBucketType::CHANNEL, body);

        std::unordered_map<discpp::snowflake, discpp::User> users;
        IterateThroughNotNullJson(result, [&](rapidjson::Document& user_json) {
            discpp::User tmp(user_json);
            users.insert({ tmp.id, tmp });
        });

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

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id) + "/reactions");
		SendDeleteRequest(endpoint, DefaultHeaders(), channel->id, RateLimitBucketType::CHANNEL);
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

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id));
		cpr::Body body("{\"content\": \"" + EscapeString(text) + "\"}");
		rapidjson::Document result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL);

		*this = discpp::Message(result);
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

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id));
		rapidjson::Document json = embed.ToJson();
		cpr::Body body("{\"embed\": " + DumpJson(json) + "}");
		rapidjson::Document result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Message(result);
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

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id));
		cpr::Body body("{\"flags\": " + std::to_string(flags) + "}");
        rapidjson::Document result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Message(result);
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

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel->id) + "/messages/" + std::to_string(id));
		SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		
		*this = discpp::Message();
	}

	inline void Message::PinMessage() {
		/**
		 * @brief Pin the message to the channel.
		 *
		 * ```cpp
		 *      message.PinMessage();
		 * ```
		 *
		 * @return void
		 */

		SendPutRequest(Endpoint("/channels/" + std::to_string(channel->id) + "/pins/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	inline void Message::UnpinMessage() {
		/**
		 * @brief Unpin this message.
		 *
		 * ```cpp
		 *      message.UnpinMessage();
		 * ```
		 *
		 * @return void
		 */

		SendDeleteRequest(Endpoint("/channels/" + std::to_string(channel->id) + "/pins/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}