#ifndef DISCPP_GUILD_H
#define DISCPP_GUILD_H

#include "discord_object.h"
#include "utils.h"
#include "image.h"
#include "permission.h"
#include "channel.h"
#include "emoji.h"

#include <utility>
#include <variant>

namespace discpp {
	class Guild;
    class VoiceState;
    class Member;
    class Role;
    class AuditLog;
    class User;

	struct GuildBan {
        /**
         * @brief Constructs a discpp::GuildBan object.
         *
         * ```cpp
         *      discpp::GuildBan guild_invite("Spamming", user);
         * ```
         *
         * @param[in] reason The reason for the ban
         * @param[in] user The banned user
         *
         * @return discpp::GuildBan, this is a constructor.
         */
		GuildBan(const std::string& reason, std::shared_ptr<discpp::User> user) : reason(reason), user(user) {

		}

        std::string reason; /**< The reason for the ban. */
        std::shared_ptr<discpp::User> user; /**< The banned user. */
	};

	class GuildInvite {
	public:
	    enum class TargetUserType : int {
	        STREAM = 1
	    };

		GuildInvite() = default;

        /**
         * @brief Constructs a discpp::GuildInvite object from json.
         *
         * ```cpp
         *      discpp::GuildInvite guild_invite(json);
         * ```
         *
         * @param[in] json The json data for the guild invite.
         *
         * @return discpp::GuildInvite, this is a constructor.
         */
		GuildInvite(rapidjson::Document& json);

        std::string code; /**< The invite code (unique ID). */
        std::shared_ptr<discpp::Guild> guild; /**< The guild this invite is for. */
        discpp::Channel channel; /**< The channel this invite is for. */
        std::shared_ptr<discpp::User> inviter; /**< he user who created the invite. */
        std::shared_ptr<discpp::User> target_user; /**< The target user for this invite. */
        TargetUserType target_user_type; /**< The type of user target for this invite. */
        int approximate_presence_count; /**< Approximate count of online members (only present when target_user is set). */
        int approximate_member_count; /**< Approximate count of total members. */
	};

	class IntegrationAccount : public DiscordObject {
	public:
        IntegrationAccount() = default;
        IntegrationAccount(rapidjson::Document& json) {

            /**
             * @brief Constructs a discpp::IntegrationAccount object from json.
             *
             * ```cpp
             *      discpp::IntegrationAccount guild_integration_account(json);
             * ```
             *
             * @param[in] json The json data for the integration account.
             *
             * @return discpp::IntegrationAccount, this is a constructor.
             */
			id = Snowflake(json["id"].GetString());
			name = json["name"].GetString();
		}

        std::string name; /**< Name of the account. */
    };

	class Integration : public DiscordObject {
	public:
	    enum class IntegrationExpireBehavior : int {
	        REMOVE_ROLE = 0,
	        KICK = 1
	    };
		    Integration() = default;

        /**
         * @brief Constructs a discpp::Integration object from json.
         *
         * ```cpp
         *      discpp::Integration integration(json);
         * ```
         *
         * @param[in] json The json data for the integration.
         *
         * @return discpp::Integration, this is a constructor.
         */
        explicit Integration(rapidjson::Document& json);

        std::string name; /**< Integration name. */
        std::string type; /**< Integration type (twitch, youtube, etc). */
        bool enabled; /**< Is this integration enabled? */
        bool syncing; /**< Is this integration syncing? */
        discpp::Snowflake role_id; /**< ID that this integration uses for "subscribers". */
        bool enable_emoticons; /**< Whether emoticons should be synced for this integration (twitch only currently). */
        IntegrationExpireBehavior expire_behavior; /**< The behavior of expiring subscribers. */
        int expire_grace_period; /**< The grace period (in days) before expiring subscribers. */
        std::shared_ptr<discpp::User> user; /**< User for this integration. */
        discpp::IntegrationAccount account; /**< Integration account information. */
        // @TODO: Convert to iso8601Time
        std::string synced_at; /**< When this integration was last synced. */
	};

