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

namespace discpp {
	class EventDispatcher {
	private:
	    inline static std::unordered_map<std::string, std::function<void(Shard& shard, const discpp::JsonObject&)>> internal_event_map = {};
        static std::unordered_map<int, rapidjson::Document> json_docs;

		static void ReadyEvent(Shard& shard, const discpp::JsonObject& result);
        static void ResumedEvent(Shard& shard, const discpp::JsonObject& result);
        static void ReconnectEvent(Shard& shard, const discpp::JsonObject& result);
        static void InvalidSessionEvent(Shard& shard, const discpp::JsonObject& result);
        static void ChannelCreateEvent(Shard& shard, const discpp::JsonObject& result);
        static void ChannelUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void ChannelDeleteEvent(Shard& shard, const discpp::JsonObject& result);
        static void ChannelPinsUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildCreateEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildDeleteEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildBanAddEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildBanRemoveEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildEmojisUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildIntegrationsUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildMemberAddEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildMemberRemoveEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildMemberUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildMembersChunkEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildRoleCreateEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildRoleUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void GuildRoleDeleteEvent(Shard& shard, const discpp::JsonObject& result);
        static void MessageCreateEvent(Shard& shard, const discpp::JsonObject& result);
        static void MessageUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void MessageDeleteEvent(Shard& shard, const discpp::JsonObject& result);
        static void MessageDeleteBulkEvent(Shard& shard, const discpp::JsonObject& result);
        static void MessageReactionAddEvent(Shard& shard, const discpp::JsonObject& result);
        static void MessageReactionRemoveEvent(Shard& shard, const discpp::JsonObject& result);
        static void MessageReactionRemoveAllEvent(Shard& shard, const discpp::JsonObject& result);
        static void PresenceUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void TypingStartEvent(Shard& shard, const discpp::JsonObject& result);
        static void UserUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void VoiceStateUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void VoiceServerUpdateEvent(Shard& shard, const discpp::JsonObject& result);
        static void WebhooksUpdateEvent(Shard& shard, const discpp::JsonObject& result);
	public:
        static void BindEvents();
		static void HandleDiscordEvent(Shard& shard, const discpp::JsonObject& j, const std::string& event_name);
        static void RegisterGatewayCustomEvent(const char* event_name, const std::function<void(Shard& shard, const discpp::JsonObject&)>& func);
	};
}

#endif