#ifndef DISCPP_MESSAGE_H
#define DISCPP_MESSAGE_H

#include "discord_object.h"
#include "channel.h"
#include "user.h"
#include "member.h"
#include "guild.h"
#include "reaction.h"
#include "role.h"
#include "embed_builder.h"
#include "attachment.h"

namespace discpp {
	enum class GetReactionsMethod : int {
		BEFORE_USER,
		AFTER_USER
	};

	struct MessageActivity {
		enum class ActivityType : int {
			NONE = 0,
			JOIN = 1,
			SPECTATE = 2,
			LISTEN = 3,
			JOIN_REQUEST = 5
		};

		ActivityType type;
		std::string party_id;

		MessageActivity() = default;
		MessageActivity(rapidjson::Document& json) {
			type = static_cast<ActivityType>(json["type"].GetInt());
			party_id = GetDataSafely<std::string>(json, "party_id");
		}
	};

	struct MessageApplication : public DiscordObject {
		//snowflake id;
		std::string cover_image;
		std::string description;
		std::string icon;
		std::string name;

		MessageApplication() = default;
		MessageApplication(rapidjson::Document& json) {
			id = SnowflakeFromString(json["id"].GetString());
			cover_image = GetDataSafely<std::string>(json, "cover_image");
			description = json["description"].GetString();
			icon = json["icon"].GetString();
			name = json["name"].GetString();
		}
	};

	struct MessageReference {
		snowflake message_id;
		snowflake channel_id;
		snowflake guild_id;

		MessageReference() = default;
		MessageReference(rapidjson::Document& json) {
			message_id = GetIDSafely(json, "message_id");
			channel_id = SnowflakeFromString(json["channel_id"].GetString());
			guild_id = GetIDSafely(json, "guild_id");
		}
	};

	class Message : public DiscordObject {
	public:
		Message() = default;
		Message(snowflake id);
		Message(snowflake message_id, snowflake channel_id);
		Message(rapidjson::Document& json);

		void AddReaction(discpp::Emoji emoji);
		void RemoveBotReaction(discpp::Emoji emoji);
		void RemoveReaction(discpp::User user, discpp::Emoji emoji);
        std::unordered_map<discpp::snowflake, discpp::User> GetReactorsOfEmoji(discpp::Emoji emoji, int amount);
		std::unordered_map<discpp::snowflake, discpp::User> GetReactorsOfEmoji(discpp::Emoji emoji, discpp::User user, GetReactionsMethod method);
		void ClearReactions();
		discpp::Message EditMessage(std::string text);
		discpp::Message EditMessage(discpp::EmbedBuilder embed);
		discpp::Message EditMessage(int flags);
		void DeleteMessage();
		inline void PinMessage();
		inline void UnpinMessage();
        inline bool IsTTS();
        inline bool MentionsEveryone();
        inline bool IsPinned();

        discpp::Channel channel;
        std::shared_ptr<discpp::Guild> guild;
        std::shared_ptr<discpp::User> author;
		std::string content;
		std::string timestamp; // TODO: Convert to iso8601Time
		std::string edited_timestamp; // TODO: Convert to iso8601Time
		std::unordered_map<discpp::snowflake, std::shared_ptr<discpp::Member>> mentions;
		std::unordered_map<discpp::snowflake, std::shared_ptr<discpp::Role>> mentioned_roles;
		std::unordered_map<discpp::snowflake, discpp::GuildChannel> mention_channels;
		std::vector<discpp::Attachment> attachments;
		std::vector<discpp::EmbedBuilder> embeds;
		std::vector<discpp::Reaction> reactions;
		snowflake webhook_id;
		int type;
		discpp::MessageActivity activity;
		discpp::MessageApplication application;
		discpp::MessageReference message_reference;
		int flags;
	private:
	    char bit_flags; /**< For internal use only. */
	};
}

#endif