	class GuildEmbed : public DiscordObject {
	public:
        GuildEmbed() = default;
		GuildEmbed(rapidjson::Document& json) {
            /**
             * @brief Constructs a discpp::GuildEmbed object from json.
             *
             * ```cpp
             *      discpp::GuildEmbed guild_embed(json);
             * ```
             *
             * @param[in] json The json data for the embed.
             *
             * @return discpp::GuildEmbed, this is a constructor.
             */

            enabled = json["enabled"].GetBool();
			//channel_id = ContainsNotNull(json, "channel_id") ? json["channel_id"].GetString() : "";
			channel_id = GetIDSafely(json, "channel_id");
		}

        bool enabled; /**< Whether the embed is enabled. */
        discpp::Snowflake channel_id; /**< The embed channel id. */
	};

	enum WidgetStyle : int {
		SHIELD,
		BANNER1,
		BANNER2,
		BANNER3,
		BANNER4
	};

    enum class GuildProperty : int {
        NAME,
        REGION,
        VERIFICATION_LEVEL,
        DEFAULT_MESSAGE_NOTIFICATIONS,
        EXPLICIT_CONTENT_FILTER,
        AFK_CHANNEL_ID,
        AFK_TIMEOUT,
        ICON,
        OWNER_ID,
        SPLASH,
        BANNER,
        SYSTEM_CHANNEL_ID,
        RULES_CHANNEL_ID,
        PUBLIC_UPDATES_CHANNEL_ID,
        PREFERRED_LOCALE
    };

    struct GuildModifyRequests {
        std::unordered_map<GuildProperty, std::variant<std::string, int, Image>> guild_requests;

        GuildModifyRequests(const GuildProperty& key, const std::variant<std::string, int, Image>& value ) : guild_requests({ std::make_pair(key, value) }) {};
        GuildModifyRequests(std::unordered_map<GuildProperty, std::variant<std::string, int, Image>> guild_requests) : guild_requests(std::move(guild_requests)) {};

        void Add(const GuildProperty& key, const std::variant<std::string, int, Image>& value) {
            guild_requests.insert({key, value});
        };

        void Remove(const GuildProperty& key) {
            guild_requests.erase(guild_requests.find(key));
        }
    };

	class Guild : public DiscordObject {
	public:
		Guild() = default;

        /**
         * @brief Constructs a discpp::Guild object from an id.
         *
         * This constructor searches the guild cache to get a guild object.
         *
         * If you set `can_request` to true, and the message is not found in cache, then we will request
         * the message from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * @param[in] id The id of the guild.
         * @param[in] can_request Whether or not the library can request the message from the REST API.
         *
         * @return discpp::Guild, this is a constructor.
         */
		Guild(const Snowflake& id, bool can_request = false);

        /**
         * @brief Constructs a discpp::Guild object by parsing json
         *
         * ```cpp
         *      discpp::Guild guild(json);
         * ```
         *
         * @param[in] json The json that makes up the guild.
         *
         * @return discpp::Guild, this is a constructor.
         */
		Guild(rapidjson::Document& json);

        /**
         * @brief Modify the guild.
         *
         * Use discpp::GuildModifyRequests to modify a field of the guild.
         *
         * ```cpp
         *		// Change the name of the guild to "Test"
         *		discpp::ModifyRequests request(discpp::GuildProperty::NAME, "Test");
         *      guild.Modify(request);
         * ```
         *
         * @param[in] modify_request The field to modify, and what to set it to.
         *
         * @return discpp::Channel - This method also sets the guild reference to the returned guild.
         */
		discpp::Guild Modify(GuildModifyRequests modify_requests);

        /**
         * @brief Deletes this guild.
         *
         * ```cpp
         *      guild.DeleteGuild();
         * ```
         *
         * @return void
         */
		void DeleteGuild();

        /**
         * @brief Gets a list of channels in this guild.
         *
         * The first element in the map is the id of the channel, while the second in the channel.
         * This makes it easy to find a channel in the array by using the `std::unordered_map::find()` method.
         *
         * ```cpp
         *      std::unordered_map<discpp::Snowflake, discpp::Channel> guild.GetChannels();
         * ```
         *
         * @return std::unordered_map<discpp::Snowflake, discpp::Channel>
         */
		std::optional<std::unordered_map<discpp::Snowflake, discpp::Channel>> GetChannels();

