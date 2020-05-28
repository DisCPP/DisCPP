#ifndef DISCPP_CHANNEL_H
#define DISCPP_CHANNEL_H

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include "discord_object.h"
#include "permission.h"
#include "embed_builder.h"

#include <variant>
#include <vector>

#include <rapidjson/document.h>

namespace discpp {
	class Message;
	class GuildInvite;
	class User;
	class Guild;

	enum class ChannelProperty : int {
		NAME,
		POSITION,
		TOPIC,
		NSFW,
		RATE_LIMIT,
		BITRATE,
		USER_LIMIT,
		PERMISSION_OVERWRITES,
		PARENT_ID
	};

	struct ModifyRequests {
	    // key, value
		std::unordered_map<ChannelProperty, std::variant<std::string, int, bool>> requests;

		ModifyRequests(ChannelProperty key, std::variant<std::string, int, bool> value) : requests({ {key, value} }) {};
		ModifyRequests(std::unordered_map<ChannelProperty, std::variant<std::string, int, bool>> requests) : requests(requests) {};

		void Add(ChannelProperty key, std::variant<std::string, int, bool> value) {
		    requests.insert({key, value});
		};

		void Remove(ChannelProperty key) {
		    requests.erase(requests.find(key));
		}
	};

	enum class GetChannelsMessagesMethod {
		AROUND,
		BEFORE,
		AFTER,
		LIMIT
	};

	struct File {
		std::string file_name;
		std::string file_path;
	};

    enum ChannelType : int {
        GUILD_TEXT,
        DM,
        GUILD_VOICE,
        GROUP_DM,
        GROUP_CATEGORY,
        GROUP_NEWS,
        GROUP_STORE
    };

	class Channel : public DiscordObject {
	public:
		Channel() = default;
        /**
         * @brief Constructs a discpp::Channel object from the id.
         *
         * ```cpp
         *      discpp::Channel channel(674023471063498772);
         * ```
         *
         * @param[in] id The channel id
         *
         * @return discpp::Channel, this is a constructor.
         */

		Channel(const snowflake& id);
        /**
         * @brief Constructs a discpp::Channel object from json.
         *
         * ```cpp
         *      discpp::channel channel(json);
         * ```
         *
         * @param[in] json The json data for the channel.
         *
         * @return discpp::Channel, this is a constructor.
         */
		Channel(rapidjson::Document& json);

		static discpp::Channel RequestChannel(discpp::snowflake);

        /**
         * @brief Send a message in this channel.
         *
         * ```cpp
         *      ctx.Send("Hello, I'm a bot!"); // Sending text
         *      ctx.Send("", false, embed); // Sending an embed
         *      ctx.Send("Command output was too large to fit in an embed.", false, nullptr, { file }); // Sending files
         * ```
         *
         * @param[in] text The text that goes along with the embed.
         * @param[in] tts Should it be a text to speech message?
         * @param[in] embed Embed to send
         * @param[in] files Files to send
         *
         * @return discpp::Message
         */
		discpp::Message Send(const std::string& text, const bool& tts = false, discpp::EmbedBuilder* embed = nullptr, std::vector<discpp::File> files = {});

        /**
         * @brief Modify the channel.
         *
         * Use discpp::ModifyRequest to modify a field of the channel.
         *
         * ```cpp
         *		// Change the name of the channel to "Test"
         *		discpp::ModifyRequests request(discpp::ChannelProperty::NAME, "Test");
         *      channel.Modify(request);
         * ```
         *
         * @param[in] modify_request The field to modify and what to set it to.
         *
         * @return discpp::Channel - This method also sets the channel reference to the returned channel.
         */
        discpp::Channel Modify(ModifyRequests& modify_requests);

        /**
         * @brief Delete this channel.
         *
         * ```cpp
         *      channel.Delete();
         * ```
         *
         * @return discpp::Channel - Returns a default channel object
         */

