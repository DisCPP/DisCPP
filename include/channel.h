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

	enum ModifyChannelValue : int {
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

	enum GetChannelsMessagesFields {
		AROUND,
		BEFORE,
		AFTER,
		LIMIT
	};

	struct File {
		std::string file_name;
		std::string file_path;
	};

	class Channel : DiscordObject {
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
		// std::vector<discord::Message> GetChannelMessages(GetChannelsMessagesFields messages_fields = GetChannelsMessagesFields::LIMIT); // TODO: https://discordapp.com/developers/docs/resources/channel#get-channel-messages
		discord::Message FindMessage(snowflake message_id);
		void BulkDeleteMessage(std::vector<snowflake> messages);
		// void EditPermissions() // TODO: https://discordapp.com/developers/docs/resources/channel#edit-channel-permissions
		std::vector<discord::GuildInvite> GetInvites();
		discord::GuildInvite CreateInvite(int max_age, int max_uses, bool temporary, bool unique);
		// void deletePermission() // TODO: https://discordapp.com/developers/docs/resources/channel#edit-channel-permissions
		void TriggerTypingIndicator();
		std::vector<discord::Message> GetPinnedMessages();
		void GroupDMAddRecipient(discord::User user); // TODO: Test
		void GroupDMRemoveRecipient(discord::User user); // TODO: Test

		snowflake id;
		int type;
		snowflake guild_id; // TODO: Convert to discord::Guild
		int position;
		std::vector<discord::Permissions> permissions;
		std::string name;
		std::string topic;
		bool nsfw;
		snowflake last_message_id;
		int bitrate;
		int user_limit;
		int rate_limit_per_user;
		std::vector<discord::User> recipients;
		std::string icon;
		snowflake owner_id;
		snowflake application_id;
		snowflake category_id;
		std::string last_pin_timestamp; // TODO: Convert to iso8601Time
	};
}

#endif