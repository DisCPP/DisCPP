#ifndef DISCORDPP_CHANNEL_H
#define DISCORDPP_CHANNEL_H

#include "discord_object.h"
#include "permission.h"
#include "embed_builder.h"

#include <nlohmann/json.hpp>

namespace discord {
	class Message;
	class GuildInvite;
	class User;
	class Guild;

	enum class ModifyChannelValue : int {
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

	struct ModifyRequest {
		ModifyChannelValue key;
		std::string value;

		ModifyRequest(ModifyChannelValue key, std::string value) : key(key), value(value) {

		};
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

	class Channel : public DiscordObject {
	public:
		Channel() = default;
		Channel(snowflake id);
		Channel(nlohmann::json json);
		Channel(nlohmann::json json, snowflake guild_id);

		discord::Message Send(std::string text, bool tts = false);
		discord::Message Send(discord::EmbedBuilder embed, std::string text = "");
		discord::Message Send(std::vector<File> files, std::string text = "");
		discord::Channel Modify(ModifyRequest modify_request);
		discord::Channel Delete();
		std::vector<discord::Message> GetChannelMessages(int amount, GetChannelsMessagesMethod get_method = GetChannelsMessagesMethod::LIMIT);
		discord::Message FindMessage(snowflake message_id);
		void BulkDeleteMessage(std::vector<snowflake> messages);
		// void EditPermissions() // TODO: https://discordapp.com/developers/docs/resources/channel#edit-channel-permissions
		std::vector<discord::GuildInvite> GetInvites();
		discord::GuildInvite CreateInvite(int max_age, int max_uses, bool temporary, bool unique);
		// void deletePermission() // TODO: https://discordapp.com/developers/docs/resources/channel#delete-channel-permission
		void TriggerTypingIndicator();
		std::vector<discord::Message> GetPinnedMessages();
		void GroupDMAddRecipient(discord::User user);
		void GroupDMRemoveRecipient(discord::User user);

		//snowflake id;
		int type;
		snowflake guild_id; /**< ID of the current channel's owning guild */
		int position; /**< Position of channel in guild's channel list */
		std::vector<discord::Permissions> permissions; /**< PermissionOverwrites of the current channel */
		std::string name; /**< Name of the current channel */
		std::string topic; /**< Topic of the current channel */
		bool nsfw; /**< Whether or not the current channel is not safe for work */
		snowflake last_message_id; /**< Last message sent in the current channel */
		int bitrate; /**< Bitrate the current voice channel is encoded at */
		int user_limit; /**< Max users the current voice channel can hold */
		int rate_limit_per_user; 
		std::vector<discord::User> recipients; /**< Members of the current groupdm channel */
		std::string icon; /**< Icon of the current groupdm channel */
		snowflake owner_id; /**< Owner of the current groupdm channel */
		snowflake application_id; 
		snowflake category_id; /**< Category the current channel is in */
		std::string last_pin_timestamp; // TODO: Convert to iso8601Time 
	};
}

#endif