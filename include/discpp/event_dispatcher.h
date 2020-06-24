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
	    inline static std::unordered_map<std::string, std::function<void(rapidjson::Document &)>> internal_event_map = {};
        static std::unordered_map<int, rapidjson::Document> json_docs;

        static void RunEvent(const std::function<void(rapidjson::Document &)>& func, rapidjson::Document& json);

		static void ReadyEvent(rapidjson::Document& result);
        static void ResumedEvent(rapidjson::Document& result);
        static void ReconnectEvent(rapidjson::Document& result);
        static void InvalidSessionEvent(rapidjson::Document& result);
        static void ChannelCreateEvent(rapidjson::Document& result);
        static void ChannelUpdateEvent(rapidjson::Document& result);
        static void ChannelDeleteEvent(rapidjson::Document& result);
        static void ChannelPinsUpdateEvent(rapidjson::Document& result);
        static void GuildCreateEvent(rapidjson::Document& result);
        static void GuildUpdateEvent(rapidjson::Document& result);
        static void GuildDeleteEvent(rapidjson::Document& result);
        static void GuildBanAddEvent(rapidjson::Document& result);
        static void GuildBanRemoveEvent(rapidjson::Document& result);
        static void GuildEmojisUpdateEvent(rapidjson::Document& result);
        static void GuildIntegrationsUpdateEvent(rapidjson::Document& result);
        static void GuildMemberAddEvent(rapidjson::Document& result);
        static void GuildMemberRemoveEvent(rapidjson::Document& result);
        static void GuildMemberUpdateEvent(rapidjson::Document& result);
        static void GuildMembersChunkEvent(rapidjson::Document& result);
        static void GuildRoleCreateEvent(rapidjson::Document& result);
        static void GuildRoleUpdateEvent(rapidjson::Document& result);
        static void GuildRoleDeleteEvent(rapidjson::Document& result);
        static void MessageCreateEvent(rapidjson::Document& result);
        static void MessageUpdateEvent(rapidjson::Document& result);
        static void MessageDeleteEvent(rapidjson::Document& result);
        static void MessageDeleteBulkEvent(rapidjson::Document& result);
        static void MessageReactionAddEvent(rapidjson::Document& result);
        static void MessageReactionRemoveEvent(rapidjson::Document& result);
        static void MessageReactionRemoveAllEvent(rapidjson::Document& result);
        static void PresenceUpdateEvent(rapidjson::Document& result);
        static void TypingStartEvent(rapidjson::Document& result);
        static void UserUpdateEvent(rapidjson::Document& result);
        static void VoiceStateUpdateEvent(rapidjson::Document& result);
        static void VoiceServerUpdateEvent(rapidjson::Document& result);
        static void WebhooksUpdateEvent(rapidjson::Document& result);
	public:
        static void BindEvents();
		static void HandleDiscordEvent(rapidjson::Document& j, std::string event_name);
        [[maybe_unused]] static void RegisterCustomEvent(const char* event_name, const std::function<void(const rapidjson::Document&)>& func);
	};
}

#endif