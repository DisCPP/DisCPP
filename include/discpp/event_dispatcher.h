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
        inline static std::unordered_map<std::string, std::function<void(nlohmann::json &)>> internal_event_map = {};

        static void RunEvent(const std::function<void(const nlohmann::json &)>& func, nlohmann::json& json);

		void ReadyEvent(const nlohmann::json& result);
		void ResumedEvent(const nlohmann::json& result);
		void ReconnectEvent(const nlohmann::json& result);
		void InvalidSessionEvent(const nlohmann::json& result);
		void ChannelCreateEvent(const nlohmann::json& result);
		void ChannelUpdateEvent(const nlohmann::json& result);
		void ChannelDeleteEvent(const nlohmann::json& result);
		void ChannelPinsUpdateEvent(const nlohmann::json& result);
		void GuildCreateEvent(const nlohmann::json& result);
		void GuildUpdateEvent(const nlohmann::json& result);
		void GuildDeleteEvent(const nlohmann::json& result);
		void GuildBanAddEvent(const nlohmann::json& result);
		void GuildBanRemoveEvent(const nlohmann::json& result);
		void GuildEmojisUpdateEvent(const nlohmann::json& result);
		void GuildIntegrationsUpdateEvent(const nlohmann::json& result);
		void GuildMemberAddEvent(const nlohmann::json& result);
		void GuildMemberRemoveEvent(const nlohmann::json& result);
		void GuildMemberUpdateEvent(const nlohmann::json& result);
		void GuildMembersChunkEvent(const nlohmann::json& result);
		void GuildRoleCreateEvent(const nlohmann::json& result);
		void GuildRoleUpdateEvent(const nlohmann::json& result);
		void GuildRoleDeleteEvent(const nlohmann::json& result);
		void MessageCreateEvent(const nlohmann::json& result);
		void MessageUpdateEvent(const nlohmann::json& result);
		void MessageDeleteEvent(const nlohmann::json& result);
		void MessageDeleteBulkEvent(const nlohmann::json& result);
		void MessageReactionAddEvent(const nlohmann::json& result);
		void MessageReactionRemoveEvent(const nlohmann::json& result);
		void MessageReactionRemoveAllEvent(const nlohmann::json& result);
		void PresenceUpdateEvent(const nlohmann::json& result);
		void TypingStartEvent(const nlohmann::json& result);
		void UserUpdateEvent(const nlohmann::json& result);
		void VoiceStateUpdateEvent(const nlohmann::json& result);
		void VoiceServerUpdateEvent(const nlohmann::json& result);
		void WebhooksUpdateEvent(const nlohmann::json& result);
	public:
		void BindEvents();
		static void HandleDiscordEvent(const nlohmann::json& j, std::string event_name);
	};
}

#endif