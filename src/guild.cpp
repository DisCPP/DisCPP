#include "guild.h"
#include "member.h"
#include "channel.h"
#include "utils.h"
#include "emoji.h"
#include "bot.h"
#include <string>
#include <iostream>

namespace discord {
	Guild::Guild(snowflake id) : DiscordObject(id) {
		/**
		 * @brief Constructs a discord::Guild object from an id.
		 *
		 * This constructor searches the guild cache to get a guild object.
		 *
		 * ```cpp
		 *      discord::Guild guild(583251190591258624);
		 * ```
		 *
		 * @param[in] id The id of the guild
		 *
		 * @return discord::Guild, this is a constructor.
		 */

		auto guild = std::find_if(discord::globals::bot_instance->guilds.begin(), discord::globals::bot_instance->guilds.end(), [id](discord::Guild a) { return id == a.id; });

		if (guild != discord::globals::bot_instance->guilds.end()) {
			*this = *guild;
		}
	}

	Guild::Guild(nlohmann::json json) {
		/**
		 * @brief Constructs a discord::Guild object by parsing json
		 *
		 * ```cpp
		 *      discord::Guild guild(json);
		 * ```
		 *
		 * @param[in] json The json that makes up the guild.
		 *
		 * @return discord::Guild, this is a constructor.
		 */

		id = GetDataSafely<snowflake>(json, "id");
		icon = GetDataSafely<std::string>(json, "icon");
		name = GetDataSafely<std::string>(json, "name");
		splash = GetDataSafely<std::string>(json, "splash");
		owner = GetDataSafely<bool>(json, owner);
		owner_id = GetDataSafely<snowflake>(json, "owner_id");
		permissions = GetDataSafely<int>(json, "permissions");
		region = GetDataSafely<std::string>(json, "region");
		afk_channel_id = GetDataSafely<snowflake>(json, "afk_channel_id");
		afk_timeout = GetDataSafely<int>(json, "afk_timeout");
		embed_enabled = GetDataSafely<bool>(json, "embed_enabled");
		embed_channel_id = GetDataSafely<snowflake>(json, "embed_channel_id");
		verification_level = (json.contains("verification_level")) ? static_cast<discord::specials::VerificationLevel>(json["verification_level"].get<int>()) : discord::specials::VerificationLevel::NO_VERIFICATION;
		default_message_notifications = (json.contains("default_message_notifications")) ? static_cast<discord::specials::DefaultMessageNotificationLevel>(json["default_message_notifications"].get<int>()) : discord::specials::DefaultMessageNotificationLevel::ALL_MESSAGES;
		explicit_content_filter = (json.contains("explicit_content_filter")) ? static_cast<discord::specials::ExplicitContentFilterLevel>(json["explicit_content_filter"].get<int>()) : discord::specials::ExplicitContentFilterLevel::DISABLED;
		if (json.contains("roles")) {
			for (auto& role : json["roles"]) {
				discord::Role tmp = discord::Role(role);
				roles.insert(std::make_pair<snowflake, Role>(static_cast<discord::snowflake>(tmp.id), static_cast<discord::Role>(tmp)));
			}
		}
		if (json.contains("emojis")) {
			for (auto& emoji : json["emojis"]) {
				discord::Emoji tmp = discord::Emoji(emoji);
				emojis.insert(std::make_pair<snowflake, Emoji>(static_cast<discord::snowflake>(tmp.id), static_cast<discord::Emoji>(tmp)));
			}
		}
		// features
		mfa_level = (json.contains("mfa_level")) ? static_cast<discord::specials::MFALevel>(json["mfa_level"].get<int>()) : discord::specials::MFALevel::NO_MFA;
		application_id = GetDataSafely<snowflake>(json, "application_id");
		widget_enabled = GetDataSafely<bool>(json, "widget_enabled");
		widget_channel_id = GetDataSafely<snowflake>(json, "widget_channel_id");
		system_channel_id = GetDataSafely<snowflake>(json, "system_channel_id");
		joined_at = GetDataSafely<std::string>(json, "joined_at");
		large = GetDataSafely<bool>(json, "large");
		unavailable = GetDataSafely<bool>(json, "unavailable");
		member_count = GetDataSafely<int>(json, "member_count");
		// voice_states
		if (json.contains("channels")) {
			for (auto& channel : json["channels"]) {
				channels.push_back(discord::Channel(channel));
			}
		}
		if (json.contains("presences") && json.contains("members")) {
			for (auto const& presence : json["presences"]) {
				auto member = std::find_if(members.begin(), members.end(), [presence](discord::Member a) { return presence["user"]["id"] == a.user.id;});

				if (member != members.end()) {
					nlohmann::json activity = presence["game"];
					
					if (!activity.is_null()) {
						discord::Activity act;

						act.text = activity["name"];
						act.type = static_cast<presence::ActivityType>(activity["type"].get<int>());
						act.status = presence["status"];
						if (activity.contains("url")) {
							act.url = activity["url"];
						}
						act.application_id = activity["id"];
						act.created_at = std::to_string(activity["created_at"].get<int>());

						member->activity = act;
					}
				}
			}
		}
		max_presences = GetDataSafely<int>(json, "max_presences");
		max_members = GetDataSafely<int>(json, "max_members");
		vanity_url_code = GetDataSafely<std::string>(json, "vanity_url_code");
		description = GetDataSafely<std::string>(json, "description");
		banner = GetDataSafely<std::string>(json, "banner");
		premium_tier = (json.contains("premium_tier")) ? static_cast<discord::specials::NitroTier>(json["premium_tier"].get<int>()) : discord::specials::NitroTier::NO_TIER;
		premium_subscription_count = GetDataSafely<int>(json, "premium_subscription_count");
		preferred_locale = GetDataSafely<std::string>(json, "preferred_locale");
		created_at = FormatTimeFromSnowflake(id);
		if (json.contains("members")) {
			for (auto& member : json["members"]) {
				members.push_back(discord::Member(member, *this));
			}
		}
	}

