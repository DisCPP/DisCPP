#ifndef DISCPP_CHANNEL_H
#define DISCPP_CHANNEL_H

#define RAPIDJSON_HAS_STDSTRING 1

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
		Channel(snowflake id);
		Channel(rapidjson::Document& json);

		static discpp::Channel RequestChannel(discpp::snowflake);

		discpp::Message Send(std::string text, bool tts = false, discpp::EmbedBuilder* embed = nullptr, std::vector<discpp::File> files = {});
		discpp::Channel Modify(ModifyRequests& modify_requests);
		discpp::Channel Delete();
		std::vector<discpp::Message> GetChannelMessages(int amount, GetChannelsMessagesMethod get_method = GetChannelsMessagesMethod::LIMIT);
		discpp::Message FindMessage(snowflake message_id);
<<<<<<< Updated upstream
		void TriggerTypingIndicator();
=======
		inline void TriggerTypingIndicator();
>>>>>>> Stashed changes
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
		GuildChannel(rapidjson::Document& json);
		GuildChannel(snowflake id, snowflake guild_id);

		void BulkDeleteMessage(std::vector<snowflake>& messages);
		void DeletePermission(discpp::Permissions& permissions); // TODO: https://discordapp.com/developers/docs/resources/channel#delete-channel-permission
		void EditPermissions(discpp::Permissions& permissions);
		GuildInvite CreateInvite(int max_age, int max_uses, bool temporary, bool unique);
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

	class DMChannel : public Channel {
	public: 
		DMChannel() = default;
		DMChannel(rapidjson::Document& json);
		DMChannel(snowflake id);

		void GroupDMAddRecipient(discpp::User& user);
		void GroupDMRemoveRecipient(discpp::User& user);

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