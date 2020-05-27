#include "settings.h"

namespace discpp {
	FriendSource::FriendSource(rapidjson::Document& json) {
		if (GetDataSafely<bool>(json, "all")) flags |= (int)FriendSourceFlags::ALL;
		if (GetDataSafely<bool>(json, "mutual_friends")) flags |= (int)FriendSourceFlags::MUTUAL_FRIENDS;
		if (GetDataSafely<bool>(json, "mutual_guilds")) flags |= (int)FriendSourceFlags::MUTUAL_GUILDS;
	}

	void FriendSource::ModifyAll(bool all) {
		if (all) {
			this->flags |= (int)FriendSourceFlags::ALL;
		} else {
			this->flags = this->flags & ~(int)FriendSourceFlags::ALL;
		}
	}
	
	void FriendSource::ModifyMutualFriends(bool mutual_friends) {
		if (mutual_friends) {
			this->flags |= (int)FriendSourceFlags::MUTUAL_FRIENDS;
		} else {
			this->flags = this->flags & ~(int)FriendSourceFlags::MUTUAL_FRIENDS;
		}
	}

	void FriendSource::ModifyMutualGuilds(bool mutual_guilds) {
		if (mutual_guilds) {
			this->flags |= (int)FriendSourceFlags::MUTUAL_GUILDS;
		} else {
			this->flags = this->flags & ~(int)FriendSourceFlags::MUTUAL_GUILDS;
		}
	}

	bool FriendSource::GetAll() const {
		return (this->flags & (int)FriendSourceFlags::ALL) == (int)FriendSourceFlags::ALL;
	}

	bool FriendSource::GetMutualFriends() const {
		return (this->flags & (int)FriendSourceFlags::MUTUAL_FRIENDS) == (int)FriendSourceFlags::MUTUAL_FRIENDS;
	}

