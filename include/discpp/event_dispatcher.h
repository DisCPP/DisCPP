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
	    inline static std::unordered_map<std::string, std::function<void(Shard& shard, rapidjson::Document&)>> internal_event_map = {};
        static std::unordered_map<int, rapidjson::Document> json_docs;

		static void ReadyEvent(Shard& shard, rapidjson::Document& result);
        static void ResumedEvent(Shard& shard, rapidjson::Document& result);
        static void ReconnectEvent(Shard& shard, rapidjson::Document& result);
        static void InvalidSessionEvent(Shard& shard, rapidjson::Document& result);
        static void ChannelCreateEvent(Shard& shard, rapidjson::Document& result);
        static void ChannelUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void ChannelDeleteEvent(Shard& shard, rapidjson::Document& result);
        static void ChannelPinsUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void GuildCreateEvent(Shard& shard, rapidjson::Document& result);
        static void GuildUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void GuildDeleteEvent(Shard& shard, rapidjson::Document& result);
        static void GuildBanAddEvent(Shard& shard, rapidjson::Document& result);
        static void GuildBanRemoveEvent(Shard& shard, rapidjson::Document& result);
        static void GuildEmojisUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void GuildIntegrationsUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void GuildMemberAddEvent(Shard& shard, rapidjson::Document& result);
        static void GuildMemberRemoveEvent(Shard& shard, rapidjson::Document& result);
        static void GuildMemberUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void GuildMembersChunkEvent(Shard& shard, rapidjson::Document& result);
        static void GuildRoleCreateEvent(Shard& shard, rapidjson::Document& result);
        static void GuildRoleUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void GuildRoleDeleteEvent(Shard& shard, rapidjson::Document& result);
        static void MessageCreateEvent(Shard& shard, rapidjson::Document& result);
        static void MessageUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void MessageDeleteEvent(Shard& shard, rapidjson::Document& result);
        static void MessageDeleteBulkEvent(Shard& shard, rapidjson::Document& result);
        static void MessageReactionAddEvent(Shard& shard, rapidjson::Document& result);
        static void MessageReactionRemoveEvent(Shard& shard, rapidjson::Document& result);
        static void MessageReactionRemoveAllEvent(Shard& shard, rapidjson::Document& result);
        static void PresenceUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void TypingStartEvent(Shard& shard, rapidjson::Document& result);
        static void UserUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void VoiceStateUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void VoiceServerUpdateEvent(Shard& shard, rapidjson::Document& result);
        static void WebhooksUpdateEvent(Shard& shard, rapidjson::Document& result);
	public:
        static void BindEvents();
		static void HandleDiscordEvent(Shard& shard, rapidjson::Document& j, const std::string& event_name);
        static void RegisterGatewayCustomEvent(const char* event_name, const std::function<void(Shard& shard, const rapidjson::Document&)>& func);
	};
}

#endif