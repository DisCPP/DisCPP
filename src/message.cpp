#include "message.h"
#include "client.h"
#include "channel.h"
#include "guild.h"
#include "member.h"
#include "embed_builder.h"
#include "exceptions.h"

namespace discpp {
	Message::Message(const Snowflake& channel_id, const Snowflake& id, bool can_request) : discpp::DiscordObject(id) {
        *this = globals::client_instance->cache.GetDiscordMessage(channel_id, id, can_request);
	}

	Message::Message(rapidjson::Document& json) {
		id = GetIDSafely(json, "id");
		channel = globals::client_instance->cache.GetChannel(SnowflakeFromString(json["channel_id"].GetString()));
		try {
            guild = channel.GetGuild();
        } catch (const DiscordObjectNotFound&) {
		} catch (const ProhibitedEndpointException&) {}

		author = ConstructDiscppObjectFromJson(json, "author", discpp::User());
        if (ContainsNotNull(json, "member")) {
            if (guild != nullptr) {
                try {
                    auto mbr = guild->GetMember(author.id);
                    member = mbr;
                } catch (const DiscordObjectNotFound&) {
                    rapidjson::Document doc(rapidjson::kObjectType);
                    doc.CopyFrom(json["member"], doc.GetAllocator());

                    // Since the member isn't cached, create it.
                    auto mbr = std::make_shared<discpp::Member>(discpp::Member(doc, *guild));
                    mbr->user = author;
                    member = mbr;

                    // Add the new member into cache since it isn't already.
                    guild->members.emplace(id, mbr);
                }
            }
        }
		content = GetDataSafely<std::string>(json, "content");
        if (discpp::ContainsNotNull(json, "timestamp")) {
            timestamp = std::chrono::system_clock::from_time_t(TimeFromDiscord(json["timestamp"].GetString()));
        }
		if (discpp::ContainsNotNull(json, "edited_timestamp")) {
		    edited_timestamp = std::chrono::system_clock::from_time_t(TimeFromDiscord(json["edited_timestamp"].GetString()));
		}
		if (GetDataSafely<bool>(json, "tts")) {
		    bit_flags |= 0b1;
		}
		if (GetDataSafely<bool>(json, "mention_everyone")) {
		    bit_flags |= 0b10;
		}
		if (ContainsNotNull(json, "mentions")) {
            for (auto const& mention : json["mentions"].GetArray()) {
                rapidjson::Document mention_json(rapidjson::kObjectType);
                mention_json.CopyFrom(mention, mention_json.GetAllocator());

                discpp::User tmp = discpp::User(mention_json);
                mentions.insert({ tmp.id, tmp });
            }
        }

        if (ContainsNotNull(json, "mention_roles")) {
            for (auto const& mentioned_role : json["mention_roles"].GetArray()) {
                rapidjson::Document mentioned_role_json;
                mentioned_role_json.CopyFrom(mentioned_role, mentioned_role_json.GetAllocator());

                mentioned_roles.push_back(SnowflakeFromString(mentioned_role_json.GetString()));
            }
        }

        if (ContainsNotNull(json, "mention_channels")) {
            for (auto const& mention_channel : json["mention_channels"].GetArray()) {
                rapidjson::Document mention_channel_json;
                mention_channel_json.CopyFrom(mention_channel, mention_channel_json.GetAllocator());

                discpp::Message::ChannelMention channel_mention(mention_channel_json);
                mention_channels.emplace(channel_mention.id, mention_channel_json);
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
		activity = std::make_shared<discpp::MessageActivity>(ConstructDiscppObjectFromJson(json, "activity", discpp::MessageActivity()));
        application = std::make_shared<discpp::MessageApplication>(ConstructDiscppObjectFromJson(json, "application", discpp::MessageApplication()));
        message_reference = std::make_shared<discpp::MessageReference>(ConstructDiscppObjectFromJson(json, "message_reference", discpp::MessageReference()));
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

	void Message::AddReaction(const discpp::Emoji& emoji) {
        discpp::Emoji tmp = emoji;

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + tmp.ToURL() + "/@me");
		SendPutRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL);
	}

	void Message::RemoveBotReaction(const discpp::Emoji& emoji) {
        discpp::Emoji tmp = emoji;
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + tmp.ToURL() + "/@me");
		SendDeleteRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL);
	}

	void Message::RemoveReaction(const discpp::User& user, const discpp::Emoji& emoji) {
        discpp::Emoji tmp = emoji;
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + tmp.ToURL() + "/" + std::to_string(user.id));
		SendDeleteRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL);
	}

	std::unordered_map<discpp::Snowflake, discpp::User> Message::GetReactorsOfEmoji(const discpp::Emoji& emoji, const int& amount) {
        discpp::Emoji tmp = emoji;
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + tmp.ToURL());
		cpr::Body body("{\"limit\": " + std::to_string(amount) + "}");
		rapidjson::Document result = SendGetRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL, body);
		
		std::unordered_map<discpp::Snowflake, discpp::User> users;
		IterateThroughNotNullJson(result, [&](rapidjson::Document& user_json) {
		    discpp::User tmp(user_json);
		    users.insert({ tmp.id, tmp });
		});

		return users;
	}

	std::unordered_map<discpp::Snowflake, discpp::User> Message::GetReactorsOfEmoji(const discpp::Emoji& emoji, const discpp::User& user, const GetReactionsMethod& method) {
        discpp::Emoji tmp = emoji;
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + tmp.ToURL());
		std::string method_str = (method == GetReactionsMethod::BEFORE_USER) ? "before" : "after";
		cpr::Body body("{\"" + method_str + "\": " + std::to_string(user.id) + "}");
		rapidjson::Document result = SendGetRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL, body);

        std::unordered_map<discpp::Snowflake, discpp::User> users;
        IterateThroughNotNullJson(result, [&](rapidjson::Document& user_json) {
            discpp::User tmp(user_json);
            users.insert({ tmp.id, tmp });
        });

		return users;
	}

	void Message::ClearReactions() {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions");
		SendDeleteRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL);
	}

	discpp::Message Message::EditMessage(const std::string& text) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id));
		cpr::Body body("{\"content\": \"" + EscapeString(text) + "\"}");
		rapidjson::Document result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL);

		*this = discpp::Message(result);
		return *this;
	}

	discpp::Message Message::EditMessage(const discpp::EmbedBuilder& embed) {

		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id));
		rapidjson::Document json = embed.ToJson();
		cpr::Body body("{\"embed\": " + DumpJson(json) + "}");
		rapidjson::Document result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Message(result);
		return *this;
	}

	discpp::Message Message::EditMessage(const int& flags) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id));
		cpr::Body body("{\"flags\": " + std::to_string(flags) + "}");
        rapidjson::Document result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Message(result);
		return *this;
	}

	void Message::DeleteMessage() {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id));
		SendDeleteRequest(endpoint, DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
		
		*this = discpp::Message();
	}

	inline void Message::PinMessage() {
		SendPutRequest(Endpoint("/channels/" + std::to_string(channel.id) + "/pins/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}

	inline void Message::UnpinMessage() {
		SendDeleteRequest(Endpoint("/channels/" + std::to_string(channel.id) + "/pins/" + std::to_string(id)), DefaultHeaders(), id, RateLimitBucketType::CHANNEL);
	}
}