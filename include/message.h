#ifndef DISCORDPP_MESSAGE_H
#define DISCORDPP_MESSAGE_H

#include "discord_object.h"
#include "channel.h"
#include "user.h"
#include "member.h"
#include "guild.h"
#include "reaction.h"
#include "role.h"

namespace discord {
	enum GetReactionsMethod : int {
		BEFORE_USER,
		AFTER_USER
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
		//discord::Message EditMessage(discord::Embed embed); // TODO: Embed
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
		//std::vector<discord::Attachment> attachments;
		//std::vector<discord::Embed> embeds;
		std::vector<discord::Reaction> reactions;
		bool pinned;
		snowflake webhook_id;
		int type;
		//activity?
		//application?
		//message_reference?
		int flags;
	};
}

#endif