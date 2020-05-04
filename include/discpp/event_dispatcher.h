#ifndef DISCPP_EVENT_DISPATCHER_H
#define DISCPP_EVENT_DISPATCHER_H

#include "event.h"
#include "utils.h"
#include "client.h"
#include <string>
#include <future>
#include <string_view>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

namespace discpp {
	class EventDispatcher {
	private:
        inline static std::unordered_map<std::string, std::function<void(rapidjson::Document &)>> internal_event_map = {};

        static void RunEvent(const std::function<void(const rapidjson::Document &)>& func, rapidjson::Document& json);

		void ReadyEvent(rapidjson::Document& result);
		void ResumedEvent(rapidjson::Document& result);
		void ReconnectEvent(rapidjson::Document& result);
		void InvalidSessionEvent(rapidjson::Document& result);
		void ChannelCreateEvent(rapidjson::Document& result);
		void ChannelUpdateEvent(rapidjson::Document& result);
		void ChannelDeleteEvent(rapidjson::Document& result);
		void ChannelPinsUpdateEvent(rapidjson::Document& result);
		void GuildCreateEvent(rapidjson::Document& result);
		void GuildUpdateEvent(rapidjson::Document& result);
		void GuildDeleteEvent(rapidjson::Document& result);
		void GuildBanAddEvent(rapidjson::Document& result);
		void GuildBanRemoveEvent(rapidjson::Document& result);
		void GuildEmojisUpdateEvent(rapidjson::Document& result);
		void GuildIntegrationsUpdateEvent(rapidjson::Document& result);
		void GuildMemberAddEvent(rapidjson::Document& result);
		void GuildMemberRemoveEvent(rapidjson::Document& result);
		void GuildMemberUpdateEvent(rapidjson::Document& result);
		void GuildMembersChunkEvent(rapidjson::Document& result);
		void GuildRoleCreateEvent(rapidjson::Document& result);
		void GuildRoleUpdateEvent(rapidjson::Document& result);
		void GuildRoleDeleteEvent(rapidjson::Document& result);
		void MessageCreateEvent(rapidjson::Document& result);
		void MessageUpdateEvent(rapidjson::Document& result);
		void MessageDeleteEvent(rapidjson::Document& result);
		void MessageDeleteBulkEvent(rapidjson::Document& result);
		void MessageReactionAddEvent(rapidjson::Document& result);
		void MessageReactionRemoveEvent(rapidjson::Document& result);
		void MessageReactionRemoveAllEvent(rapidjson::Document& result);
		void PresenceUpdateEvent(rapidjson::Document& result);
		void TypingStartEvent(rapidjson::Document& result);
		void UserUpdateEvent(rapidjson::Document& result);
		void VoiceStateUpdateEvent(rapidjson::Document& result);
		void VoiceServerUpdateEvent(rapidjson::Document& result);
		void WebhooksUpdateEvent(rapidjson::Document& result);
	public:
		void BindEvents();
		static void HandleDiscordEvent(rapidjson::Document& j, std::string event_name);
	};
}

#endif