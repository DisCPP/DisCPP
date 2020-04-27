#ifndef DISCPP_EVENT_DISPATCHER_H
#define DISCPP_EVENT_DISPATCHER_H

#include "event.h"
#include "utils.h"
#include "bot.h"
#include <string>
#include <future>
#include <string_view>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

namespace discpp {
	class EventDispatcher {
	private:
		void ReadyEvent(nlohmann::json& result);
		void ResumedEvent(nlohmann::json& result);
		void ReconnectEvent(nlohmann::json& result);
		void InvalidSessionEvent(nlohmann::json& result);
		void ChannelCreateEvent(nlohmann::json& result);
		void ChannelUpdateEvent(nlohmann::json& result);
		void ChannelDeleteEvent(nlohmann::json& result);
		void ChannelPinsUpdateEvent(nlohmann::json& result);
		void GuildCreateEvent(nlohmann::json& result);
		void GuildUpdateEvent(nlohmann::json& result);
		void GuildDeleteEvent(nlohmann::json& result);
		void GuildBanAddEvent(nlohmann::json& result);
		void GuildBanRemoveEvent(nlohmann::json& result);
		void GuildEmojisUpdateEvent(nlohmann::json& result);
		void GuildIntegrationsUpdateEvent(nlohmann::json& result);
		void GuildMemberAddEvent(nlohmann::json& result);
		void GuildMemberRemoveEvent(nlohmann::json& result);
		void GuildMemberUpdateEvent(nlohmann::json& result);
		void GuildMembersChunkEvent(nlohmann::json& result);
		void GuildRoleCreateEvent(nlohmann::json& result);
		void GuildRoleUpdateEvent(nlohmann::json& result);
		void GuildRoleDeleteEvent(nlohmann::json& result);
		void MessageCreateEvent(nlohmann::json& result);
		void MessageUpdateEvent(nlohmann::json& result);
		void MessageDeleteEvent(nlohmann::json& result);
		void MessageDeleteBulkEvent(nlohmann::json& result);
		void MessageReactionAddEvent(nlohmann::json& result);
		void MessageReactionRemoveEvent(nlohmann::json& result);
		void MessageReactionRemoveAllEvent(nlohmann::json& result);
		void PresenceUpdateEvent(nlohmann::json& result);
		void TypingStartEvent(nlohmann::json& result);
		void UserUpdateEvent(nlohmann::json& result);
		void VoiceStateUpdateEvent(nlohmann::json& result);
		void VoiceServerUpdateEvent(nlohmann::json& result);
		void WebhooksUpdateEvent(nlohmann::json& result);
	public:
		void BindEvents();
		static void HandleDiscordEvent(nlohmann::json& j, std::string event_name);
	};
}

#endif