        /**
         * @brief Gets a list of channel categories in this guild.
         *
         * The first element in the map is the id of the category, while the second is the category channel object.
         * This makes it easy to find a channel in the array by using the `std::unordered_map::find()` method.
         *
         * ```cpp
         *      std::unordered_map<discpp::Snowflake, discpp::Channel> guild.GetCategories();
         * ```
         *
         * @return std::unordered_map<discpp::Snowflake, discpp::Channel>
         */
		std::optional<std::unordered_map<discpp::Snowflake, discpp::Channel>> GetCategories();

        /**
         * @brief Gets a list of channel that don't have parents in this guild.
         *
         * The first element in the map is the id of the channel, while the second is the channel object.
         * This makes it easy to find a channel in the array by using the `std::unordered_map::find()` method.
         *
         * @return std::unordered_map<discpp::Snowflake, discpp::Channel>
         */
        std::optional<std::unordered_map<discpp::Snowflake, discpp::Channel>> GetParentlessChannels();

        /**
         * @brief Gets a channel in this guild.
         *
         * @param[in] id The id of the channel you want to retrieve
         *
         * @return discpp::Channel
         */
        [[nodiscard]] std::optional<discpp::Channel> GetChannel(const Snowflake& id) const;

        /**
         * @brief Creates a channel for this Guild.
         *
         * ```cpp
         *      discpp::Channel channel = guild.CreateChannel("Test", discpp::ChannelType::GUILD_TEXT, "Just a test channel", 0, 0, 0, 0, overwrites, category_channel, false);
         * ```
         *
         * @param[in] name The name of the new channel.
         * @param[in] topic The topic of the new channel.
         * @param[in] type The type of the new channel.
         * @param[in] bitrate The bitrate of the new channel (only for voice channels).
         * @param[in] user_limit The user limit of the new channel (only for voice channels).
         * @param[in] rate_limit_per_user The chat delay for a user of the new channel.
         * @param[in] position The sorting position of the new channel.
         * @param[in] permission_overwrites Array of permission overwrite objects.
         * @param[in] parent_id The parent id of the new channel.
         * @param[in] nsfw Whether the new channel is marked as nsfw.
         *
         * @return discpp::Channel
         */
        discpp::Channel CreateChannel(const std::string& name, const std::string& topic = "", const ChannelType& type = ChannelType::GUILD_TEXT,
                const int& bitrate = 0, const int& user_limit = 0, const int& rate_limit_per_user = 0, const int& position = 0,
                const std::vector<discpp::Permissions>& permission_overwrites = {}, const discpp::Snowflake& parent_id = 0, const bool nsfw = false);

        /**
         * @brief Modifies channel's positions in order to vector elements.
         *
         * ```cpp
         *      guild.ModifyChannelPositions(new_channel_positions);
         * ```
         *
         * @param[in] new_channel_positions The new channel positions in order of the vector elements.
         *
         * @return void
         */
        void ModifyChannelPositions(const std::vector<discpp::Channel>& new_channel_positions);

        /**
         * @brief Gets a discpp::Member from this guild.
         *
         * If you set `can_request` to true, and the member is not found in cache, then we will request
         * the member from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * @param[in] id The member's id
         *
         * @return std::shared_ptr<discpp::Member>
         */
        std::optional<std::shared_ptr<discpp::Member>> GetMember(const Snowflake& id, bool can_request = false);

        /**
         * @brief Ensures the bot has a permission.
         *
         * If the bot does not have the permission a discpp::NoPermissionException will be thrown.
         *
         * ```cpp
         *      Guild::EnsureBotPermission(Permission::MANAGE_CHANNELS);
         * ```
         *
         * @param[in] req_perm The permission to check if the bot has.
         *
         */
        void EnsureBotPermission(const Permission& req_perm);