	discord::Guild Guild::ModifyGuildName(std::string name) {
		/**
		 * @brief Modify guild name.
		 *
		 * ```cpp
		 *      guild.ModifyGuildName("Test");
		 * ```
		 *
		 * @param[in] name The new guild name.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"name\": \"" + EscapeString(name) + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildRegion(discord::snowflake region_id) {
		/**
		 * @brief Modify guild region.
		 *
		 * ```cpp
		 *      guild.ModifyGuildRegion(new_region_id);
		 * ```
		 *
		 * @param[in] region_id The new guild region.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"region\": \"" + region_id + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildVerificationLevel(discord::specials::VerificationLevel verification_level) {
		/**
		 * @brief Modify guild verification level.
		 *
		 * ```cpp
		 *      guild.ModifyGuildVerificationLevel(discord::specials::VerificationLevel::LOW);
		 * ```
		 *
		 * @param[in] verification_level The new guild verification level.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"verification_level\": \"" + std::to_string(verification_level) + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildDefaultMessageNotifications(discord::specials::DefaultMessageNotificationLevel notification_level) {
		/**
		 * @brief Modify guild message notifications.
		 *
		 * ```cpp
		 *      guild.ModifyGuildDefaultMessageNotifications(discord::specials::DefaultMessageNotificationLevel::ALL_MESSAGES);
		 * ```
		 *
		 * @param[in] notification_level The new guild notification level.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"default_message_notifications\": \"" + std::to_string(notification_level) + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildExplicitContentFilter(discord::specials::ExplicitContentFilterLevel explicit_content_filter) {
		/**
		 * @brief Modify guild explicit content filter.
		 *
		 * ```cpp
		 *      guild.ModifyGuildExplicitContentFilter(discord::specials::VerificationLevel::DISABLED);
		 * ```
		 *
		 * @param[in] explicit_content_filter The new guild content filter.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"verification_level\": \"" + std::to_string(verification_level) + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildAFKChannelID(discord::snowflake afk_channel_id) {
		/**
		 * @brief Modify guild afk channel id.
		 *
		 * ```cpp
		 *      guild.ModifyGuildAFKChannelID(663949599937134603);
		 * ```
		 *
		 * @param[in] afk_channel_id The new guild afk channel id.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"afk_channel_id\": \"" + afk_channel_id + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildAFKTimeout(int timeout) {
		/**
		 * @brief Modify guild afk timeout.
		 *
		 * ```cpp
		 *      guild.ModifyGuildAFKTimeout(1000);
		 * ```
		 *
		 * @param[in] timeout The new guild afk timeout (in seconds).
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"afk_timeout\": \"" + std::to_string(timeout) + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildOwnerID(discord::snowflake owner_id) {
		/**
		 * @brief Modify guild owner id.
		 *
		 * ```cpp
		 *      guild.ModifyGuildOwnerID(661013339748696075);
		 * ```
		 *
		 * @param[in] owner_id The new guild owner id to transfer the guild ownership.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"owner_id\": \"" + owner_id + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildSystemChannelID(discord::snowflake system_channel_id) {
		/**
		 * @brief Modify guild system channel id.
		 *
		 * ```cpp
		 *      guild.ModifyGuildSystemChannelID(661013339748696075);
		 * ```
		 *
		 * @param[in] system_channel_id The new guild system channel id.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"system_channel_id\": \"" + system_channel_id + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildRulesChannelID(discord::snowflake rules_channel_id) {
		/**
		 * @brief Modify guild rules channel id.
		 *
		 * ```cpp
		 *      guild.ModifyGuildRulesChannelID(661013339748696075);
		 * ```
		 *
		 * @param[in] rules_channel_id The new guild rules channel id.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"rules_channel_id\": \"" + rules_channel_id + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildPublicUpdatesChannelID(discord::snowflake public_updates_channel_id) {
		/**
		 * @brief Modify guild public update channel id.
		 *
		 * ```cpp
		 *      guild.ModifyGuildPublicUpdatesChannelID(661013339748696075);
		 * ```
		 *
		 * @param[in] public_updates_channel_id The new guild public updates channel id.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"public_updates_channel_id\": \"" + public_updates_channel_id + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	discord::Guild Guild::ModifyGuildPreferredLocale(std::string preferred_locale) {
		/**
		 * @brief Modify guild preferred local.
		 *
		 * ```cpp
		 *      guild.ModifyGuildPreferredLocale("en-US");
		 * ```
		 *
		 * @param[in] preferred_locale The new guild preferred locale.
		 *
		 * @return discord::Guild
		 */

		cpr::Body body("{\"preferred_locale\": \"" + preferred_locale + "\"}");

		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);

