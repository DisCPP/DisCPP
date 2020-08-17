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

	Message::Message(const discpp::JsonObject& json) {
		id = json.GetIDSafely("id");
		channel = globals::client_instance->cache.GetChannel(SnowflakeFromString(json["channel_id"].GetString()));
		try {
            guild = channel.GetGuild().value();
        } catch (const exceptions::DiscordObjectNotFound&) {
		} catch (const exceptions::ProhibitedEndpointException&) {}

		author = json.ConstructDiscppObjectFromJson("author", discpp::User());
        if (json.ContainsNotNull("member")) {
            if (guild != nullptr) {
                try {
                    auto mbr = guild->GetMember(author.id);
                    member = mbr;
                } catch (const exceptions::DiscordObjectNotFound&) {

                    // Since the member isn't cached, create it.
                    auto mbr = std::make_shared<discpp::Member>(json["member"], *guild);
                    mbr->user = author;
                    member = mbr;

                    // Add the new member into cache since it isn't already.
                    guild->members.emplace(id, mbr);
                }
            }
        }
		content = json.Get<std::string>("content");
        if (json.ContainsNotNull("timestamp")) {
            timestamp = std::chrono::system_clock::from_time_t(TimeFromDiscord(json["timestamp"].GetString()));
        }
		if (json.ContainsNotNull("edited_timestamp")) {
		    edited_timestamp = std::chrono::system_clock::from_time_t(TimeFromDiscord(json["edited_timestamp"].GetString()));
		}
		if (json.Get<bool>("tts")) {
		    bit_flags |= 0b1;
		}
		if (json.Get<bool>("mention_everyone")) {
		    bit_flags |= 0b10;
		}
		if (json.ContainsNotNull("mentions")) {
		    json.IterateThrough("mentions", [&] (const discpp::JsonObject& mention_json)->bool {
                discpp::User tmp = discpp::User(mention_json);
                mentions.insert({ tmp.id, tmp });

                return true;
		    });
        }

        if (json.ContainsNotNull("mention_roles")) {
            json.IterateThrough("mention_roles", [&] (const discpp::JsonObject& mentioned_role_json)->bool {
                mentioned_roles.emplace_back(mentioned_role_json.GetString());

                return true;
            });
        }

        if (json.ContainsNotNull("mention_channels")) {
            json.IterateThrough("mention_channels", [&] (const discpp::JsonObject& mention_channel_json) ->bool {
                discpp::Message::ChannelMention channel_mention(mention_channel_json);
                mention_channels.emplace(channel_mention.id, mention_channel_json);
                return true;
            });
        }

        if (json.ContainsNotNull("attachments")) {
            json.IterateThrough("attachments", [&] (const discpp::JsonObject& attachment_json)->bool {
                attachments.emplace_back(attachment_json);

                return true;
            });
        }

        if (json.ContainsNotNull("embeds")) {
            json.IterateThrough("embeds", [&] (const discpp::JsonObject& embed_json)->bool {
                embeds.emplace_back(embed_json);
                return true;
            });
        }

        if (json.ContainsNotNull("reactions")) {
            json.IterateThrough("reactions", [&] (const discpp::JsonObject& reaction_json)->bool {
                reactions.emplace_back(reaction_json);
                return true;
            });
        }
        if (json.Get<bool>("pinned")) {
            bit_flags |= 0b100;
        }
		webhook_id = json.GetIDSafely("webhook_id");
		type = json.Get<int>("type");
		activity = std::make_shared<discpp::MessageActivity>(json.ConstructDiscppObjectFromJson("activity", discpp::MessageActivity()));
        application = std::make_shared<discpp::MessageApplication>(json.ConstructDiscppObjectFromJson("application", discpp::MessageApplication()));
        message_reference = std::make_shared<discpp::MessageReference>(json.ConstructDiscppObjectFromJson("message_reference", discpp::MessageReference()));
		flags = json.Get<int>("flags");
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

		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL, body);
		
		std::unordered_map<discpp::Snowflake, discpp::User> users;
        result->IterateThrough([&](discpp::JsonObject& user_json)->bool {
		    discpp::User tmp(user_json);
		    users.emplace(tmp.id, tmp);
		    return true;
		});

		return users;
	}

	std::unordered_map<discpp::Snowflake, discpp::User> Message::GetReactorsOfEmoji(const discpp::Emoji& emoji, const discpp::User& user, const GetReactionsMethod& method) {
        discpp::Emoji tmp = emoji;
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id) + "/reactions/" + tmp.ToURL());
		std::string method_str = (method == GetReactionsMethod::BEFORE_USER) ? "before" : "after";
		cpr::Body body("{\"" + method_str + "\": " + std::to_string(user.id) + "}");
		std::unique_ptr<discpp::JsonObject> result = SendGetRequest(endpoint, DefaultHeaders(), channel.id, RateLimitBucketType::CHANNEL, body);

        std::unordered_map<discpp::Snowflake, discpp::User> users;
        result->IterateThrough([&](discpp::JsonObject& user_json)->bool {
            discpp::User tmp(user_json);
            users.emplace(tmp.id, tmp);
            return true;
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
		std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL);

		*this = discpp::Message(*result);
		return *this;
	}

	discpp::Message Message::EditMessage(const discpp::EmbedBuilder& embed) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id));

		cpr::Body body("{\"embed\": " + embed.ToJson().DumpJson() + "}");
		std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Message(*result);
		return *this;
	}

	discpp::Message Message::EditMessage(const int& flags) {
		std::string endpoint = Endpoint("/channels/" + std::to_string(channel.id) + "/messages/" + std::to_string(id));
		cpr::Body body("{\"flags\": " + std::to_string(flags) + "}");
        std::unique_ptr<discpp::JsonObject> result = SendPatchRequest(endpoint, DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::CHANNEL, body);

        *this = discpp::Message(*result);
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