#ifndef DISCORDPP_GUILD_H
#define DISCORDPP_GUILD_H

#include "discord_object.h"
#include "emoji.h"
#include "member.h"
#include "channel.h"
#include "utils.h"
#include "role.h"

#include <nlohmann/json.hpp>

namespace discord {
	enum GuildChannelType : int {
		GUILD_TEXT = 0,
		DM = 1,
		GUILD_VOICE = 2,
		GROUP_DM = 3,
		GUILD_CATEGORY = 4,
		GUILD_NEWS = 5,
		GUILD_STORE = 6
	};

	class Guild : DiscordObject {
	public:
		Guild() = default;
		Guild(snowflake id);
		Guild(nlohmann::json json);

		// discord::Guild ModifyGuild(); // https://discordapp.com/developers/docs/resources/guild#modify-guild
		void DeleteGuild();
		std::vector<discord::Channel> GetChannels();
		// discord::Channel CreateChannel(std::string name, GuildChannelType type, std::string topic, int bitrate, int user_limit, int rate_limit_per_user, int position, discord::PermissionOverwrites permission_overwrites, discord::Channel category_id, bool nsfw);
		// void ModifyChannelPositions(std::vector<discord::Channel> channels, std::vector<int> positiion); // https://discordapp.com/developers/docs/resources/guild#modify-guild-channel-positions
		discord::Member GetMember(snowflake id);
		std::vector<discord::Member> GetMembers(int limit); // Limit
		std::vector<discord::Member> GetMembers(snowflake id); // After
		discord::Member AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf);

		std::vector<discord::Emoji> GetEmojis();
		discord::Emoji GetEmoji(snowflake id);
		// discord::Emoji CreateEmoji(std::string name, std::string image, std::vector<discord::Role> roles); // https://discordapp.com/developers/docs/resources/emoji#create-guild-emoji
		discord::Emoji ModifyEmoji(discord::Emoji emoji, std::string name, std::vector<discord::Role> roles); // https://discordapp.com/developers/docs/resources/emoji#modify-guild-emoji
		void DeleteEmoji(discord::Emoji emoji);

		snowflake id;
		std::string name;
		std::string icon;
		std::string splash;
		std::string owner;
		snowflake owner_id;
		int permissions;
		std::string region;
		snowflake afk_channel_id;
		int afk_timeout;
		bool embed_enabled;
		snowflake embed_channel_id;
		discord::specials::VerificationLevel verification_level;
		discord::specials::DefaultMessageNotificationLevel default_message_notifications;
		discord::specials::ExplicitContentFilterLevel explicit_content_filter;
		std::vector<discord::Role> roles;
		std::vector<discord::Emoji> emojis;
		// features
		discord::specials::MFALevel mfa_level;
		snowflake application_id;
		bool widget_enabled;
		snowflake widget_channel_id;
		snowflake system_channel_id;
		std::string joined_at; // TODO: Convert to iso8601Time
		bool large;
		bool unavailable;
		int member_count;
		//voice_states
		std::vector<discord::Member> members;
		std::vector<discord::Channel> channels;
		//presences
		int max_presences;
		int max_members;
		std::string vanity_url_code;
		std::string description;
		std::string banner;
		discord::specials::NitroTier premium_tier;
		int premium_subscription_count;
		std::string preferred_locale;
	};
}

#endif