        discpp::Channel Delete();
        /**
         * @brief Get channel's messages depending on the given method.
         *
         * ```cpp
         *      std::vector<discpp::Message> messages = channel.GetChannelMessages(50);
         * ```
         *
         * @param[in] amount The amount of the messages to get unless the method is not "limit".
         * @param[in] get_method The method of how to get the messages.
         *
         * @return std::vector<discpp::Message>
         */
        std::vector<discpp::Message> GetChannelMessages(int amount, GetChannelsMessagesMethod get_method = GetChannelsMessagesMethod::LIMIT);

        /**
         * @brief Get a message from the channel from the id.
         *
         * ```cpp
         *      channel.FindMessage(685199299042476075);
         * ```
         *
         * @param[in] message_id The message id.
         *
         * @return discpp::Message
         */
        discpp::Message FindMessage(const snowflake& message_id);

        /**
         * @brief Triggers a typing indicator.
         *
         * ```cpp
         *      channel.TriggerTypingIndicator();
         * ```
         *
         * @return void
         */
		void TriggerTypingIndicator();

        /**
         * @brief Get all messages pinned to the channel.
         *
         * ```cpp
         *      channel.GetPinnedMessages();
         * ```
         *
         * @return std::vector<discpp::Message>
         */
		std::vector<discpp::Message> GetPinnedMessages();

        ChannelType type; /**< The type of channel. */
		std::string name; /**< The name of the channel. */
		std::string topic; /**< The channel topic. */
		snowflake last_message_id; /**< The ID of the last message sent in this channel. */
        // TODO: Convert to iso8601Time
		std::string last_pin_timestamp; /**< When the last pinned message was pinned. */
	};

	class GuildChannel : public Channel {
	public:
		GuildChannel() = default;

        /**
         * @brief Constructs a discpp::GuildChannel object from json.
         *
         * ```cpp
         *      discpp::GuildChannel GuildChannel(json);
         * ```
         *
         * @param[in] json The json data for the guild channel.
         *
         * @return discpp::GuildChannel, this is a constructor.
         */
		GuildChannel(rapidjson::Document& json);

        /**
         * @brief Constructs a discpp::GuildChannel from id and guild's id.
         *
         * ```cpp
         *      discpp::GuildChannel GuildChannel(channel_id, guild_id);
         * ```
         *
         * @param[in] id the id of the channel
         * @param[in] guild_id the guild id
         *
         * @return discpp::GuildChannel, this is a constructor.
         */
		GuildChannel(const snowflake& id, const snowflake& guild_id);

        /**
         * @brief Delete several messages (2-100).
         *
         * ```cpp
         *      channel.BulkDeleteMessage({message_a, message_b, message_c});
         * ```
         *
         * @param[in] messages The messages to delete.
         *
         * @return void
         */
		void BulkDeleteMessage(const std::vector<snowflake>& messages);

        /**
         * @brief Remove permission overwrites for this channel.
         *
         * ```cpp
         *      channel.DeletePermission(permissions);
         * ```
         *
         * @param[in] permissions The permissions that will be removed
         *
         * @return void
         */
		void DeletePermission(const discpp::Permissions& permissions); // TODO: https://discordapp.com/developers/docs/resources/channel#delete-channel-permission

        /**
         * @brief Edit permission overwrites for this channel.
         *
         * ```cpp
         *      channel.EditPermissions(permissions);
         * ```
         *
         * @param[in] permissions The permissions that the channels permission overwrites will be set to.
         *
         * @return void
         */
		void EditPermissions(const discpp::Permissions& permissions);

        /**
         * @brief Returns owning guild object
         *
         * ```cpp
         *      discpp::Guild = channel.GetGuild();
         * ```
         *
         * @return discpp::Guild
         */
		std::shared_ptr<discpp::Guild> GetGuild();

        /**
         * @brief Create an invite for the channel.
         *
         * ```cpp
         *      discpp::GuildInvite invite = channel.CreateInvite(86400, 5, true, true);
         * ```
         *
         * @param[in] max_age How long the invite will last for.
         * @param[in] max_uses Max uses of the invite.
         * @param[in] temporary Whether this invite only grants temporary membership.
         * @param[in] If true, dont try to reuse similar invites.
         *
         * @return discpp::GuildInvite
         */
		GuildInvite CreateInvite(const int& max_age, const int& max_uses, const bool& temporary, const bool& unique);
		std::vector<GuildInvite> GetInvites();

