#ifndef DISCORD_EVENT_LISTENER_H
#define DISCORD_EVENT_LISTENER_H

namespace discord {
	struct EventListenerHandle {
		unsigned int id;
	};

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Event, T>>>
	class EventHandler {
	public:
		using IdType = unsigned int;

		static EventListenerHandle RegisterListener(std::function<bool(const T&)> listener) {
			/**
			 * @brief Registers an event listener.
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

			auto id = GetNextId();
			GetHandlers()[id] = listener;
			return EventListenerHandle{ id };
		}

		static void RemoveListener(EventListenerHandle handle) {
			/**
			 * @brief Removes an event listener.
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

			GetHandlers().erase(handle.id);
		}

		static void TriggerEvent(T e) {
			/**
			 * @brief Triggers an event.
			 *
			 * ```cpp
			 *      discord::EventHandler<discord::MessageCreateEvent>::TriggerEvent(discord::MessageCreateEvent(created_message));
			 * ```
			 *
			 * @param[in] e The event to trigger.
			 *
			 * @return void
			 */

			for (const auto& h : GetHandlers())
			{
				h.second(e);
			}
		}

	private:
		static IdType GetNextId() {
			static IdType id = 0;
			return ++id;
		}

		static std::unordered_map<IdType, std::function<bool(const T&)>>& GetHandlers() {
			static std::unordered_map<IdType, std::function<bool(const T&)>> handlers;
			return handlers;
		}
	};

	template<typename T>
	class EventHandler<T*, std::enable_if_t<std::is_base_of_v<Event, T>>> {};

	template<typename T>
	class EventHandler<T&, std::enable_if_t<std::is_base_of_v<Event, T>>> {};

	template<typename T>
	class EventHandler<const T, std::enable_if_t<std::is_base_of_v<Event, T>>> {};

	// For convenience
	template<typename T>
	void DispatchEvent(const T& t) {
		/**
		 * @brief Dispatches an event, shorter than using TriggerEvent.
		 *
		 * ```cpp
		 *      DispatchEvent(discord::MessageCreateEvent(created_message));
		 * ```
		 *
		 * @param[in] t The event to dispatch.
		 *
		 * @return void
		 */

		EventHandler<T>::TriggerEvent(t);
	}

	// To let the caller pass pointers as the event object
	template<typename T, typename = std::enable_if_t<std::is_base_of_v<Event, T>>>
	void DispatchEvent(T* t) {
		/**
		 * @brief Dispatches an event pointer, shorter than using TriggerEvent.
		 *
		 * ```cpp
		 *      DispatchEvent(&discord::MessageCreateEvent(created_message));
		 * ```
		 *
		 * @param[in] t The event to dispatch.
		 *
		 * @return void
		 */
		//static_assert(std::is_base_of_v<Event, T>, "T must derive from Event");
		DispatchEvent<T>(*t);
	}
}

#endif