#include "settings.h"

namespace discpp {
	FriendSource::FriendSource(rapidjson::Document& json) {
		if (GetDataSafely<bool>(json, "all")) flags |= (unsigned int) FriendSourceFlags::ALL;
		if (GetDataSafely<bool>(json, "mutual_friends")) flags |= (unsigned int) FriendSourceFlags::MUTUAL_FRIENDS;
		if (GetDataSafely<bool>(json, "mutual_guilds")) flags |= (unsigned int) FriendSourceFlags::MUTUAL_GUILDS;
	}

	void FriendSource::ModifyAll(const bool all) {
		if (all) {
			this->flags |= (unsigned int)FriendSourceFlags::ALL;
		} else {
			this->flags = this->flags & ~(unsigned int)FriendSourceFlags::ALL;
		}
	}
	
	void FriendSource::ModifyMutualFriends(const bool mutual_friends) {
		if (mutual_friends) {
			this->flags |= (unsigned int)FriendSourceFlags::MUTUAL_FRIENDS;
		} else {
			this->flags = this->flags & ~(unsigned int)FriendSourceFlags::MUTUAL_FRIENDS;
		}
	}

	void FriendSource::ModifyMutualGuilds(const bool mutual_guilds) {
		if (mutual_guilds) {
			this->flags |= (unsigned int)FriendSourceFlags::MUTUAL_GUILDS;
		} else {
			this->flags = this->flags & ~(unsigned int)FriendSourceFlags::MUTUAL_GUILDS;
		}
	}

	bool FriendSource::GetAll() const {
		return (this->flags & (unsigned int)FriendSourceFlags::ALL) == (unsigned int)FriendSourceFlags::ALL;
	}

	bool FriendSource::GetMutualFriends() const {
		return (this->flags & (unsigned int)FriendSourceFlags::MUTUAL_FRIENDS) == (unsigned int)FriendSourceFlags::MUTUAL_FRIENDS;
	}

	bool FriendSource::GetMutualGuilds() const {
		return (this->flags & (unsigned int)FriendSourceFlags::MUTUAL_GUILDS) == (unsigned int)FriendSourceFlags::MUTUAL_GUILDS;
	}

	ClientUserSettings::ClientUserSettings(rapidjson::Document& json) {
		locale = StringToLocale(GetDataSafely<std::string>(json, "locale"));
		status = GetDataSafely<std::string>(json, "status");
		custom_status = GetDataSafely<std::string>(json, "custom_status");
		afk_timeout = GetDataSafely<int>(json, "afk_timeout");
		timezone_offset = GetDataSafely<int>(json, "timezone_offset");
		theme = static_cast<Theme>(GetDataSafely<int>(json, "theme"));

		if (ContainsNotNull(json, "guild_positions")) {
			for (auto const& guild : json["guild_positions"].GetArray()) {
				guild_positions.push_back(discpp::Snowflake(guild.GetString()));
			}
		}

		explicit_content_filter = static_cast<discpp::ExplicitContentFilter>(json["explicit_content_filter"].GetInt());
		rapidjson::Document friend_source_flags_json;
		friend_source_flags_json.CopyFrom(json["friend_source_flags"], friend_source_flags_json.GetAllocator());
		friend_source_flags = FriendSource(friend_source_flags_json);

		if (GetDataSafely<bool>(json, "show_current_game")) flags |= (unsigned int) ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		if (GetDataSafely<bool>(json, "default_guilds_restricted")) flags |= (unsigned int) ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		if (GetDataSafely<bool>(json, "inline_attachment_media")) flags |= (unsigned int) ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		if (GetDataSafely<bool>(json, "inline_embed_media")) flags |= (unsigned int) ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		if (GetDataSafely<bool>(json, "gif_auto_play")) flags |= (unsigned int) ClientUserSettingsFlags::GIF_AUTO_PLAY;
		if (GetDataSafely<bool>(json, "render_embeds")) flags |= (unsigned int) ClientUserSettingsFlags::RENDER_EMBEDS;
		if (GetDataSafely<bool>(json, "render_reactions")) flags |= (unsigned int) ClientUserSettingsFlags::RENDER_REACTIONS;
		if (GetDataSafely<bool>(json, "animate_emoji")) flags |= (unsigned int) ClientUserSettingsFlags::ANIMATE_EMOJI;
		if (GetDataSafely<bool>(json, "enable_tts_command")) flags |= (unsigned int) ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		if (GetDataSafely<bool>(json, "message_display_compact")) flags |= (unsigned int) ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		if (GetDataSafely<bool>(json, "convert_emoticons")) flags |= (unsigned int) ClientUserSettingsFlags::CONVERT_EMOTICONS;
		if (GetDataSafely<bool>(json, "disable_games_tab")) flags |= (unsigned int) ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		if (GetDataSafely<bool>(json, "developer_mode")) flags |= (unsigned int) ClientUserSettingsFlags::DEVELOPER_MODE;
		if (GetDataSafely<bool>(json, "detect_platform_accounts")) flags |= (unsigned int) ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		if (GetDataSafely<bool>(json, "stream_notifications_enabled")) flags |= (unsigned int) ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		if (GetDataSafely<bool>(json, "allow_accessibility_detection")) flags |= (unsigned int) ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		if (GetDataSafely<bool>(json, "contact_sync_enabled")) flags |= (unsigned int) ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		if (GetDataSafely<bool>(json, "native_phone_integration_enabled")) flags |= (unsigned int) ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
	}

