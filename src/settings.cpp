#include "settings.h"

namespace discpp {
	FriendSourceFlags::FriendSourceFlags(rapidjson::Document& json) {
		if (GetDataSafely<bool>(json, "all")) flags |= 0b1;
		if (GetDataSafely<bool>(json, "mutual_friends")) flags |= 0b01;
		if (GetDataSafely<bool>(json, "mutual_guilds")) flags |= 0b001;
	}

	void FriendSourceFlags::ModifyAll(bool all) {
		if (all) {
			this->flags |= 0b1;
		} else {
			this->flags = this->flags & ~0b1;
		}
	}
	
	void FriendSourceFlags::ModifyMutualFriends(bool mutual_friends) {
		if (mutual_friends) {
			this->flags |= 0b01;
		} else {
			this->flags = this->flags & ~0b01;
		}
	}

	void FriendSourceFlags::ModifyMutualGuilds(bool mutual_guilds) {
		if (mutual_guilds) {
			this->flags |= 0b001;
		} else {
			this->flags = this->flags & ~0b001;
		}
	}

	inline bool FriendSourceFlags::GetAll() {
		return (this->flags & 0b1) == 0b1;
	}

	inline bool FriendSourceFlags::GetMutualFriends() {
		return (this->flags & 0b01) == 0b01;
	}

	inline bool FriendSourceFlags::GetMutualGuilds() {
		return (this->flags & 0b001) == 0b001;
	}

	ClientUserSettings::ClientUserSettings(rapidjson::Document& json) {
		locale = LocaleToString(GetDataSafely<std::string>(json, "locale"));
		status = GetDataSafely<std::string>(json, "status");
		custom_status = GetDataSafely<std::string>(json, "custom_status");
		afk_timeout = GetDataSafely<int>(json, "afk_timeout");
		timezone_offset = GetDataSafely<int>(json, "timezone_offset");

		if (ContainsNotNull(json, "guild_positions")) {
			for (auto const& guild : json["guild_positions"].GetArray()) {
				guild_positions.push_back(SnowflakeFromString(guild.GetString()));
			}
		}

		explicit_content_filter = static_cast<discpp::ExplicitContentFilter>(json["explicit_content_filter"].GetInt());
		rapidjson::Document friend_source_flags_json;
		friend_source_flags_json.CopyFrom(json["friend_source_flags"], friend_source_flags_json.GetAllocator());
		friend_source_flags = FriendSourceFlags(friend_source_flags_json);

		if (GetDataSafely<bool>(json, "show_current_game")) flags |= ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		if (GetDataSafely<bool>(json, "default_guilds_restricted")) flags |= ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		if (GetDataSafely<bool>(json, "inline_attachment_media")) flags |= ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		if (GetDataSafely<bool>(json, "inline_embed_media")) flags |= ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		if (GetDataSafely<bool>(json, "gif_auto_play")) flags |= ClientUserSettingsFlags::GIF_AUTO_PLAY;
		if (GetDataSafely<bool>(json, "render_embeds")) flags |= ClientUserSettingsFlags::RENDER_EMBEDS;
		if (GetDataSafely<bool>(json, "render_reactions")) flags |= ClientUserSettingsFlags::RENDER_REACTIONS;
		if (GetDataSafely<bool>(json, "animate_emoji")) flags |= ClientUserSettingsFlags::ANIMATE_EMOJI;
		if (GetDataSafely<bool>(json, "enable_tts_command")) flags |= ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		if (GetDataSafely<bool>(json, "message_display_compact")) flags |= ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		if (GetDataSafely<bool>(json, "convert_emoticons")) flags |= ClientUserSettingsFlags::CONVERT_EMOTICONS;
		if (GetDataSafely<bool>(json, "disable_games_tab")) flags |= ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		if (GetDataSafely<bool>(json, "developer_mode")) flags |= ClientUserSettingsFlags::DEVELOPER_MODE;
		if (GetDataSafely<bool>(json, "detect_platform_accounts")) flags |= ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		if (GetDataSafely<bool>(json, "stream_notifications_enabled")) flags |= ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		if (GetDataSafely<bool>(json, "allow_accessibility_detection")) flags |= ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		if (GetDataSafely<bool>(json, "contact_sync_enabled")) flags |= ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		if (GetDataSafely<bool>(json, "native_phone_integration_enabled")) flags |= ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
	}

