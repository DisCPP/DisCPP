#ifndef DISCPP_EVENT_LISTENER_H
#define DISCPP_EVENT_LISTENER_H

#include "event.h"
#include "utils.h"
#include "client.h"
#include "log.h"

#include <climits>

namespace discpp {
	struct EventListenerHandle {
		unsigned int id = UINT_MAX;
	};

	//template<typename T>
	class EventHandler {
	private:
	    discpp::Client* client;
	public:
        EventHandler(discpp::Client* client) : client(client) {}

		using IdType = unsigned int;

        template<typename T>
		EventListenerHandle RegisterListener(const std::function<void(const T&)>& listener) {
			/**
			 * @brief Registers an event listener.
			 *
			 * The given event class must derive from discpp::Event
			 *
			 * ```cpp
			 *      discpp::EventHandler<discpp::ChannelPinsUpdateEvent>::RegisterListener([](const discpp::ChannelPinsUpdateEvent& event) {
			 *			event.channel.Send("Detected a pin update!");
			 *		});
			 * ```
			 *
			 * @param[in] listener The code to execute when the event gets dispatched.
			 *
			 * @return discpp::EventListenerhandle
			 */

			// Make sure that the given event class derives from discpp::Event
			static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discpp::Event");

			client->logger->Debug(LogTextColor::GREEN + "Event listener registered: " + typeid(T).name());

			auto id = GetNextId();
			GetHandlers()[id] = [listener](const Event& base_evt) {
                const T* event = dynamic_cast<const T*>(&base_evt);

                if (event) {
                    listener(*event);
                }
			};
			return EventListenerHandle{ id };
		}

		void RemoveListener(const EventListenerHandle& handle) {
			/**
			 * @brief Removes an event listener.
			 *
			 * The given event class must derive from discpp::Event.
			 *
			 * ```cpp
			 *      auto eventListen = discpp::EventHandler<discpp::ChannelPinsUpdateEvent>::RegisterListener([](discpp::ChannelPinsUpdateEvent event)->bool {
			 *			event.channel.Send("Detected a pin update!");
			 *			return false;
			 *		});
			 *
			 *		discpp::EventHandler<discpp::ChannelPinsUpdateEvent>::RemoveListener(eventListen);
			 * ```
			 *
			 * @param[in] handle The event listener to remove.
			 *
			 * @return void
			 */

			//static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discpp::Event");

			//client->logger->Debug("Event listener removed: " + std::string(typeid(T).name()));

			GetHandlers().erase(handle.id);
		}

        template<typename T>
		void TriggerEvent(const discpp::Event& e) {
			/**
			 * @brief Triggers an event.
			 *
			 * The given event class must derive from discpp::Event. The event will be thrown on another thread.
			 *
			 * ```cpp
			 *      discpp::EventHandler<discpp::MessageCreateEvent>::TriggerEvent(discpp::MessageCreateEvent(created_message));
			 * ```
			 *
			 * @param[in] e The event to trigger.
			 *
			 * @return void
			 */

			//static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discpp::Event");

			client->logger->Debug("Event listener triggered: " + std::string(typeid(e).name()));

			for (std::pair<IdType, std::function<void(const T&)>> handler : GetHandlers()) {
			    std::function<void(const Event*)> func = [handler, e](const Event* base_evt) {
			        const T* event = dynamic_cast<const T*>(base_evt);

			        if (event) {
                        handler.second(*event);
			        }
			    };

			    client->DoFunctionLater(func, &e);
			}
		}

	private:
		IdType GetNextId() {
			static IdType id = 0;
			return ++id;
		}

		std::unordered_map<IdType, std::function<void(const discpp::Event&)>>& GetHandlers() {
			static std::unordered_map<IdType, std::function<void(const discpp::Event&)>> handlers;
			return handlers;
		}
	};

	/*template<typename T>
	class EventHandler<T*> {};

	template<typename T>
	class EventHandler<T&> {};

	template<typename T>
	class EventHandler<const T> {};

	// For convenience
	template<typename T>
	void DispatchEvent(const T& t) {
		*//**
		 * @brief Dispatches an event, shorter than using TriggerEvent.
		 *
		 * The given event class must derive from discpp::Event.
		 *
		 * ```cpp
		 *      DispatchEvent(discpp::MessageCreateEvent(created_message));
		 * ```
		 *
		 * @param[in] t The event to dispatch.
		 *
		 * @return void
		 *//*

		static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discpp::Event");

		EventHandler<T>::TriggerEvent(t);
	}

	// To let the caller pass pointers as the event object
	template<typename T>
	void DispatchEvent(T* t) {
		*//**
		 * @brief Dispatches an event pointer, shorter than using TriggerEvent.
		 *
		 * The given event class must derive from discpp::Event.
		 *
		 * ```cpp
		 *      DispatchEvent(&discpp::MessageCreateEvent(created_message));
		 * ```
		 *
		 * @param[in] t The event to dispatch.
		 *
		 * @return void
		 *//*

		static_assert(std::is_base_of_v<Event, T>, "Event class must derive from discpp::Event");

		DispatchEvent<T>(*t);
	}*/
}

#endif