	void ClientUserSettings::ModifyShowCurrentGame(const bool show_current_game) {
		if (show_current_game) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		}
	}

	void ClientUserSettings::ModifyDefaultGuildsRestricted(const bool default_guilds_restricted) {
		if (default_guilds_restricted) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		}
	}

	void ClientUserSettings::ModifyInlineAttachmentMedia(const bool inline_attachment_media) {
		if (inline_attachment_media) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		}
	}

	void ClientUserSettings::ModifyInlineEmbedMedia(const bool inline_embed_media) {
		if (inline_embed_media) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		}
	}

	void ClientUserSettings::ModifyGifAutoPlay(const bool gif_auto_play) {
		if (gif_auto_play) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::GIF_AUTO_PLAY;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::GIF_AUTO_PLAY;
		}
	}

	void ClientUserSettings::ModifyRenderEmbeds(const bool render_embeds) {
		if (render_embeds) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::RENDER_EMBEDS;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::RENDER_EMBEDS;
		}
	}

	void ClientUserSettings::ModifyRenderReactions(const bool render_reactions) {
		if (render_reactions) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::RENDER_REACTIONS;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::RENDER_REACTIONS;
		}
	}

	void ClientUserSettings::ModifyAnimateEmoji(const bool animate_emoji) {
		if (animate_emoji) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::ANIMATE_EMOJI;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::ANIMATE_EMOJI;
		}
	}
	
	void ClientUserSettings::ModifyEnableTtsCommand(const bool enable_tts_command) {
		if (enable_tts_command) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		}
	}
	
	void ClientUserSettings::ModifyMessageDisplayCompact(const bool message_display_compact) {
		if (message_display_compact) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		}
	}
	
	void ClientUserSettings::ModifyConvertEmoticons(const bool convert_emoticons) {
		if (convert_emoticons) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::CONVERT_EMOTICONS;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::CONVERT_EMOTICONS;
		}
	}
	
	void ClientUserSettings::ModifyDisableGamesTab(const bool disable_games_tab) {
		if (disable_games_tab) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		}
	}
	
	void ClientUserSettings::ModifyDeveloperMode(const bool developer_mode) {
		if (developer_mode) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::DEVELOPER_MODE;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::DEVELOPER_MODE;
		}
	}
	
	void ClientUserSettings::ModifyDetectPlatformAccounts(const bool detect_platform_accounts) {
		if (detect_platform_accounts) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		}
	}
	
	void ClientUserSettings::ModifyStreamNotificationsEnabled(const bool stream_notifications_enabled) {
		if (stream_notifications_enabled) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		}
	}
	
	void ClientUserSettings::ModifyAllowAccessibilityDetection(const bool allow_accessibility_detection) {
		if (allow_accessibility_detection) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		}
	}
	
	void ClientUserSettings::ModifyContactSyncEnabled(const bool contact_sync_enabled) {
		if (contact_sync_enabled) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		}
	}
	
	void ClientUserSettings::ModifyNativePhoneIntegrationEnabled(const bool native_phone_integration_enabled) {
		if (native_phone_integration_enabled) {
			this->flags |= (unsigned int) ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
		} else {
			this->flags = this->flags & ~(unsigned int) ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
		}
	}

	bool ClientUserSettings::GetShowCurrentGame() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::SHOW_CURRENT_GAME) == (unsigned int) ClientUserSettingsFlags::SHOW_CURRENT_GAME;
	}
	
	bool ClientUserSettings::GetDefaultGuildsRestricted() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED) == (unsigned int) ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
	}
	
	bool ClientUserSettings::GetInlineAttachmentMedia() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA) == (unsigned int) ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
	}
	
	bool ClientUserSettings::GetInlineEmbedMedia() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::INLINE_EMBED_MEDIA) == (unsigned int) ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
	}
	
	bool ClientUserSettings::GetGifAutoPlay() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::GIF_AUTO_PLAY) == (unsigned int) ClientUserSettingsFlags::GIF_AUTO_PLAY;
	}
	
	bool ClientUserSettings::GetRenderEmbeds() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::RENDER_EMBEDS) == (unsigned int) ClientUserSettingsFlags::RENDER_EMBEDS;
	}
	
	bool ClientUserSettings::GetRenderReactions() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::RENDER_REACTIONS) == (unsigned int) ClientUserSettingsFlags::RENDER_REACTIONS;
	}
	
	bool ClientUserSettings::GetAnimateEmoji() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::ANIMATE_EMOJI) == (unsigned int) ClientUserSettingsFlags::ANIMATE_EMOJI;
	}
	
	bool ClientUserSettings::GetEnableTtsCommand() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::ENABLE_TTS_COMMAND) == (unsigned int) ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
	}
	
	bool ClientUserSettings::GetMessageDisplayCompact() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT) == (unsigned int) ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
	}
	
	bool ClientUserSettings::GetConvertEmoticons() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::CONVERT_EMOTICONS) == (unsigned int) ClientUserSettingsFlags::CONVERT_EMOTICONS;
	}
	
	bool ClientUserSettings::GetDisableGamesTab() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::DISABLE_GAMES_TAB) == (unsigned int) ClientUserSettingsFlags::DISABLE_GAMES_TAB;
	}
	
	bool ClientUserSettings::GetDeveloperMode() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::DEVELOPER_MODE) == (unsigned int) ClientUserSettingsFlags::DEVELOPER_MODE;
	}
	
	bool ClientUserSettings::GetDetectPlatformAccounts() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS) == (unsigned int) ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
	}
	
	bool ClientUserSettings::GetStreamNotificationsEnabled() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED) == (unsigned int) ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
	}
	
	bool ClientUserSettings::GetAllowAccessibilityDetection() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION) == (unsigned int) ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
	}
	
	bool ClientUserSettings::GetContactSyncEnabled() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::CONTACT_SYNC_ENABLED) == (unsigned int) ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
	}
	
	bool ClientUserSettings::GetNativePhoneIntegrationEnabled() const {
		return (this->flags & (unsigned int) ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED) == (unsigned int) ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
	}
}