        /**
         * @brief Adds a discpp::Member to this guild.
         *
         * ```cpp
         *      discpp::Member added_member =  guild.AddMember(119886831578775554, access_token, "New User is here", roles, false, false);
         * ```
         *
         * @param[in] id The id of this member.
         * @param[in] access_token The oauth2 token granted with guilds.join to the bot's application for the member.
         * @param[in] nick The user nick name.
         * @param[in] roles The users role.
         * @param[in] mute Whether the user is muted in voice channels.
         * @param[in] deaf Whether the user is deafened in voice channels.
         *
         * @return std::shared_ptr<discpp::Member>
         */
        std::shared_ptr<discpp::Member> AddMember(const Snowflake& id, const std::string& access_token, const std::string& nick, const std::vector<discpp::Role>& roles, const bool mute, const bool deaf);

        /**
         * @brief Remove a member from the guild.
         *
         * ```cpp
         *      guild.RemoveMember(member);
         * ```
         *
         * @param[in] member The member to remove from the guild.
         *
         * @return void
         */
        void RemoveMember(const discpp::Member& member);

        /**
         * @brief Get all guild bans
         *
         * ```cpp
         *      std::vector<discpp::GuildBan> bans = guild.GetBans();
         * ```
         *
         * @return std::vector<discpp::GuildBan>
         */
		std::vector<discpp::GuildBan> GetBans() const;

        /**
         * @brief Get ban reasons if they are any.
         *
         * ```cpp
         *      std::optional<std::string> reason = guild.GetMemberBanReason(member);
         * ```
         *
         * @param[in] member The member to get a ban remove of.
         *
         * @return std::optional<std::string>
         */
		std::string GetMemberBanReason(const discpp::Member& member) const;

        /**
         * @brief Ban a guild member.
         *
         * ```cpp
         *      guild.BanMember(member, "Reason");
         * ```
         *
         * @param[in] member The member to ban.
         * @param[in] reason The reason to ban them.
         *
         * @return void
         */
		void BanMember(const discpp::Member& member, const std::string& reason = "");

        /**
         * @brief Ban a guild member by id.
         *
         * ```cpp
         *      guild.BanMemberById(150312037426135041, "Reason");
         * ```
         *
         * @param[in] user_id The id to ban.
         * @param[in] reason The reason to ban them.
         *
         * @return void
         */
		void BanMemberById(const Snowflake& user_id, const std::string& reason = "");

        /**
         * @brief Unban a guild member.
         *
         * ```cpp
         *      guild.UnbanMember(member);
         * ```
         *
         * @param[in] member The member to unban.
         *
         * @return void
         */
		void UnbanMember(const discpp::Member& member);

        /**
         * @brief Unban a guild member.
         *
         * ```cpp
         *      guild.UnbanMemberById(150312037426135041);
         * ```
         *
         * @param[in] user_id The id to unban.
         *
         * @return void
         */
		void UnbanMemberById(const Snowflake& user_id);

        /**
         * @brief Kick a guild member.
         *
         * ```cpp
         *      guild.KickMember(member);
         * ```
         *
         * @param[in] member The member to kick.
         *
         * @return void
         */
		void KickMember(const discpp::Member& member, const std::string& reason = "");

        /**
         * @brief Kick a guild member by id.
         *
         * ```cpp
         *      guild.KickMemberById(150312037426135041);
         * ```
         *
         * @param[in] user_id The id to kick.
         *
         * @return void
         */
        void KickMemberById(const Snowflake& member_id, const std::string& reason = "");

        /**
         * @brief Retrieve a guild role.
         *
         * ```cpp
         *      discpp::Role new_role = guild.GetRole(638157816325996565)
         * ```
         *
         * @param[in] id The id of the role you want to retrieve
         *
         * @return std::shared_ptr<discpp::Role>
         */
        [[nodiscard]] std::optional<std::shared_ptr<discpp::Role>> GetRole(const Snowflake& id) const;

        /**
         * @brief Create a guild role.
         *
         * ```cpp
         *      discpp::Role new_role = guild.CreateRole("New Role", permissions, 0xffffff, false, true);
         * ```
         *
         * @param[in] name The new role name.
         * @param[in] permissions The new role permissions.
         * @param[in] color The new role color.
         * @param[in] hoist Whether or not to hoist the role.
         * @param[in] mentionable Whether or not the role is mentionable.
         *
         * @return discpp::Role
         */
        std::shared_ptr<discpp::Role> CreateRole(const std::string& name, const Permissions& permissions = Permissions(), const int& color = 0, const bool hoist = false, const bool mentionable = false);

