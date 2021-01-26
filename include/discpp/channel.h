#ifndef DISCPP_CHANNEL_H
#define DISCPP_CHANNEL_H

#include "discord_object.h"
#include "permission.h"
#include "embed_builder.h"
#include "utils.h"
#include "user.h"

#include <variant>
#include <vector>
#include <optional>

namespace discpp {
	class Message;
	class GuildInvite;
	//class User;
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

	struct RequestChannelsMessageMethod {
	    Snowflake around_id = 0;
        Snowflake before_id = 0;
        Snowflake after_id = 0;
	};

    /**
     * @brief Helper method for requesting channel messages around a specific message id.
     *
     * @param[in] around_id The message id to get messages around.
     *
     * @return RequestChannelsMessageMethod
     */
    inline RequestChannelsMessageMethod RequestMessagesAround(Snowflake around_id) {
        RequestChannelsMessageMethod request;
        request.around_id = around_id;
        return request;
    }

    /**
     * @brief Helper method for requesting channel messages before a specific message id.
     *
     * @param[in] before_id The message id to get messages before.
     *
     * @return RequestChannelsMessageMethod
     */
    inline RequestChannelsMessageMethod RequestMessagesBefore(Snowflake before_id) {
        RequestChannelsMessageMethod request;
        request.before_id = before_id;
        return request;
    }

    /**
     * @brief Helper method for requesting channel messages after a specific message id.
     *
     * @param[in] after_id The message id to get messages after.
     *
     * @return RequestChannelsMessageMethod
     */
    inline RequestChannelsMessageMethod RequestMessagesAfter(Snowflake after_id) {
        RequestChannelsMessageMethod request;
        request.after_id = after_id;
        return request;
    }

	struct File {
		std::string file_name;
		std::string file_path;
	};

	class Channel : public DiscordObject {
	public:
	    Channel() = default;

		Channel(discpp::Client* client);

        /**
         * @brief Constructs a discpp::Channel object from the id.
         *
         * If you set `can_request` to true, and the message is not found in cache, then we will request
         * the message from the REST API. But if its not true, and its not found, an exception will be
         * thrown of DiscordObjectNotFound.
         *
         * @param[in] id The id of the channel.
         * @param[in] can_request Whether or not the library can request the message from the REST API.
         *
         * @return discpp::Channel, this is a constructor.
         */
		Channel(discpp::Client* client, const Snowflake& id, bool can_request = false);

        /**
         * @brief Constructs a discpp::Channel object from json.
         *
         * @param[in] json The json data for the channel.
         *
         * @return discpp::Channel, this is a constructor.
         */
		Channel(discpp::Client* client, rapidjson::Document& json);

        /**
         * @brief Requests a channel from discord's api.
         *
         * ```cpp
         *      discpp::Channel channel = discpp::Channel::RequestChannel(channel_id);
         * ```
         *
         * @param[in] id The channel id.
         *
         * @return discpp::Channel
         */
		static discpp::Channel RequestChannel(discpp::Client* client, discpp::Snowflake id); // @TODO: Remove due to discpp::Cache

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
		discpp::Message Send(const std::string& text, const bool tts = false, discpp::EmbedBuilder* embed = nullptr, std::vector<discpp::File> files = {}) const;

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
         * @return void
         */

        void Delete();

        /**
         * @brief Get channel's messages depending on the given method.
         *
         * If you set the RequestChannelsMessageMethod yourself, dont set more than one of the message ids.
         *
         *
         * ```cpp
         *      std::vector<discpp::Message> messages_after = channel.RequestMessages(50, RequestChannelsMessageAfter(725152124471738388));
         *      std::vector<discpp::Message> messages_before = channel.RequestMessages(50, RequestChannelsMessageBefore(725152124471738388));
         *      std::vector<discpp::Message> messages_around = channel.RequestMessages(50, RequestChannelsMessageAround(725152124471738388));
         * ```
         *
         * @param[in] amount The amount of the messages to get unless the method is not "limit".
         * @param[in] get_method The method of how to get the messages.
         *
         * @return std::vector<discpp::Message>
         */
        std::vector<discpp::Message> RequestMessages(int amount, RequestChannelsMessageMethod get_method = {}) const;

        /**
         * @brief Requests the channel's message from the discord api.
         *
         * @param[in] id The message id
         *
         * @return discpp::Message
         */
        discpp::Message RequestMessage(const discpp::Snowflake& message_id);

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
        void BulkDeleteMessage(const std::vector<Snowflake>& messages);

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
         * @return std::shared_ptr<discpp::Guild>
         */
        [[nodiscard]] std::shared_ptr<discpp::Guild> GetGuild() const;

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
        GuildInvite CreateInvite(const int& max_age, const int& max_uses, const bool temporary, const bool unique);

        /**
         * @brief Lists all active invites for this channel
         * ```cpp
         *      std::optional<std::vector<GuildInvite>> invites = channel.GetInvites();
         * ```
         *
         * @return std::optional<std::vector<GuildInvite>>
         */

        std::optional<std::vector<GuildInvite>> GetInvites();

        /**
         * @brief Lists children channels for this category.
         * ```cpp
         *      auto children = category.GetChildren();
         * ```
         *
         * @return std::unordered_map<discpp::Snowflake, discpp::Channel>
         */
        std::unordered_map<discpp::Snowflake, discpp::Channel> GetChildren();

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

        /**
         * @brief Retrieve channel icon url.
         *
         * @param[in] img_type Optional parameter for type of image
         *
         * @return std::string
         */
        std::string GetIconURL(const ImageType& img_type = ImageType::AUTO) const;

        inline std::string GetFormattedLastPinTimestamp() const {
            return FormatTime(this->last_pin_timestamp);
        }

        ChannelType type; /**< The type of channel. */
		std::string name; /**< The name of the channel. */
		std::string topic; /**< The channel topic. */
		Snowflake last_message_id; /**< The ID of the last message sent in this channel. */
		time_t last_pin_timestamp; /**< When the last pinned message was pinned. */
        bool nsfw; /**< Whether or not the current channel is not safe for work. */
        int bitrate; /**< The bitrate (in bits) of the voice channel. */
        int position; /**< Position of channel in guild's channel list. */
        int rate_limit_per_user; /**< Amount of seconds a user has to wait before sending another message (0-21600); bots, as well as users with the permission manage_messages or manage_channel, are unaffected. */
        int user_limit; /**< The user limit of the voice channel. */
        Snowflake guild_id; /**< Guild id of the current channel. */
        Snowflake category_id = 0; /**< ID of the parent category for a channel (each parent category can contain up to 50 channels). */
        std::vector<discpp::Permissions> permissions; /**< Explicit permission overwrites for members and roles. */
        Snowflake owner_id; /**< ID of the DM creator. */
        Snowflake application_id; /**< Application ID of the group DM creator if it is bot-created. */
        std::vector<discpp::User> recipients; /**< The recipients of the DM. */
	private:
        uint64_t icon_hex[2] = {0, 0};
        bool is_icon_gif = false;
	};
}

#endif