	bool FriendSource::GetMutualGuilds() const {
		return (this->flags & (int)FriendSourceFlags::MUTUAL_GUILDS) == (int)FriendSourceFlags::MUTUAL_GUILDS;
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
				guild_positions.push_back(SnowflakeFromString(guild.GetString()));
			}
		}

		explicit_content_filter = static_cast<discpp::ExplicitContentFilter>(json["explicit_content_filter"].GetInt());
		rapidjson::Document friend_source_flags_json;
		friend_source_flags_json.CopyFrom(json["friend_source_flags"], friend_source_flags_json.GetAllocator());
		friend_source_flags = FriendSource(friend_source_flags_json);

		if (GetDataSafely<bool>(json, "show_current_game")) flags |= (int)ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		if (GetDataSafely<bool>(json, "default_guilds_restricted")) flags |= (int)ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		if (GetDataSafely<bool>(json, "inline_attachment_media")) flags |= (int)ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		if (GetDataSafely<bool>(json, "inline_embed_media")) flags |= (int)ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		if (GetDataSafely<bool>(json, "gif_auto_play")) flags |= (int)ClientUserSettingsFlags::GIF_AUTO_PLAY;
		if (GetDataSafely<bool>(json, "render_embeds")) flags |= (int)ClientUserSettingsFlags::RENDER_EMBEDS;
		if (GetDataSafely<bool>(json, "render_reactions")) flags |= (int)ClientUserSettingsFlags::RENDER_REACTIONS;
		if (GetDataSafely<bool>(json, "animate_emoji")) flags |= (int)ClientUserSettingsFlags::ANIMATE_EMOJI;
		if (GetDataSafely<bool>(json, "enable_tts_command")) flags |= (int)ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		if (GetDataSafely<bool>(json, "message_display_compact")) flags |= (int)ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		if (GetDataSafely<bool>(json, "convert_emoticons")) flags |= (int)ClientUserSettingsFlags::CONVERT_EMOTICONS;
		if (GetDataSafely<bool>(json, "disable_games_tab")) flags |= (int)ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		if (GetDataSafely<bool>(json, "developer_mode")) flags |= (int)ClientUserSettingsFlags::DEVELOPER_MODE;
		if (GetDataSafely<bool>(json, "detect_platform_accounts")) flags |= (int)ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		if (GetDataSafely<bool>(json, "stream_notifications_enabled")) flags |= (int)ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		if (GetDataSafely<bool>(json, "allow_accessibility_detection")) flags |= (int)ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		if (GetDataSafely<bool>(json, "contact_sync_enabled")) flags |= (int)ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		if (GetDataSafely<bool>(json, "native_phone_integration_enabled")) flags |= (int)ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
	}

	void ClientUserSettings::ModifyShowCurrentGame(bool show_current_game) {
		if (show_current_game) {
			this->flags |= (int)ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		}
	}

	void ClientUserSettings::ModifyDefaultGuildsRestricted(bool default_guilds_restricted) {
		if (default_guilds_restricted) {
			this->flags |= (int)ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		}
	}

	void ClientUserSettings::ModifyInlineAttachmentMedia(bool inline_attachment_media) {
		if (inline_attachment_media) {
			this->flags |= (int)ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		}
	}

	void ClientUserSettings::ModifyInlineEmbedMedia(bool inline_embed_media) {
		if (inline_embed_media) {
			this->flags |= (int)ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		}
	}

	void ClientUserSettings::ModifyGifAutoPlay(bool gif_auto_play) {
		if (gif_auto_play) {
			this->flags |= (int)ClientUserSettingsFlags::GIF_AUTO_PLAY;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::GIF_AUTO_PLAY;
		}
	}

	void ClientUserSettings::ModifyRenderEmbeds(bool render_embeds) {
		if (render_embeds) {
			this->flags |= (int)ClientUserSettingsFlags::RENDER_EMBEDS;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::RENDER_EMBEDS;
		}
	}

	void ClientUserSettings::ModifyRenderReactions(bool render_reactions) {
		if (render_reactions) {
			this->flags |= (int)ClientUserSettingsFlags::RENDER_REACTIONS;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::RENDER_REACTIONS;
		}
	}

	void ClientUserSettings::ModifyAnimateEmoji(bool animate_emoji) {
		if (animate_emoji) {
			this->flags |= (int)ClientUserSettingsFlags::ANIMATE_EMOJI;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::ANIMATE_EMOJI;
		}
	}
	
	void ClientUserSettings::ModifyEnableTtsCommand(bool enable_tts_command) {
		if (enable_tts_command) {
			this->flags |= (int)ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		}
	}
	
	void ClientUserSettings::ModifyMessageDisplayCompact(bool message_display_compact) {
		if (message_display_compact) {
			this->flags |= (int)ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		}
	}
	
	void ClientUserSettings::ModifyConvertEmoticons(bool convert_emoticons) {
		if (convert_emoticons) {
			this->flags |= (int)ClientUserSettingsFlags::CONVERT_EMOTICONS;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::CONVERT_EMOTICONS;
		}
	}
	
	void ClientUserSettings::ModifyDisableGamesTab(bool disable_games_tab) {
		if (disable_games_tab) {
			this->flags |= (int)ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		}
	}
	
	void ClientUserSettings::ModifyDeveloperMode(bool developer_mode) {
		if (developer_mode) {
			this->flags |= (int)ClientUserSettingsFlags::DEVELOPER_MODE;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::DEVELOPER_MODE;
		}
	}
	
	void ClientUserSettings::ModifyDetectPlatformAccounts(bool detect_platform_accounts) {
		if (detect_platform_accounts) {
			this->flags |= (int)ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		}
	}
	
	void ClientUserSettings::ModifyStreamNotificationsEnabled(bool stream_notifications_enabled) {
		if (stream_notifications_enabled) {
			this->flags |= (int)ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		}
	}
	
	void ClientUserSettings::ModifyAllowAccessibilityDetection(bool allow_accessibility_detection) {
		if (allow_accessibility_detection) {
			this->flags |= (int)ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		}
	}
	
	void ClientUserSettings::ModifyContactSyncEnabled(bool contact_sync_enabled) {
		if (contact_sync_enabled) {
			this->flags |= (int)ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		}
	}
	
	void ClientUserSettings::ModifyNativePhoneIntegrationEnabled(bool native_phone_integration_enabled) {
		if (native_phone_integration_enabled) {
			this->flags |= (int)ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
		} else {
			this->flags = this->flags & ~(int)ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
		}
	}

	bool ClientUserSettings::GetShowCurrentGame() const {
		return (this->flags & (int)ClientUserSettingsFlags::SHOW_CURRENT_GAME) == (int)ClientUserSettingsFlags::SHOW_CURRENT_GAME;
	}
	
	bool ClientUserSettings::GetDefaultGuildsRestricted() const {
		return (this->flags & (int)ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED) == (int)ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
	}
	
	bool ClientUserSettings::GetInlineAttachmentMedia() const {
		return (this->flags & (int)ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA) == (int)ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
	}
	
	bool ClientUserSettings::GetInlineEmbedMedia() const {
		return (this->flags & (int)ClientUserSettingsFlags::INLINE_EMBED_MEDIA) == (int)ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
	}
	
	bool ClientUserSettings::GetGifAutoPlay() const {
		return (this->flags & (int)ClientUserSettingsFlags::GIF_AUTO_PLAY) == (int)ClientUserSettingsFlags::GIF_AUTO_PLAY;
	}
	
	bool ClientUserSettings::GetRenderEmbeds() const {
		return (this->flags & (int)ClientUserSettingsFlags::RENDER_EMBEDS) == (int)ClientUserSettingsFlags::RENDER_EMBEDS;
	}
	
	bool ClientUserSettings::GetRenderReactions() const {
		return (this->flags & (int)ClientUserSettingsFlags::RENDER_REACTIONS) == (int)ClientUserSettingsFlags::RENDER_REACTIONS;
	}
	
	bool ClientUserSettings::GetAnimateEmoji() const {
		return (this->flags & (int)ClientUserSettingsFlags::ANIMATE_EMOJI) == (int)ClientUserSettingsFlags::ANIMATE_EMOJI;
	}
	
	bool ClientUserSettings::GetEnableTtsCommand() const {
		return (this->flags & (int)ClientUserSettingsFlags::ENABLE_TTS_COMMAND) == (int)ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
	}
	
	bool ClientUserSettings::GetMessageDisplayCompact() const {
		return (this->flags & (int)ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT) == (int)ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
	}
	
	bool ClientUserSettings::GetConvertEmoticons() const {
		return (this->flags & (int)ClientUserSettingsFlags::CONVERT_EMOTICONS) == (int)ClientUserSettingsFlags::CONVERT_EMOTICONS;
	}
	
	bool ClientUserSettings::GetDisableGamesTab() const {
		return (this->flags & (int)ClientUserSettingsFlags::DISABLE_GAMES_TAB) == (int)ClientUserSettingsFlags::DISABLE_GAMES_TAB;
	}
	
	bool ClientUserSettings::GetDeveloperMode() const {
		return (this->flags & (int)ClientUserSettingsFlags::DEVELOPER_MODE) == (int)ClientUserSettingsFlags::DEVELOPER_MODE;
	}
	
	bool ClientUserSettings::GetDetectPlatformAccounts() const {
		return (this->flags & (int)ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS) == (int)ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
	}
	
	bool ClientUserSettings::GetStreamNotificationsEnabled() const {
		return (this->flags & (int)ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED) == (int)ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
	}
	
	bool ClientUserSettings::GetAllowAccessibilityDetection() const {
		return (this->flags & (int)ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION) == (int)ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
	}
	
	bool ClientUserSettings::GetContactSyncEnabled() const {
		return (this->flags & (int)ClientUserSettingsFlags::CONTACT_SYNC_ENABLED) == (int)ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
	}
	
	bool ClientUserSettings::GetNativePhoneIntegrationEnabled() const {
		return (this->flags & (int)ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED) == (int)ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
	}
}