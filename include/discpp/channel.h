#ifndef DISCPP_CHANNEL_H
#define DISCPP_CHANNEL_H

#include "discord_object.h"
#include "permission.h"
#include "embed_builder.h"

#include <nlohmann/json.hpp>

#include <variant>

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
		Channel(snowflake id);
		Channel(nlohmann::json json);
		Channel(nlohmann::json json, snowflake guild_id);

		discpp::Message Send(std::string text, bool tts = false, discpp::EmbedBuilder* embed = nullptr, std::vector<discpp::File> files = {});
		discpp::Channel Modify(ModifyRequests modify_requests);
		discpp::Channel Delete();
		std::vector<discpp::Message> GetChannelMessages(int amount, GetChannelsMessagesMethod get_method = GetChannelsMessagesMethod::LIMIT);
		discpp::Message FindMessage(snowflake message_id);
		void BulkDeleteMessage(std::vector<snowflake> messages);
		void EditPermissions(discpp::Permissions permissions);
		std::vector<discpp::GuildInvite> GetInvites();
		discpp::GuildInvite CreateInvite(int max_age, int max_uses, bool temporary, bool unique);
		void DeletePermission(discpp::Permissions permissions); // TODO: https://discordapp.com/developers/docs/resources/channel#delete-channel-permission
		void TriggerTypingIndicator();
		std::vector<discpp::Message> GetPinnedMessages();
		void GroupDMAddRecipient(discpp::User user);
		void GroupDMRemoveRecipient(discpp::User user);

        ChannelType type; /**< The type of channel. */
		snowflake guild_id; /**< ID of the current channel's owning guild. */
		int position; /**< Position of channel in guild's channel list. */
		std::vector<discpp::Permissions> permissions; /**< Explicit permission overwrites for members and roles. */
		std::string name; /**< The name of the channel. */
		std::string topic; /**< The channel topic. */
		bool nsfw; /**< Whether or not the current channel is not safe for work. */
		snowflake last_message_id; /**< The ID of the last message sent in this channel. */
		int bitrate; /**< The bitrate (in bits) of the voice channel. */
		int user_limit; /**< The user limit of the voice channel. */
		int rate_limit_per_user; /**< Amount of seconds a user has to wait before sending another message (0-21600); bots, as well as users with the permission manage_messages or manage_channel, are unaffected. */
		std::vector<discpp::User> recipients; /**< The recipients of the DM. */
		std::string icon; /**< Hashed icon for this channel. */
		snowflake owner_id; /**< ID of the DM creator. */
		snowflake application_id; /**< Application ID of the group DM creator if it is bot-created. */
		snowflake category_id; /**< ID of the parent category for a channel (each parent category can contain up to 50 channels). */
        // TODO: Convert to iso8601Time
		std::string last_pin_timestamp; /**< When the last pinned message was pinned. */
	};
}

#endif