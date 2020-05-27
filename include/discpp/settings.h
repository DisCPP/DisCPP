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

	inline std::string LocaleToString(Locale locale) {
		return locale_str_map[locale];
	}
	
	std::unordered_map<std::string, Locale> str_locale_map = {
		{"da", DA}, {"de", DE}, {"en-GB", EN_GB}, {"en-US", EN_US},
		{"es-ES", ES_ES}, {"fr", FR}, {"hr", HR}, {"it", IT},
		{"lt", LT}, {"hu", HU}, {"nl", NL}, {"no", NO},
		{"pl", PL}, {"pt-BR", PT_BR}, {"ro", RO}, {"fi", FI},
		{"sv-SE", SV_SE}, {"vi", VI}, {"tr", TR}, {"cs", CS},
		{"el", EL}, {"bg", BG}, {"ru", RU}, {"uk", UK},
		{"th", TH}, {"zh-CN", ZH_CN}, {"ja", JA}, {"zh-TW", ZH_TW},
		{"ko", KO}
	};

	inline Locale StringToLocale(std::string locale) {
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

		inline bool GetAll();
		inline bool GetMutualFriends();
		inline bool GetMutualGuilds();

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

		inline bool GetShowCurrentGame();
		inline bool GetDefaultGuildsRestricted();
		inline bool GetInlineAttachmentMedia();
		inline bool GetInlineEmbedMedia();
		inline bool GetGifAutoPlay();
		inline bool GetRenderEmbeds();
		inline bool GetRenderReactions();
		inline bool GetAnimateEmoji();
		inline bool GetEnableTtsCommand();
		inline bool GetMessageDisplayCompact();
		inline bool GetConvertEmoticons();
		inline bool GetDisableGamesTab();
		inline bool GetDeveloperMode();
		inline bool GetDetectPlatformAccounts();
		inline bool GetStreamNotificationsEnabled();
		inline bool GetAllowAccessibilityDetection();
		inline bool GetContactSyncEnabled();
		inline bool GetNativePhoneIntegrationEnabled();

		std::string locale;
		std::string status;
		std::string custom_status;
		std::vector<snowflake> guild_positions;
		ExplicitContentFilter explicit_content_filter;
		FriendSourceFlags friend_source_flags;
		Theme theme;
		int afk_timeout;
		int timezone_offset;

	private:
		int flags;
	};
}

#endif