        bool operator==(GuildChannel& other) const {
            return this->id == other.id;
        }

        bool operator!=(GuildChannel& other) const {
            return this->id != other.id;
        }

		bool nsfw; /**< Whether or not the current channel is not safe for work. */
		int bitrate; /**< The bitrate (in bits) of the voice channel. */
		int position; /**< Position of channel in guild's channel list. */
		int rate_limit_per_user; /**< Amount of seconds a user has to wait before sending another message (0-21600); bots, as well as users with the permission manage_messages or manage_channel, are unaffected. */
		int user_limit; /**< The user limit of the voice channel. */
		snowflake guild_id; /**< Guild id of the current channel. */
		snowflake category_id; /**< ID of the parent category for a channel (each parent category can contain up to 50 channels). */
		std::vector<discpp::Permissions> permissions; /**< Explicit permission overwrites for members and roles. */
	};

    class CategoryChannel : public GuildChannel {
    public:
        CategoryChannel() = default;


        /**
         * @brief Constructs a discpp::CategoryChannel object from json.
         *
         * ```cpp
         *      discpp::CategoryChannel CategoryChannel(json);
         * ```
         *
         * @param[in] json The json data for the category channel.
         *
         * @return discpp::CategoryChannel, this is a constructor.
         */
        CategoryChannel(rapidjson::Document& json) : GuildChannel(json) {}

        /**
         * @brief Constructs a discpp::CategoryChannel from id and guild's id.
         *
         * ```cpp
         *      discpp::CategoryChannel CategoryChannel(channel_id, guild_id);
         * ```
         *
         * @param[in] id the id of the channel
         * @param[in] guild_id the guild id
         *
         * @return discpp::CategoryChannel, this is a constructor.
         */
        CategoryChannel(const snowflake& id, const snowflake& guild_id) : GuildChannel(id, guild_id) {}

        /**
         * @brief Lists children channels for this category.
         * ```cpp
         *      std::unordered_map<discpp::snowflake, discpp::GuildChannel> children = category.GetChildren();
         * ```
         *
         * @return std::unordered_map<discpp::snowflake, discpp::GuildChannel>
         */
        std::unordered_map<discpp::snowflake, discpp::GuildChannel> GetChildren();
    };

	class DMChannel : public Channel {
	public: 
		DMChannel() = default;


        /**
         * @brief Constructs a discpp::DMChannel from json.
         *
         * ```cpp
         *      discpp::DMChannel GuildChannel(json);
         * ```
         *
         * @param[in] json The json data for the dm channel.
         *
         * @return discpp::DMChannel, this is a constructor.
         */
		DMChannel(rapidjson::Document& json);

        /**
         * @brief Constructs a discpp::DMChannel from id.
         *
         * ```cpp
         *      discpp::DMChannel GuildChannel(channel_id);
         * ```
         *
         * @param[in] id the id of the channel
         *
         * @return discpp::DMChannel, this is a constructor.
         */
        DMChannel(const snowflake& id);


        /**
         * @brief Add a recipient to the group dm.
         *
         * This only works if the channel is a group dm.
         *
         * ```cpp
         *      channel.GroupDMAddRecipient(user);
         * ```
         *
         * @return void
         */
		void GroupDMAddRecipient(const discpp::User& user);

        /**
         * @brief Remove a recipient from the group dm.
         *
         * ```cpp
         *      channel.GroupDMRemoveRecipient(user);
         * ```
         *
         * @param[in] User The user to remove.
         *
         * @return void
         */
        void GroupDMRemoveRecipient(const discpp::User& user);

        bool operator==(GuildChannel& other) const {
            return this->id == other.id;
        }

        bool operator!=(GuildChannel& other) const {
            return this->id != other.id;
        }

		std::string icon; /**< Hashed icon for this channel. */
		snowflake owner_id; /**< ID of the DM creator. */
		snowflake application_id; /**< Application ID of the group DM creator if it is bot-created. */
		std::vector<discpp::User> recipients; /**< The recipients of the DM. */
	};
}

#endif