	void ClientUserSettings::ModifyShowCurrentGame(bool show_current_game) {
		if (show_current_game) {
			this->flags |= ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::SHOW_CURRENT_GAME;
		}
	}

	void ClientUserSettings::ModifyDefaultGuildsRestricted(bool default_guilds_restricted) {
		if (default_guilds_restricted) {
			this->flags |= ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
		}
	}

	void ClientUserSettings::ModifyInlineAttachmentMedia(bool inline_attachment_media) {
		if (inline_attachment_media) {
			this->flags |= ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
		}
	}

	void ClientUserSettings::ModifyInlineEmbedMedia(bool inline_embed_media) {
		if (inline_embed_media) {
			this->flags |= ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
		}
	}

	void ClientUserSettings::ModifyGifAutoPlay(bool gif_auto_play) {
		if (gif_auto_play) {
			this->flags |= ClientUserSettingsFlags::GIF_AUTO_PLAY;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::GIF_AUTO_PLAY;
		}
	}

	void ClientUserSettings::ModifyRenderEmbeds(bool render_embeds) {
		if (render_embeds) {
			this->flags |= ClientUserSettingsFlags::RENDER_EMBEDS;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::RENDER_EMBEDS;
		}
	}

	void ClientUserSettings::ModifyRenderReactions(bool render_reactions) {
		if (render_reactions) {
			this->flags |= ClientUserSettingsFlags::RENDER_REACTIONS;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::RENDER_REACTIONS;
		}
	}

	void ClientUserSettings::ModifyAnimateEmoji(bool animate_emoji) {
		if (animate_emoji) {
			this->flags |= ClientUserSettingsFlags::ANIMATE_EMOJI;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::ANIMATE_EMOJI;
		}
	}
	
	void ClientUserSettings::ModifyEnableTtsCommand(bool enable_tts_command) {
		if (enable_tts_command) {
			this->flags |= ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
		}
	}
	
	void ClientUserSettings::ModifyMessageDisplayCompact(bool message_display_compact) {
		if (message_display_compact) {
			this->flags |= ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
		}
	}
	
	void ClientUserSettings::ModifyConvertEmoticons(bool convert_emoticons) {
		if (convert_emoticons) {
			this->flags |= ClientUserSettingsFlags::CONVERT_EMOTICONS;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::CONVERT_EMOTICONS;
		}
	}
	
	void ClientUserSettings::ModifyDisableGamesTab(bool disable_games_tab) {
		if (disable_games_tab) {
			this->flags |= ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::DISABLE_GAMES_TAB;
		}
	}
	
	void ClientUserSettings::ModifyDeveloperMode(bool developer_mode) {
		if (developer_mode) {
			this->flags |= ClientUserSettingsFlags::DEVELOPER_MODE;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::DEVELOPER_MODE;
		}
	}
	
	void ClientUserSettings::ModifyDetectPlatformAccounts(bool detect_platform_accounts) {
		if (detect_platform_accounts) {
			this->flags |= ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
		}
	}
	
	void ClientUserSettings::ModifyStreamNotificationsEnabled(bool stream_notifications_enabled) {
		if (stream_notifications_enabled) {
			this->flags |= ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
		}
	}
	
	void ClientUserSettings::ModifyAllowAccessibilityDetection(bool allow_accessibility_detection) {
		if (allow_accessibility_detection) {
			this->flags |= ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
		}
	}
	
	void ClientUserSettings::ModifyContactSyncEnabled(bool contact_sync_enabled) {
		if (contact_sync_enabled) {
			this->flags |= ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
		}
	}
	
	void ClientUserSettings::ModifyNativePhoneIntegrationEnabled(bool native_phone_integration_enabled) {
		if (native_phone_integration_enabled) {
			this->flags |= ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
		} else {
			this->flags = this->flags & ~ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
		}
	}