        /**
         * @brief Modifies role's positions in order to vector elements.
         *
         * ```cpp
         *      guild.ModifyRolePositions(new_role_positions);
         * ```
         *
         * @param[in] new_role_positions The new role positions in order of the vector elements.
         *
         * @return void
         */
        void ModifyRolePositions(const std::vector<discpp::Role>& new_role_positions);

        /**
         * @brief Modify a guild role.
         *
         * ```cpp
         *      discpp::Role modified_role = guild.ModifyRole(role, "New Role", permissions, 0xffffff, false, true);
         * ```
         *
         * @param[in] name The new role name.
         * @param[in] permissions The new role permissions.
         * @param[in] color The new role color.
         * @param[in] hoist Whether or not to hoist the role.
         * @param[in] mentionable Whether or not the role is mentionable.
         *
         * @return discpp::Role
         */
        std::shared_ptr<discpp::Role> ModifyRole(const discpp::Role& role, const std::string& name, const Permissions& permissions = Permissions(), const int& color = 0, const bool hoist = false, const bool mentionable = false);

        /**
         * @brief Deleted a guild role.
         *
         * ```cpp
         *      guild.DeleteRole(role);
         * ```
         *
         * @param[in] role The role to delete.
         *
         * @return void
         */
        void DeleteRole(const discpp::Role& role);

        /**
         * @brief Get guild prune amount.
         *
         * ```cpp
         *      int amount_pruned = guild.GetPruneAmount(10);
         * ```
         *
         * @param[in] days Number of days to count prune for.
         *
         * @return int - Amount of users pruned.
         */
		int GetPruneAmount(const int& days) const;

        /**
         * @brief Begin a guild prune (kicks inactive players).
         *
         * ```cpp
         *      guild.BeginPrune(14);
         * ```
         *
         * @param[in] days Number of days to prune.
         *
         * @return void
         */
		void BeginPrune(const int& days);

        /**
         * @brief Get guild invites.
         *
         * ```cpp
         *      std::vector<discpp::GuildInvite> invites = guild.GetInvites();
         * ```
         *
         * @return std::vector<discpp::GuildInvite>
         */
		[[nodiscard]] std::optional<std::vector<discpp::GuildInvite>> GetInvites() const;

        /**
         * @brief Get guild integrations.
         *
         * ```cpp
         *      std::vector<discpp::Integration> integration = guild.GetIntegrations();
         * ```
         *
         * @return std::vector<discpp::Integration>
         */
		[[nodiscard]] std::optional<std::vector<discpp::Integration>> GetIntegrations() const;

        /**
         * @brief Create a guild integration.
         *
         * ```cpp
         *      guild.CreateIntegration(integration_id, integration_type);
         * ```
         *
         * @param[in] id The integration id.
         * @param[in] type The integration type.
         *
         * @return void
         */
		void CreateIntegration(const Snowflake& id, const std::string& type);

        /**
         * @brief Modify a guild integration.
         *
         * ```cpp
         *      guild.ModifyIntegration(integration, 0, 2, true);
         * ```
         *
         * @param[in] guild_integration The guild integration to modify.
         * @param[in] expire_behavior The behavior when an integration subscription lapses.
         * @param[in] expire_grace_period Period (in days) where the integration will ignore lapsed subscriptions.
         * @param[in] enable_emoticons Whether emoticons should be synced for this integration (twitch only currently).
         *
         * @return void
         */
		void ModifyIntegration(const discpp::Integration& guild_integration, const int& expire_behavior, const int& expire_grace_period, const bool enable_emoticons);

        /**
         * @brief Delete a guild integration.
         *
         * ```cpp
         *      guild.DeleteIntegration(integration);
         * ```
         *
         * @param[in] guild_integration The guild integration to delete.
         *
         * @return void
         */
		void DeleteIntegration(const discpp::Integration& guild_integration);

