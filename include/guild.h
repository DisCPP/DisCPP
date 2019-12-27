#ifndef DISCORDPP_GUILD_H
#define DISCORDPP_GUILD_H

#include "discord_object.h"
#include "utils.h"
#include "channel.h"
#include "emoji.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Member;

	class Guild : DiscordObject {
	public:
		Guild() = default;
		Guild(snowflake id);
		Guild(nlohmann::json json);

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
		//std::vector<discord::Role> roles;
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