#ifndef DISCPP_SETTINGS_H
#define DISCPP_SETTINGS_H

#include "utils.h"
#include <string>
#include <vector>

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

	inline std::string LocaleToString(const Locale& locale) {
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

	inline Locale StringToLocale(const std::string& locale) {
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

	inline std::string ThemeToString(const Theme& theme) {
		std::unordered_map<Theme, std::string> theme_str_map = {
			{Theme::DARK, "dark"}, {Theme::LIGHT, "light"}
		};
		return theme_str_map[theme];
	}

	inline Theme StringToTheme(const std::string& theme) {
	    std::unordered_map<std::string, Theme> str_theme_map = {
            {"dark", Theme::DARK}, {"light", Theme::LIGHT}
	    };
	    return str_theme_map[theme];
	}

	enum class FriendSourceFlags : int {
		ALL = 0b1, 
		MUTUAL_FRIENDS = 0b01, 
		MUTUAL_GUILDS = 0b001
	};

	class FriendSource {
	public:
		FriendSource() = default;

        /**
         * @brief Constructs a discpp::FriendSource object by parsing json.
         *
         * ```cpp
         *      discpp::FriendSource friend_source(json);
         * ```
         *
         * @param[in] json The json that makes up of FriendSource object.
         *
         * @return discpp::FriendSource, this is a constructor.
         */
		FriendSource(rapidjson::Document& json);

        /**
         * @brief Modifies All bool value
         *
         * ```cpp
         *      FriendSource.ModifyAll(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyAll(const bool all);

        /**
         * @brief Modifies MutualFriends bool value
         *
         * ```cpp
         *      FriendSource.ModifyMutualFriends(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyMutualFriends(const bool mutual_friends);

        /**
         * @brief Modifies MutualGuilds bool value
         *
         * ```cpp
         *      FriendSource.ModifyMutualGuilds(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyMutualGuilds(const bool mutual_guilds);

        /**
         * @brief Check if all is enabled
         *
         * ```cpp
         *      bool all_enabled = FriendSource.GetAll();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetAll() const;

        /**
         * @brief Check if MutualFriends is enabled
         *
         * ```cpp
         *      bool mutual_friends_enabled = FriendSource.GetMutualFriends();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetMutualFriends() const;

        /**
         * @brief Check if MutualGuilds is enabled
         *
         * ```cpp
         *      bool mutual_guilds_enabled = FriendSource.GetMutualGuilds();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetMutualGuilds() const;

	private:
		int flags;
	};

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

	class ClientUserSettings {
	public:
		ClientUserSettings() = default;

        /**
         * @brief Constructs a discpp::ClientUserSettings object by parsing json.
         *
         * ```cpp
         *      discpp::ClientUserSettings client_user_settings(json);
         * ```
         *
         * @param[in] json The json that makes up of ClientUserSettings object.
         *
         * @return discpp::ClientUserSettings, this is a constructor.
         */
		ClientUserSettings(rapidjson::Document& json);

        /**
         * @brief Modifies ShowCurrentGame bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyShowCurrentGame(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyShowCurrentGame(const bool show_current_game);

        /**
         * @brief Modifies DefaultGuildsRestricted bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyDefaultGuildsRestricted(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyDefaultGuildsRestricted(const bool default_guilds_restricted);

        /**
         * @brief Modifies InlineAttachmentMedia bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyInlineAttachmentMedia(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyInlineAttachmentMedia(const bool inline_attachment_media);

        /**
         * @brief Modifies InlineEmbedMedia bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyEmbedMedia(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyInlineEmbedMedia(const bool inline_embed_media);

        /**
         * @brief Modifies GifAutoPlay bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyGifAutoPlay(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyGifAutoPlay(const bool gif_auto_play);

        /**
         * @brief Modifies RenderEmbeds bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyRenderEmbeds(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyRenderEmbeds(const bool render_embeds);

        /**
         * @brief Modifies RenderReactions bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyRenderReactions(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyRenderReactions(const bool render_reactions);

        /**
         * @brief Modifies AnimateEmoji bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyAnimateEmoji(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyAnimateEmoji(const bool animate_emoji);

        /**
         * @brief Modifies EnableTtsCommand bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyEnableTtsCommand(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyEnableTtsCommand(const bool enable_tts_command);

        /**
         * @brief Modifies MessageDisplayCompact bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyMessageDisplayCompact(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyMessageDisplayCompact(const bool message_display_compact);

        /**
         * @brief Modifies ConvertEmoticons bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyConvertEmoticons(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyConvertEmoticons(const bool convert_emoticons);

        /**
         * @brief Modifies DisableGamesTab bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyDisableGamesTab(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyDisableGamesTab(const bool disable_games_tab);

        /**
         * @brief Modifies DeveloperMode bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyDeveloperMode(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyDeveloperMode(const bool developer_mode);

        /**
         * @brief Modifies DetectPlatformAccounts bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyDetectPlatformAccounts(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyDetectPlatformAccounts(const bool detect_platform_accounts);

        /**
         * @brief Modifies StreamNotificationsEnabled bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyStreamNotificationsEnabled(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyStreamNotificationsEnabled(const bool stream_notifications_enabled);

        /**
         * @brief Modifies AllowAccessibilityDetection bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyAllowAccessibilityDetection(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyAllowAccessibilityDetection(const bool allow_accessibility_detection);

        /**
         * @brief Modifies ContactSyncEnabled bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyContactSyncEnabled(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyContactSyncEnabled(const bool contact_sync_enabled);

        /**
         * @brief Modifies NativePhoneIntegrationEnabled bool value
         *
         * ```cpp
         *      ClientUserSettings.ModifyNativePhoneIntegrationEnabled(bool);
         * ```
         *
         * @param[in] bool The value to change to
         *
         * @return void
         */
		void ModifyNativePhoneIntegrationEnabled(const bool native_phone_integration_enabled);

        /**
         * @brief Check if ShowCurrentGame is enabled
         *
         * ```cpp
         *      bool show_current_game = ClientUserSettings.GetShowCurrentGame();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetShowCurrentGame() const;

        /**
         * @brief Check if DefaultGuildsRestricted is enabled
         *
         * ```cpp
         *      bool default_guilds_restricted = ClientUserSettings.GetDefaultGuildsRestricted();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetDefaultGuildsRestricted() const;

        /**
         * @brief Check if InlineAttachmentMedia is enabled
         *
         * ```cpp
         *      bool inline_attachment_media = ClientUserSettings.GetInlineAttachmentMedia();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetInlineAttachmentMedia() const;

        /**
         * @brief Check if InlineEmbedMedia is enabled
         *
         * ```cpp
         *      bool inline_embed_media = ClientUserSettings.GetInlineEmbedMedia();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetInlineEmbedMedia() const;

        /**
         * @brief Check if GifAutoPlay is enabled
         *
         * ```cpp
         *      bool gif_auto_play = ClientUserSettings.GetGifAutoPlay();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetGifAutoPlay() const;

        /**
         * @brief Check if GetRenderEmbeds() is enabled
         *
         * ```cpp
         *      bool render_embeds = ClientUserSettings.GetRenderEmbeds();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetRenderEmbeds() const;

        /**
         * @brief Check if RenderReactions is enabled
         *
         * ```cpp
         *      bool render_reactions = ClientUserSettings.GetRenderReactions();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetRenderReactions() const;

        /**
         * @brief Check if AnimateEmoji is enabled
         *
         * ```cpp
         *      bool animate_emoji = ClientUserSettings.GetAnimateEmoji();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetAnimateEmoji() const;

        /**
         * @brief Check if EnableTtsCommand is enabled
         *
         * ```cpp
         *      bool enable_tts_command = ClientUserSettings.GetEnableTtsCommand();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetEnableTtsCommand() const;

        /**
         * @brief Check if MessageDisplayCompact is enabled
         *
         * ```cpp
         *      bool message_display_compact = ClientUserSettings.GetMessageDisplayCompact();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetMessageDisplayCompact() const;

        /**
         * @brief Check if ConvertEmoticons is enabled
         *
         * ```cpp
         *      bool convert_emoticons = ClientUserSettings.GetConvertEmoticons();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetConvertEmoticons() const;

        /**
         * @brief Check if DisableGamesTab is enabled
         *
         * ```cpp
         *      bool disable_games_tab = ClientUserSettings.GetDisableGamesTab();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetDisableGamesTab() const;

        /**
         * @brief Check if DeveloperMode is enabled
         *
         * ```cpp
         *      bool developer_mode = ClientUserSettings.GetDeveloperMode();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetDeveloperMode() const;

        /**
         * @brief Check if DetectPlatformAccounts is enabled
         *
         * ```cpp
         *      bool detect_platform_accounts = ClientUserSettings.GetDetectPlatformAccounts();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetDetectPlatformAccounts() const;

        /**
         * @brief Check if StreamNotificationsEnabled is enabled
         *
         * ```cpp
         *      bool stream_notifications_enabled = ClientUserSettings.GetStreamNotificationsEnabled();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetStreamNotificationsEnabled() const;

        /**
         * @brief Check if AllowAccessibilityDetection() is enabled
         *
         * ```cpp
         *      bool allow_accessibility_detection = ClientUserSettings.GetAllowAccessibilityDetection();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetAllowAccessibilityDetection() const;

        /**
         * @brief Check if ContactSyncEnabled is enabled
         *
         * ```cpp
         *      bool contact_sync_enabled = ClientUserSettings.GetContactSyncEnabled();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetContactSyncEnabled() const;

        /**
         * @brief Check if NativePhoneIntegrationEnabled is enabled
         *
         * ```cpp
         *      bool native_phone_integration_enabled = ClientUserSettings.GetNativePhoneIntegrationEnabled();
         * ```
         *
         * @return bool
         */
		[[nodiscard]] bool GetNativePhoneIntegrationEnabled() const;

		std::string status;
		std::string custom_status;
		std::vector<Snowflake> guild_positions;
		ExplicitContentFilter explicit_content_filter;
		FriendSource friend_source_flags;
        Locale locale;
		Theme theme;
		int afk_timeout;
		int timezone_offset;

	private:
		int flags;
	};
}

#endif