        /**
         * @brief Sync a guild integration.
         *
         * ```cpp
         *      guild.SyncIntegration(integration);
         * ```
         *
         * @param[in] guild_integration The guild integration to sync.
         *
         * @return void
         */
		void SyncIntegration(const discpp::Integration& guild_integration);

        /**
         * @brief Get a guild embed.
         *
         * ```cpp
         *      discpp::GuildEmbed guild_embed = guild.GetGuildEmbed();
         * ```
         *
         * @return discpp::GuildEmbed
         */
		[[nodiscard]] std::optional<GuildEmbed> GetGuildEmbed() const;

        /**
         * @brief Modify a guild embed.
         *
         * ```cpp
         *      discpp::GuildEmbed modified_guild_embed = guild.ModifyGuildEmbed(381871767846780928, true);
         * ```
         *
         * @return discpp::GuildEmbed
         */
		GuildEmbed ModifyGuildEmbed(const Snowflake& channel_id, const bool enabled);

        /**
         * @brief Returns a partial invite object for guilds with that feature enabled.
         *
         * Requires the MANAGE_GUILD permission. code will be null if a vanity url for the guild is not set. Only `code` and `uses` are valid
         *
         * ```cpp
         *      discpp::GuildInvite vanity_url = ctx.guild->GetVanityURL();
         * ```
         *
         * @return discpp::GuildInvite
         */
		[[nodiscard]] std::optional<discpp::GuildInvite> GetVanityURL() const;

        /**
         * @brief Get a widget image url.
         *
         * ```cpp
         *      std::string widget_image_url = guild.GetWidgetImageURL(style);
         * ```
         *
         * @return std::string
         */
		std::string GetWidgetImageURL(const WidgetStyle& widget_style = WidgetStyle::SHIELD) const;

        /**
         * @brief Returns the requested member.
         *
         * If its already cached return it, else send a REST request and return the requested member.
         *
         * @return std::shared_ptr<discpp::Member>
         */
        std::shared_ptr<discpp::Member> RequestMemberIfNotExist(const Snowflake& member_id);

        /**
         * @brief Get all guild emojis.
         *
         * ```cpp
         *      std::unordered_map<Snowflake, std::shared_ptr<Emoji>> guild_emojis = guild.GetEmojis();
         * ```
         *
         * @return std::unordered_map<Snowflake, std::shared_ptr<Emoji>>
         */
		std::unordered_map<Snowflake, discpp::Emoji> GetEmojis();

        /**
         * @brief Get a guild emoji.
         *
         * ```cpp
         *      discpp::Emoji emoji = guild.GetEmoji(685895680115605543);
         * ```
         *
         * @param[in] id The emoji's id.
         *
         * @return Emoji
         */
        discpp::Emoji GetEmoji(const Snowflake& id) const;

        /**
         * @brief Create a guild emoji.
         *
         * ```cpp
         *      ctx.guild->CreateEmoji("test", { &std::ifstream("C:\\emoji.png", std::ios::in | std::ios::binary), "C:\\emoji.png" }, {});
         * ```
         *
         * @param[in] name The emoji name.
         * @param[in] image The image for the emoji.
         * @param[in] roles The roles for the emoji.
         *
         * @return Emoji
         */
        discpp::Emoji CreateEmoji(const std::string& name, discpp::Image& image, const std::vector<discpp::Role>& roles);

        /**
         * @brief Modify a guild emoji.
         *
         * ```cpp
         *      discpp::Emoji modified_emoji = guild.ModifyEmoji(emoji, "New emoji", roles);
         * ```
         *
         * @param[in] emoji The emoji to modify.
         * @param[in] name The emoji's name.
         * @param[in] role The emoji's roles.
         *
         * @return Emoji
         */
		discpp::Emoji ModifyEmoji(const discpp::Emoji& emoji, const std::string& name, const std::vector<discpp::Role>& roles);

        /**
         * @brief Delete a guild emoji.
         *
         * ```cpp
         *      guild.DeleteEmoji(emoji);
         * ```
         *
         * @return void
         */
		void DeleteEmoji(const discpp::Emoji& emoji);

