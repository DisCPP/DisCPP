#ifndef DISCPP_SETTINGS_H
#define DISCPP_SETTINGS_H

#define RAPIDJSON_HAS_STDSTRING 1

#include "utils.h"
#include <string>
#include <vector>
#include <rapidjson/document.h>

namespace discpp {
	enum class Locale : int {
		/* Danish, German, British English, American English */
		DA, DE, EN_GB, EN_US,
		/* Spanish, French, Croatian, Italian */
		ES_ES, FR, HR, IT,
		/* Lithuanian, Hungarian, Dutch, Norwegian */
		LT, HU, NL, NO,
		/* Polish, Portuguese (Brazil), Romanian (Romania), Finnish */
		PL, PT_BR, RO, FI,
		/* Swedish, Vietnamese, Turkish, Czech */
		SV_SE, VI, TR, CS,
		/* Greek, Bulgarian, Russian, Ukrainian */
		EL, BG, RU, UK,
		/* Thai, Chinese (China), Japanese, Chinese (Taiwan) */
		TH, ZH_CN, JA, ZH_TW,
		/* Korean */
		KO
	};

	inline std::string LocaleToString(Locale locale) {
		/* conversions */
		std::unordered_map<Locale, std::string> locale_str_map = {
			{Locale::DA, "da"}, {Locale::DE, "de"}, {Locale::EN_GB, "en-GB"}, {Locale::EN_US, "en-US"},
			{Locale::ES_ES, "es-ES"}, {Locale::FR, "fr"}, {Locale::HR, "hr"}, {Locale::IT, "it"},
			{Locale::LT, "lt"}, {Locale::HU, "hu"}, {Locale::NL, "nl"}, {Locale::NO, "no"},
			{Locale::PL, "pl"}, {Locale::PT_BR, "pt-BR"}, {Locale::RO, "ro"}, {Locale::FI, "fi"},
			{Locale::SV_SE, "sv-SE"}, {Locale::VI, "vi"}, {Locale::TR, "tr"}, {Locale::CS, "cs"},
			{Locale::EL, "el"}, {Locale::BG, "bg"}, {Locale::RU, "ru"}, {Locale::UK, "uk"},
			{Locale::TH, "th"}, {Locale::ZH_CN, "zh-CN"}, {Locale::JA, "ja"}, {Locale::ZH_TW, "zh-TW"},
			{Locale::KO, "ko"}
		};
		return locale_str_map[locale];
	}

	inline Locale StringToLocale(std::string locale) {
		/* conversions */
		std::unordered_map<std::string, Locale> str_locale_map = {
			{"da", Locale::DA}, {"de", Locale::DE}, {"en-GB", Locale::EN_GB}, {"en-US", Locale::EN_US},
			{"es-ES", Locale::ES_ES}, {"fr", Locale::FR}, {"hr", Locale::HR}, {"it", Locale::IT},
			{"lt", Locale::LT}, {"hu", Locale::HU}, {"nl", Locale::NL}, {"no", Locale::NO},
			{"pl", Locale::PL}, {"pt-BR", Locale::PT_BR}, {"ro", Locale::RO}, {"fi", Locale::FI},
			{"sv-SE", Locale::SV_SE}, {"vi", Locale::VI}, {"tr", Locale::TR}, {"cs", Locale::CS},
			{"el", Locale::EL}, {"bg", Locale::BG}, {"ru", Locale::RU}, {"uk", Locale::UK},
			{"th", Locale::TH}, {"zh-CN", Locale::ZH_CN}, {"ja", Locale::JA}, {"zh-TW", Locale::ZH_TW},
			{"ko", Locale::KO}
		};
		return str_locale_map[locale];
	}

	enum class ExplicitContentFilter : int {
		OFF = 0, MODERATE, SEVERE
	};

	enum class Theme : int {
		DARK, LIGHT
	};

	std::unordered_map<Theme, std::string> theme_str_map = {
		{Theme::DARK, "dark"}, {Theme::LIGHT, "light"}
	};

	inline std::string ThemeToString(Theme theme) {
		return theme_str_map[theme];
	}