		return discord::Guild(result);
	}

	void Guild::DeleteGuild() {
		/**
		 * @brief Deletes this guild.
		 *
		 * ```cpp
		 *      guild.DeleteGuild();
		 * ```
		 *
		 * @return void
		 */

		SendDeleteRequest(Endpoint("/guilds/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::vector<discord::Channel> Guild::GetChannels() {
		/**
		 * @brief Gets a list of channels in this guild.
		 *
		 * ```cpp
		 *      std::vector<discord::Channel> guild.GetChannels();
		 * ```
		 *
		 * @return std::vector<discord::Channel>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		std::vector<discord::Channel> channels;
		for (auto& channel : result) {
			channels.push_back(discord::Channel(channel));
		}
		return channels;
	}

	discord::Channel Guild::CreateChannel(std::string name, std::string topic, GuildChannelType type, int bitrate, int user_limit, int rate_limit_per_user, int position, std::vector<discord::Permissions> permission_overwrites, discord::Channel category, bool nsfw) {
		/**
		 * @brief Creates a channel for this Guild.
		 *
		 * ```cpp
		 *      discord::Channel channel = guild.CreateChannel("Test", discord::GuildChannelType::GUILD_TEXT, "Just a test channel", 0, 0, 0, 0, overwrites, category_channel, false);
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
		 * @param[in] nsfw Wheather the new channel is marked as nsfw.
		 *
		 * @return discord::Channel
		 */

		if (bitrate < 8000) bitrate = 8000;

		nlohmann::json permission_json = nlohmann::json::array();
		for (auto perm : permission_overwrites) {
			permission_json.push_back(perm.ToJson());
		}

		nlohmann::json json_raw = nlohmann::json({
			{"name", name},
			{"type", type},
			{"rate_limit_per_user", rate_limit_per_user},
			{"position", position},
			{"nsfw", nsfw}
		});

		if (!topic.empty()) json_raw.push_back({ "topic", EscapeString(topic) });
		if (type == GuildChannelType::GUILD_VOICE) {
			json_raw.push_back({ "bitrate", bitrate });
			json_raw.push_back({ "user_limit", user_limit });
		}

		if (permission_json.size() > 0) {
			json_raw.push_back({ "permission_overwrites", permission_json });
		}

		if (!category.id.empty()) {
			json_raw.push_back({ "parent_id", category.id });
		}


		cpr::Body body(json_raw.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::CHANNEL, body);
		discord::Channel channel(result);
		channels.push_back(channel);

		return channel;
	}

	void Guild::ModifyChannelPositions(std::vector<discord::Channel> new_channel_positions) {
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

		nlohmann::json json_raw = nlohmann::json();

		for (int i = 0; i < new_channel_positions.size(); i++) {
			json_raw.push_back({ {"id", new_channel_positions[i].id}, {"position", i} });
		}

		cpr::Body body(json_raw.dump());
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id + "/channels"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::CHANNEL, body);
	}

	discord::Member Guild::GetMember(snowflake id) {
		/**
		 * @brief Gets a discord::Member from this guild.
		 *
		 * ```cpp
		 *      discord::Member member = guild.GetMember(228846961774559232);
		 * ```
		 *
		 * @param[in] id The member's id
		 *
		 * @return discord::Member
		 */

		auto member = std::find_if(discord::globals::bot_instance->members.begin(), discord::globals::bot_instance->members.end(), [id](discord::Member a) { return id == a.user.id; });

		if (member != discord::globals::bot_instance->members.end()) {
			return *member;
		}
		throw std::runtime_error("Member not found!");
	}

	discord::Member Guild::AddMember(snowflake id, std::string access_token, std::string nick, std::vector<discord::Role> roles, bool mute, bool deaf) {
		/**
		 * @brief Adds a discord::Member to this guild.
		 *
		 * ```cpp
		 *      discord::Member added_member =  guild.AddMember(119886831578775554, access_token, "New User is here", roles, false, false);
		 * ```
		 *
		 * @param[in] id The id of this member.
		 * @param[in] access_token The oauth2 token granted witht he guilds.join to the bot's application for the member.
		 * @param[in] nick The user nick name.
		 * @param[in] roles The users role.
		 * @param[in] mute Whether the user is muted in voice channels.
		 * @param[in] deaf Whether the user is deafened in voice channels.
		 *
		 * @return discord::Member
		 */

		std::string json_roles = "[";
		for (discord::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + role.id + "\"";
			} else {
				json_roles += ", \"" + role.id + "\"";
			}
		}
		json_roles += "]";

		cpr::Body body("{\"access_token\": \"" + access_token + "\", \"nick\": \"" + nick + "\", \"roles\": " + json_roles + ", \"mute\": " + std::to_string(mute) + ", \"deaf\": " + std::to_string(deaf) + "}");
		nlohmann::json result = SendPutRequest(Endpoint("/guilds/" + this->id + "/members/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		return (result == "{}") ? discord::Member(id) : discord::Member(result, id); // If the member is already added, return it.
	}

	void Guild::RemoveMember(discord::Member member) {
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

		SendDeleteRequest(Endpoint("/guilds/" + id + "/members/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	std::vector<discord::GuildBan> Guild::GetBans() {
		/**
		 * @brief Get all guild bans
		 *
		 * ```cpp
		 *      std::vector<discord::GuildBan> bans = guild.GetBans();
		 * ```
		 *
		 * @return std::vector<discord::GuildBan>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/bans"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		
		std::vector<discord::GuildBan> guild_bans;
		for (auto& guild_ban : result) {
			std::string reason = (!guild_ban["reason"].is_null()) ? guild_ban["reason"] : "";
			guild_bans.push_back(discord::GuildBan(reason, discord::User(guild_ban["user"])));
		}

		return guild_bans;
	}

	std::optional<std::string> Guild::GetMemberBanReason(discord::Member member) {
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

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/bans/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		if (result.contains("reason")) return result["reason"];

		return std::nullopt;
	}

	void Guild::BanMember(discord::Member member, std::string reason) {
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

		cpr::Body body("{\"reason\": \"" + EscapeString(reason) + "\"}");
		nlohmann::json json = SendPutRequest(Endpoint("/guilds/" + id + "/bans/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::UnbanMember(discord::Member member) {
		/**
		 * @brief Unban a guild member.
		 *
		 * ```cpp
		 *      std::optional<std::string> reason = guild.GetMemberBanReason(member);
		 * ```
		 *
		 * @param[in] member The member to unban.
		 *
		 * @return void
		 */

		SendDeleteRequest(Endpoint("/guilds/" + id + "/bans/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	void Guild::KickMember(discord::Member member) {
		/**
		 * @brief Kick a guild member.
		 *
		 * ```cpp
		 *      guild.KickMember(member);
		 * ```
		 *
		 * @param[in] member The member to ban.
		 *
		 * @return void
		 */

		SendDeleteRequest(Endpoint("guilds/" + id + "/members/" + member.user.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	discord::Role Guild::CreateRole(std::string name, Permissions permissions, int color, bool hoist, bool mentionable) {
		/**
		 * @brief Create a guild role.
		 *
		 * ```cpp
		 *      discord::Role new_role = guild.CreateRole("New Role", permissions, 0xffffff, false, true);
		 * ```
		 *
		 * @param[in] name The new role name.
		 * @param[in] permissions The new role permissions.
		 * @param[in] color The new role color.
		 * @param[in] hoist Whether or not to hoist the role.
		 * @param[in] mentionable Whether or not the role is mentionable.
		 *
		 * @return discord::Role
		 */

		nlohmann::json json_body = nlohmann::json({
			{"name", EscapeString(name)},
			{"permissions", permissions.allow_perms.value},
			{"color", color},
			{"hoist", hoist},
			{"mentionable", mentionable}
		});

		cpr::Body body(json_body.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/" + id + "/roles"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discord::Role new_role(result);
		roles.insert(std::pair<snowflake, Role>(new_role.id, new_role));

		return new_role;
	}

	void Guild::ModifyRolePositions(std::vector<discord::Role> new_role_positions) {
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

		nlohmann::json json_raw = nlohmann::json();

		for (int i = 0; i < new_role_positions.size(); i++) {
			json_raw.push_back({ {"id", new_role_positions[i].id}, {"position", i} });
		}

		cpr::Body body(json_raw.dump());
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id + "/roles"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::CHANNEL, body);
	}

	discord::Role Guild::ModifyRole(discord::Role role, std::string name, Permissions permissions, int color, bool hoist, bool mentionable) {
		/**
		 * @brief Create a guild role.
		 *
		 * ```cpp
		 *      discord::Role modified_role = guild.ModifyRole(role, "New Role", permissions, 0xffffff, false, true);
		 * ```
		 *
		 * @param[in] name The new role name.
		 * @param[in] permissions The new role permissions.
		 * @param[in] color The new role color.
		 * @param[in] hoist Whether or not to hoist the role.
		 * @param[in] mentionable Whether or not the role is mentionable.
		 *
		 * @return discord::Role
		 */

		nlohmann::json json_body = nlohmann::json({
			{"name", name},
			{"permissions", permissions.allow_perms.value},
			{"color", color},
			{"hoist", hoist},
			{"mentionable", mentionable}
		});

		cpr::Body body(json_body.dump());
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id + "/roles/" + role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discord::Role modified_role(result);
		std::unordered_map<snowflake, Role>::iterator it = roles.find(role.id);
		if (it != roles.end()) {
			it->second = modified_role;
		}

		return modified_role;
	}

	void Guild::DeleteRole(discord::Role role) {
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

		nlohmann::json result = SendDeleteRequest(Endpoint("/guilds/" + id + "/roles/" + role.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		roles.erase(role.id);
	}

	int Guild::GetPruneAmount(int days) {
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

		if (days < 1) {
			throw std::runtime_error("Cannot get prune amount with less than 1 day.");
		}

		cpr::Body body("{\"days\": " + std::to_string(days) + "}");

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);
		
		return result["pruned"].get<int>();
	}

	void Guild::BeginPrune(int days) {
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

		cpr::Body body("{\"days\": " + std::to_string(days) + "}");
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/" + id + "/prune"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, discord::RateLimitBucketType::GUILD, body);
	}

	std::vector<discord::GuildInvite> Guild::GetInvites() {
		/**
		 * @brief Get guild invites.
		 *
		 * ```cpp
		 *      std::vector<discord::GuildInvite> invites = guild.GetInvites();
		 * ```
		 *
		 * @return std::vector<discord::GuildInvite>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/invites"), DefaultHeaders(), {}, {});

		std::vector<discord::GuildInvite> guild_invites;
		for (auto& guild_invite : result) {
			guild_invites.push_back(discord::GuildInvite(guild_invite));
		}

		return guild_invites;
	}

	std::vector<discord::GuildIntegration> Guild::GetIntegrations() {
		/**
		 * @brief Get guild integrations.
		 *
		 * ```cpp
		 *      std::vector<discord::GuildIntegration> itegration = guild.GetIntegrations();
		 * ```
		 *
		 * @return std::vector<discord::GuildIntegration>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/integrations"), DefaultHeaders(), {}, {});

		std::vector<discord::GuildIntegration> guild_integrations;
		for (auto& guild_integration : result) {
			guild_integrations.push_back(discord::GuildIntegration(guild_integration));
		}

		return guild_integrations;
	}

	void Guild::CreateIntegration(snowflake id, std::string type) {
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

		cpr::Body body("{\"type\": \"" + type + "\", \"id\": \"" + id + "\"}");
		SendPostRequest(Endpoint("/guilds/" + this->id + "/integrations"), DefaultHeaders(), this->id, RateLimitBucketType::GUILD, body);
	}

	void Guild::ModifyIntegration(discord::GuildIntegration guild_integration, int expire_behavior, int expire_grace_period, bool enable_emoticons) {
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

		cpr::Body body("{\"expire_behavior\": " + std::to_string(expire_behavior) + ", \"expire_grace_period\": " + std::to_string(expire_grace_period) + ", \"enable_emoticons\": " + std::to_string(enable_emoticons) + "}");
		SendPostRequest(Endpoint("/guilds/" + id + "/integrations/" + guild_integration.id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
	}

	void Guild::DeleteIntegration(discord::GuildIntegration guild_integration) {
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

		SendDeleteRequest(Endpoint("/guilds/" + id + "/integrations/" + guild_integration.id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	void Guild::SyncIntegration(discord::GuildIntegration guild_integration) {
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

		SendPostRequest(Endpoint("/guilds/" + id + "/integrations/" + guild_integration.id + "/sync"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
	}

	discord::GuildEmbed Guild::GetGuildEmbed() {
		/**
		 * @brief Get a guild embed.
		 *
		 * ```cpp
		 *      discord::GuildEmbed guild_embed = guild.GetGuildEmbed();
		 * ```
		 *
		 * @return discord::GuildEmbed
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		return discord::GuildEmbed(result);
	}

	discord::GuildEmbed Guild::ModifyGuildEmbed(snowflake channel_id, bool enabled) {
		/**
		 * @brief Modify a guild embed.
		 *
		 * ```cpp
		 *      discord::GuildEmbed modified_guild_embed = guild.ModifyGuildEmbed(381871767846780928, true);
		 * ```
		 *
		 * @return discord::GuildEmbed
		 */

		cpr::Body body("{\"channel_id\": \"" + channel_id + "\", \"enabled\": " + ((enabled) ? "true" : "false") + "}");
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + id + "/embed"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return discord::GuildEmbed();
	}

	std::string Guild::GetWidgetImageURL(WidgetStyle widget_style) {
		/**
		 * @brief Get a widget image url.
		 *
		 * ```cpp
		 *      std::string widget_image_url = guild.GetWidgetImageURL(style);
		 * ```
		 *
		 * @return std::string
		 */

		std::string style;
		switch (widget_style) {
		case WidgetStyle::SHIELD:
			style = "shield";
			break;
		case WidgetStyle::BANNER1:
			style = "banner1";
			break;
		case WidgetStyle::BANNER2:
			style = "banner2";
			break;
		case WidgetStyle::BANNER3:
			style = "banner3";
			break;
		case WidgetStyle::BANNER4:
			style = "banner4";
			break;
		}
		cpr::Body body("{\"style\": " + style + "}");
		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/widget.png"), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);

		return std::string();
	}

	std::unordered_map<snowflake, Emoji> Guild::GetEmojis() {
		/**
		 * @brief Get all guild emojis.
		 *
		 * ```cpp
		 *      std::vector<discord::Emoji> guild_emojis = guild.GetEmojis();
		 * ```
		 *
		 * @return std::vector<discord::Emoji>
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + id + "/emojis"), DefaultHeaders(), {}, {});

		std::unordered_map<snowflake, Emoji> emojis;
		for (auto& emoji : result) {
			discord::Emoji tmp = discord::Emoji(emoji);
			emojis.insert(std::pair<snowflake, Emoji>(static_cast<snowflake>(tmp.id), static_cast<Emoji>(tmp)));
		}
		this->emojis = emojis;

		return emojis;
	}

	discord::Emoji Guild::GetEmoji(snowflake id) {
		/**
		 * @brief Get a guild emoji.
		 *
		 * ```cpp
		 *      discord::Emoji emoji = guild.GetEmoji(685895680115605543);
		 * ```
		 *
		 * @param[in] id The emoji's id.
		 *
		 * @return discord::Emoji
		 */

		nlohmann::json result = SendGetRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), {}, {});

		return discord::Emoji(result);
	}

	discord::Emoji Guild::CreateEmoji(std::string name, discord::Image image, std::vector<discord::Role> roles) {
		/**
		 * @brief Create a guild emoji.
		 *
		 * ```cpp
		 *      ctx.guild.CreateEmoji("test", { &std::ifstream("C:\\emoji.png", std::ios::in | std::ios::binary), "C:\\emoji.png" }, {});
		 * ```
		 *
		 * @param[in] name The emoji name.
		 * @param[in] image The image for the emoji.
		 * @param[in] roles The roles for the emoji.
		 *
		 * @return discord::Emoji
		 */

		nlohmann::json role_json;
		for (discord::Role role : roles) {
			role_json.push_back(role.id);
		}

		nlohmann::json body_raw = nlohmann::json({
			{"name", EscapeString(name)},
			{"image", image.ToDataURI()},
			{"roles", role_json}
		});

		cpr::Body body(body_raw.dump());
		nlohmann::json result = SendPostRequest(Endpoint("/guilds/" + id + "/emojis"), DefaultHeaders({ { "Content-Type", "application/json" } }), id, RateLimitBucketType::GUILD, body);

		return discord::Emoji(result);
	}

	discord::Emoji Guild::ModifyEmoji(discord::Emoji emoji, std::string name, std::vector<discord::Role> roles) {
		/**
		 * @brief Modify a guild emoji.
		 *
		 * ```cpp
		 *      discord::Emoji modified_emoji = guild.ModifyEmoji(emoji, "New emoji", roles);
		 * ```
		 *
		 * @param[in] emoji The emoji to modfy.
		 * @param[in] name The emoji's name.
		 * @param[in] role The emoji's roles.
		 *
		 * @return discord::Emoji
		 */

		std::string json_roles = "[";
		for (discord::Role role : roles) {
			if (&role == &roles.front()) {
				json_roles += "\"" + role.id + "\"";
			}
			else {
				json_roles += ", \"" + role.id + "\"";
			}
		}
		json_roles += "]";

		cpr::Body body("{\"name\": \"" + name + "\", \"roles\": " + json_roles + "}");
		nlohmann::json result = SendPatchRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD, body);
		discord::Emoji em(result);
		std::unordered_map<snowflake, Emoji>::iterator it = emojis.find(em.id);
		if (it != emojis.end()) {
			it->second = em;
		}
		return em;
	}

	void Guild::DeleteEmoji(discord::Emoji emoji) {
		/**
		 * @brief Delete a guild emoji.
		 *
		 * ```cpp
		 *      guild.DeleteEmoji(emoji);
		 * ```
		 *
		 * @return void
		 */

		nlohmann::json result = SendDeleteRequest(Endpoint("/guilds/" + this->id + "/emojis/" + id), DefaultHeaders(), id, RateLimitBucketType::GUILD);
		emojis.erase(emoji.id);
	}

	
	std::string Guild::GetIconURL(discord::ImageType imgType) {
		std::string idString = this->id.c_str();
		std::string url = "https://cdn.discordapp.com/icons/" + idString +  "/" + this->icon;
		if (imgType == ImageType::AUTO) imgType = StartsWith(this->icon, "a_") ? ImageType::GIF : ImageType::PNG;
		switch (imgType) {
		case ImageType::GIF:
			return cpr::Url(url + ".gif");
		case ImageType::JPEG:
			return cpr::Url(url + ".jpeg");
		case ImageType::PNG:
			return cpr::Url(url + ".png");
		case ImageType::WEBP:
			return cpr::Url(url + ".webp");
		default:
			return cpr::Url(url);
		}
	}
}