        /**
         * @brief Retrieve guild icon url.
         *
         * ```cpp
         *      std::string icon_url = guild.GetIconURL()
         * ```
         *
         * @param[in] img_type Optional parameter for type of image
         *
         * @return std::string
         */
		std::string GetIconURL(const discpp::ImageType& img_type = discpp::ImageType::AUTO) const;

        /**
         * @brief Returns if the guild has an icon.
         *
         * @return bool
         */
        bool HasIcon() const;

        /**
         * @brief Retrieve guild banner url.
         *
         * @param[in] img_type Optional parameter for type of image
         *
         * @return std::string
         */
        std::string GetBannerURL(const discpp::ImageType& img_type = discpp::ImageType::AUTO) const;

        /**
         * @brief Returns if the guild has a banner.
         *
         * @return bool
         */
        bool HasBanner() const;

        /**
         * @brief Retrieve guild splash url.
         *
         * @param[in] img_type Optional parameter for type of image
         *
         * @return std::string
         */
        std::string GetSplashURL(const discpp::ImageType& img_type = discpp::ImageType::AUTO) const;

        /**
         * @brief Returns if the guild has a splash.
         *
         * @return bool
         */
        bool HasSplash() const;

        /**
         * @brief Retrieve guild discovery splash url.
         *
         * @param[in] img_type Optional parameter for type of image
         *
         * @return std::string
         */
        std::string GetDiscoverySplashURL(const discpp::ImageType& img_type = discpp::ImageType::AUTO) const;

        /**
         * @brief Returns if the guild has a splash.
         *
         * @return bool
         */
        bool HasDiscoverySplash() const;

        /**
         * @brief Retrieve guild owner as a discpp::Member object
         *
         * ```cpp
         *      std::shared_ptr<discpp::Member> owner = guild.GetOwnerObject()
         * ```
         *
         * @return std::shared_ptr<discpp::Member>
         */
        inline std::shared_ptr<discpp::Member> GetOwnerMember();

        /**
         * @brief Returns an audit log object.
         *
         * ```cpp
         *      discpp::GetAuditLog audit_log = ctx.guild->GetAuditLog();
         * ```
         *
         * @return discpp::GetAuditLog
         */
		discpp::AuditLog GetAuditLog() const;

        /**
         * @brief Returns if the client is the guild's owner.
         *
         * @return bool
         */
		bool IsBotOwner() const;

        /**
         * @brief Returns if the guild has embeds enabled.
         *
         * @return bool
         */
		bool IsEmbedEnabled() const;

        /**
         * @brief Returns if the guild has widgets enabled.
         *
         * @return bool
         */
		bool IsWidgetEnabled() const;

        /**
         * @brief Returns if the guild is large.
         *
         * @return bool
         */
		bool IsLarge() const;

        /**
         * @brief Returns if the guild has embeds enabled.
         *
         * @return bool
         */
		bool IsUnavailable() const;

        /**
         * @brief Gets the formatted joined at time and date for the bot.
         *
         * Formatted output: `%Y-%m-%d @ %H:%M:%S GMT`
         *
         * @return std::string
         */
        std::string GetFormattedJoinedAt() const;

        /**
         * @brief Gets the joined at time and date for the bot.
         *
         * @return std::chrono::system_clock::time_point
         */
        std::chrono::system_clock::time_point GetJoinedAt() const;

        /**
         * @brief Gets the formatted created at time and date for this guild.
         *
         * Formatted output: `%Y-%m-%d @ %H:%M:%S GMT`
         *
         * @return std::string
         */
        std::string GetFormattedCreatedAt() const;

        /**
         * @brief Gets the created at time and date for this guild.
         *
         * @return std::chrono::system_clock::time_point
         */
        std::chrono::system_clock::time_point GetCreatedAt() const;

