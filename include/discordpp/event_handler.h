#ifndef DISCORD_EVENT_LISTENER_H
#define DISCORD_EVENT_LISTENER_H

#include "event.h"
#include "utils.h"

namespace discord {
	struct EventListenerHandle {
		unsigned int id;
	};

	template<typename T>
	class EventHandler {
	public:
		using IdType = unsigned int;

		static EventListenerHandle RegisterListener(std::function<void(const T&)> listener) {
			/**
			 * @brief Registers an event listener.
			 *
			 * The given event class must derive from discord::Event
			 *
			 * ```cpp
			 *      discord::EventHandler<discord::ChannelPinsUpdateEvent>::RegisterListener([](discord::ChannelPinsUpdateEvent event)->bool {
			 *			event.channel.Send("Detected a pin update!");
			 *			return false;
			 *		});
			 * ```
			 *
			 * @param[in] listener The code to execute when the event gets dispatched.
			 *
			 * @return discord::EventListenerhandle
			 */

			// Make sure that the given event class derives from discord::Event
			static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discord::Event");

			auto id = GetNextId();
			GetHandlers()[id] = listener;
			return EventListenerHandle{ id };
		}

		static void RemoveListener(EventListenerHandle handle) {
			/**
			 * @brief Removes an event listener.
			 *
			 * The given event class must derive from discord::Event.
			 *
			 * ```cpp
			 *      auto eventListen = discord::EventHandler<discord::ChannelPinsUpdateEvent>::RegisterListener([](discord::ChannelPinsUpdateEvent event)->bool {
			 *			event.channel.Send("Detected a pin update!");
			 *			return false;
			 *		});
			 *
			 *		discord::EventHandler<discord::ChannelPinsUpdateEvent>::RemoveListener(eventListen);
			 * ```
			 *
			 * @param[in] handle The event listener to remove.
			 *
			 * @return void
			 */

			static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discord::Event");

			GetHandlers().erase(handle.id);
		}

		static void TriggerEvent(T e) {
			/**
			 * @brief Triggers an event.
			 *
			 * The given event class must derive from discord::Event. The event will be thrown on another thread.
			 *
			 * ```cpp
			 *      discord::EventHandler<discord::MessageCreateEvent>::TriggerEvent(discord::MessageCreateEvent(created_message));
			 * ```
			 *
			 * @param[in] e The event to trigger.
			 *
			 * @return void
			 */

			static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discord::Event");

			for (std::pair<IdType, std::function<void(const T&)>> handler : GetHandlers()) {
				discord::globals::bot_instance->futures.push_back(std::async(std::launch::async, handler.second, e));
			}
		}

	private:
		static IdType GetNextId() {
			static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discord::Event");

			static IdType id = 0;
			return ++id;
		}

		static std::unordered_map<IdType, std::function<void(const T&)>>& GetHandlers() {
			static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discord::Event");

			static std::unordered_map<IdType, std::function<void(const T&)>> handlers;
			return handlers;
		}
	};

	template<typename T>
	class EventHandler<T*> {};

	template<typename T>
	class EventHandler<T&> {};

	template<typename T>
	class EventHandler<const T> {};

	// For convenience
	template<typename T>
	void DispatchEvent(const T& t) {
		/**
		 * @brief Dispatches an event, shorter than using TriggerEvent.
		 *
		 * The given event class must derive from discord::Event.
		 *
		 * ```cpp
		 *      DispatchEvent(discord::MessageCreateEvent(created_message));
		 * ```
		 *
		 * @param[in] t The event to dispatch.
		 *
		 * @return void
		 */

		static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discord::Event");

		EventHandler<T>::TriggerEvent(t);
	}

	// To let the caller pass pointers as the event object
	template<typename T>
	void DispatchEvent(T* t) {
		/**
		 * @brief Dispatches an event pointer, shorter than using TriggerEvent.
		 *
		 * The given event class must derive from discord::Event.
		 *
		 * ```cpp
		 *      DispatchEvent(&discord::MessageCreateEvent(created_message));
		 * ```
		 *
		 * @param[in] t The event to dispatch.
		 *
		 * @return void
		 */

		static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discord::Event");

		DispatchEvent<T>(*t);
	}
}

#endif