	enum class ClientUserSettingsFlags {
		SHOW_CURRENT_GAME = 0b1,
		DEFAULT_GUILDS_RESTRICTED = 0b01,
		INLINE_ATTACHMENT_MEDIA = 0b001,
		INLINE_EMBED_MEDIA = 0b0001,
		GIF_AUTO_PLAY = 0b00001,
		RENDER_EMBEDS = 0b000001,
		RENDER_REACTIONS = 0b0000001,
		ANIMATE_EMOJI = 0b00000001,
		ENABLE_TTS_COMMAND = 0b000000001,
		MESSAGE_DISPLAY_COMPACT = 0b0000000001,
		CONVERT_EMOTICONS = 0b00000000001,
		DISABLE_GAMES_TAB = 0b000000000001,
		DEVELOPER_MODE = 0b0000000000001,
		DETECT_PLATFORM_ACCOUNTS = 0b00000000000001,
		STREAM_NOTIFICATIONS_ENABLED = 0b000000000000001,
		ALLOW_ACCESSIBILITY_DETECTION = 0b0000000000000001,
		CONTACT_SYNC_ENABLED = 0b00000000000000001,
		NATIVE_PHONE_INTEGRATION_ENABLED = 0b00000000000000001
	};

	class FriendSourceFlags {
	public:
		FriendSourceFlags() = default;
		FriendSourceFlags(rapidjson::Document& json);

		void ModifyAll(bool all);
		void ModifyMutualFriends(bool mutual_friends);
		void ModifyMutualGuilds(bool mutual_guilds);

		inline bool GetAll() const;
		inline bool GetMutualFriends() const;
		inline bool GetMutualGuilds() const;

	private:
		int flags;
	};

	class ClientUserSettings {
	public:
		ClientUserSettings() = default;
		ClientUserSettings(rapidjson::Document& json);

		void ModifyShowCurrentGame(bool show_current_game);
		void ModifyDefaultGuildsRestricted(bool default_guilds_restricted);
		void ModifyInlineAttachmentMedia(bool inline_attachment_media);
		void ModifyInlineEmbedMedia(bool inline_embed_media);
		void ModifyGifAutoPlay(bool gif_auto_play);
		void ModifyRenderEmbeds(bool render_embeds);
		void ModifyRenderReactions(bool render_reactions);
		void ModifyAnimateEmoji(bool animate_emoji);
		void ModifyEnableTtsCommand(bool enable_tts_command);
		void ModifyMessageDisplayCompact(bool message_display_compact);
		void ModifyConvertEmoticons(bool convert_emoticons);
		void ModifyDisableGamesTab(bool disable_games_tab);
		void ModifyDeveloperMode(bool developer_mode);
		void ModifyDetectPlatformAccounts(bool detect_platform_accounts);
		void ModifyStreamNotificationsEnabled(bool stream_notifications_enabled);
		void ModifyAllowAccessibilityDetection(bool allow_accessibility_detection);
		void ModifyContactSyncEnabled(bool contact_sync_enabled);
		void ModifyNativePhoneIntegrationEnabled(bool native_phone_integration_enabled);

		[[nodiscard]] inline bool GetShowCurrentGame() const;
		[[nodiscard]] inline bool GetDefaultGuildsRestricted() const;
		[[nodiscard]] inline bool GetInlineAttachmentMedia() const;
		[[nodiscard]] inline bool GetInlineEmbedMedia() const;
		[[nodiscard]] inline bool GetGifAutoPlay() const;
		[[nodiscard]] inline bool GetRenderEmbeds() const;
		[[nodiscard]] inline bool GetRenderReactions() const;
		[[nodiscard]] inline bool GetAnimateEmoji() const;
		[[nodiscard]] inline bool GetEnableTtsCommand() const;
		[[nodiscard]] inline bool GetMessageDisplayCompact() const;
		[[nodiscard]] inline bool GetConvertEmoticons() const;
		[[nodiscard]] inline bool GetDisableGamesTab() const;
		[[nodiscard]] inline bool GetDeveloperMode() const;
		[[nodiscard]] inline bool GetDetectPlatformAccounts() const;
		[[nodiscard]] inline bool GetStreamNotificationsEnabled() const;
		[[nodiscard]] inline bool GetAllowAccessibilityDetection() const;
		[[nodiscard]] inline bool GetContactSyncEnabled() const;
		[[nodiscard]] inline bool GetNativePhoneIntegrationEnabled() const;

		std::string status;
		std::string custom_status;
		std::vector<snowflake> guild_positions;
		ExplicitContentFilter explicit_content_filter;
		FriendSourceFlags friend_source_flags{};
        Locale locale;
		Theme theme;
		int afk_timeout;
		int timezone_offset;

	private:
		int flags;
	};
}

#endif