	inline bool ClientUserSettings::GetShowCurrentGame() {
		return (this->flags & ClientUserSettingsFlags::SHOW_CURRENT_GAME) == ClientUserSettingsFlags::SHOW_CURRENT_GAME;
	}
	
	inline bool ClientUserSettings::GetDefaultGuildsRestricted() {
		return (this->flags & ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED) == ClientUserSettingsFlags::DEFAULT_GUILDS_RESTRICTED;
	}
	
	inline bool ClientUserSettings::GetInlineAttachmentMedia() {
		return (this->flags & ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA) == ClientUserSettingsFlags::INLINE_ATTACHMENT_MEDIA;
	}
	
	inline bool ClientUserSettings::GetInlineEmbedMedia() {
		return (this->flags & ClientUserSettingsFlags::INLINE_EMBED_MEDIA) == ClientUserSettingsFlags::INLINE_EMBED_MEDIA;
	}
	
	inline bool ClientUserSettings::GetGifAutoPlay() {
		return (this->flags & ClientUserSettingsFlags::GIF_AUTO_PLAY) == ClientUserSettingsFlags::GIF_AUTO_PLAY;
	}
	
	inline bool ClientUserSettings::GetRenderEmbeds() {
		return (this->flags & ClientUserSettingsFlags::RENDER_EMBEDS) == ClientUserSettingsFlags::RENDER_EMBEDS;
	}
	
	inline bool ClientUserSettings::GetRenderReactions() {
		return (this->flags & ClientUserSettingsFlags::RENDER_REACTIONS) == ClientUserSettingsFlags::RENDER_REACTIONS;
	}
	
	inline bool ClientUserSettings::GetAnimateEmoji() {
		return (this->flags & ClientUserSettingsFlags::ANIMATE_EMOJI) == ClientUserSettingsFlags::ANIMATE_EMOJI;
	}
	
	inline bool ClientUserSettings::GetEnableTtsCommand() {
		return (this->flags & ClientUserSettingsFlags::ENABLE_TTS_COMMAND) == ClientUserSettingsFlags::ENABLE_TTS_COMMAND;
	}
	
	inline bool ClientUserSettings::GetMessageDisplayCompact() {
		return (this->flags & ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT) == ClientUserSettingsFlags::MESSAGE_DISPLAY_COMPACT;
	}
	
	inline bool ClientUserSettings::GetConvertEmoticons() {
		return (this->flags & ClientUserSettingsFlags::CONVERT_EMOTICONS) == ClientUserSettingsFlags::CONVERT_EMOTICONS;
	}
	
	inline bool ClientUserSettings::GetDisableGamesTab() {
		return (this->flags & ClientUserSettingsFlags::DISABLE_GAMES_TAB) == ClientUserSettingsFlags::DISABLE_GAMES_TAB;
	}
	
	inline bool ClientUserSettings::GetDeveloperMode() {
		return (this->flags & ClientUserSettingsFlags::DEVELOPER_MODE) == ClientUserSettingsFlags::DEVELOPER_MODE;
	}
	
	inline bool ClientUserSettings::GetDetectPlatformAccounts() {
		return (this->flags & ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS) == ClientUserSettingsFlags::DETECT_PLATFORM_ACCOUNTS;
	}
	
	inline bool ClientUserSettings::GetStreamNotificationsEnabled() {
		return (this->flags & ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED) == ClientUserSettingsFlags::STREAM_NOTIFICATIONS_ENABLED;
	}
	
	inline bool ClientUserSettings::GetAllowAccessibilityDetection() {
		return (this->flags & ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION) == ClientUserSettingsFlags::ALLOW_ACCESSIBILITY_DETECTION;
	}
	
	inline bool ClientUserSettings::GetContactSyncEnabled() {
		return (this->flags & ClientUserSettingsFlags::CONTACT_SYNC_ENABLED) == ClientUserSettingsFlags::CONTACT_SYNC_ENABLED;
	}
	
	inline bool ClientUserSettings::GetNativePhoneIntegrationEnabled() {
		return (this->flags & ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED) == ClientUserSettingsFlags::NATIVE_PHONE_INTEGRATION_ENABLED;
	}
}