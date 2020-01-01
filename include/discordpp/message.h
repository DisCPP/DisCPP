#ifndef DISCORDPP_MESSAGE_H
#define DISCORDPP_MESSAGE_H

#include "discord_object.h"
#include "channel.h"
#include "user.h"
#include "member.h"
#include "guild.h"
#include "reaction.h"
#include "role.h"
#include "embed_builder.h"
#include "attachment.h"

namespace discord {
	enum GetReactionsMethod : int {
		BEFORE_USER,
		AFTER_USER
	};

	struct MessageActivity {
		enum ActivityType : int {
			NONE = 0,
			JOIN = 1,
			SPECTATE = 2,
			LISTEN = 3,
			JOIN_REQUEST = 5
		};

		ActivityType type;
		std::string party_id;

		MessageActivity() = default;
		MessageActivity(nlohmann::json json) {
			type = static_cast<ActivityType>(json["type"]);
			party_id = json["pary_id"];
		}
	};

	struct MessageApplication {
		snowflake id;
		std::string cover_image;
		std::string description;
		std::string icon;
		std::string name;

		MessageApplication() = default;
		MessageApplication(nlohmann::json json) {
			id = json["id"].get<snowflake>();
			cover_image = json["cover_image"];
			description = json["description"];
			icon = json["icon"];
			name = json["name"];
		}
	};

	struct MessageReference {
		snowflake message_id;
		snowflake channel_id;
		snowflake guild_id;

		MessageReference() = default;
		MessageReference(nlohmann::json json) {
			message_id = json["message_id"].get<snowflake>();
			channel_id = json["channel_id"].get<snowflake>();
			guild_id = json["guild_id"].get<snowflake>();
		}
	};

	class Message : DiscordObject {
	public:
		Message() = default;
		Message(snowflake id);
		Message(nlohmann::json json);

		void AddReaction(discord::Emoji emoji);
		void RemoveBotReaction(discord::Emoji emoji);
		void RemoveReaction(discord::User user, discord::Emoji emoji);
		std::vector<discord::User> GetReactorsOfEmoji(discord::Emoji emoji, int amount);
		std::vector<discord::User> GetReactorsOfEmoji(discord::Emoji emoji, discord::User user, GetReactionsMethod method);
		void ClearReactions();
		discord::Message EditMessage(std::string text);
		discord::Message EditMessage(discord::EmbedBuilder embed);
		discord::Message EditMessage(int flags);
		void DeleteMessage();
		void PinMessage();
		void UnpinMessage();

		snowflake id;
		discord::Channel channel;
		discord::Guild guild;
		discord::User author;
		std::string content;
		std::string timestamp; // TODO: Convert to iso8601Time
		std::string edited_timestamp; // TODO: Convert to iso8601Time
		bool tts;
		bool mention_everyone;
		std::vector<discord::Member> mentions;
		std::vector<snowflake> mentioned_roles; // TODO: Use discord::Role instead of snowflakes
		std::vector<discord::Channel> mention_channels;
		std::vector<discord::Attachment> attachments;
		std::vector<discord::EmbedBuilder> embeds;
		std::vector<discord::Reaction> reactions;
		bool pinned;
		snowflake webhook_id;
		int type;
		discord::MessageActivity activity;
		discord::MessageApplication application;
		discord::MessageReference message_reference;
		int flags;
	};
}

#endif