		std::string name; /**< Guild name. */
		Snowflake owner_id; /**< ID of the guild owner. */
		int permissions; /**< Total permissions for the bot in the guild (does not include channel overrides). */
		std::string region; /**< Voice region id for the guild. */
		Snowflake afk_channel_id; /**< ID of afk channel. */
		int afk_timeout; /**< AFK timeout in seconds. */
		discpp::specials::VerificationLevel verification_level; /**< Verification level required for the guild. */
		discpp::specials::DefaultMessageNotificationLevel default_message_notifications; /**< Default message notifications level. */
		discpp::specials::ExplicitContentFilterLevel explicit_content_filter; /**< Explicit content filter level. */
		std::unordered_map<Snowflake, std::shared_ptr<Role>> roles; /**< Roles in the guild. */
		std::unordered_map<Snowflake, Emoji> emojis; /**< Custom guild emojis. */
		std::vector<std::string> features; /**< Enabled guild features. */
		discpp::specials::MFALevel mfa_level; /**< Required MFA level for the guild. */
		Snowflake application_id; /**< Application id of the guild creator if it is bot-created. */
		bool widget_enabled; /**< Whether or not the server widget is enabled. */
		Snowflake widget_channel_id; /**< The channel id for the server widget. */
		Snowflake system_channel_id; /**< The id of the channel where guild notices such as welcome messages and boost events are posted. */
        int system_channel_flags; /**< System channel flags. */
        Snowflake rules_channel_id; /**< The id of the channel where "PUBLIC" guilds display rules and/or guidelines. */
        std::chrono::system_clock::time_point joined_at; /**< When this guild was joined at. */
		int member_count; /**< Total number of members in this guild. */
		std::vector<discpp::VoiceState> voice_states; /**< Array of partial voice state objects. */
		std::unordered_map<Snowflake, std::shared_ptr<Member>> members; /**< Users in the guild. */
		std::unordered_map<Snowflake, discpp::Channel> channels; /**< Channels in the guild. */
		int max_presences; /**< The maximum amount of presences for the guild (the default value, currently 25000, is in effect when null is returned). */
		int max_members; /**< The maximum amount of members for the guild. */
		std::string vanity_url_code; /**< The vanity url code for the guild. */
		std::string description; /**< The description for the guild. */
		discpp::specials::NitroTier premium_tier; /**< Premium tier (Server Boost level). */
		int premium_subscription_count; /**< The number of boosts this server currently has. */
		std::string preferred_locale; /**< The preferred locale of a "PUBLIC" guild used in server discovery and notices from Discord; defaults to "en-US". */
        discpp::Channel public_updates_channel; /**< The channel where admins and moderators of "PUBLIC" guilds receive notices from Discord. */
		int approximate_member_count; /**< Approximate number of members in this guild, returned from the GET /guild/<id> endpoint when with_counts is true. */
		int approximate_presence_count; /**< Approximate number of online members in this guild, returned from the GET /guild/<id> endpoint when with_counts is true. */
	private:
        unsigned char flags = 0b0;
        uint64_t icon_hex[2] = {0, 0};
        uint64_t splash_hex[2] = {0, 0};
        uint64_t discovery_hex[2] = {0, 0};
        uint64_t banner_hex[2] = {0, 0};

        bool is_icon_gif = false;
    };

    class VoiceState {
    public:
        VoiceState() = default;

        /**
         * @brief Constructs a discpp::VoiceState object from json.
         *
         * ```cpp
         *      discpp::VoiceState voice_state(json);
         * ```
         *
         * @param[in] json The json data for the voice state.
         *
         * @return discpp::VoiceState, this is a constructor.
         */
        VoiceState(rapidjson::Document& json);

        Snowflake guild_id; /**< The guild id this voice state is for. */
        Snowflake channel_id; /**< The channel id this user is connected to. */
        Snowflake user_id; /**< The user id this voice state is for. */
        std::shared_ptr<discpp::Member> member; /**< The guild member this voice state is for. */
        std::string session_id; /**< The session id for this voice state. */
        bool deaf; /**< Whether this user is deafened by the server. */
        bool mute; /**< Whether this user is muted by the server. */
        bool self_deaf; /**< Whether this user is locally deafened. */
        bool self_mute; /**< Whether this user is locally muted. */
        bool self_stream; /**< Whether this user is streaming using "Go Live". */
        bool suppress; /**< Whether this user is muted by the current user. */
    };
}

#endif