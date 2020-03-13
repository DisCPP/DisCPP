#ifndef DISCORD_EVENT_LISTENER_H
#define DISCORD_EVENT_LISTENER_H

namespace discord {
	struct EventListenerHandle {
		unsigned int id;
	};

	template <typename T>
	class EventHandler {
	public:
		using IdType = unsigned int;

		static EventListenerHandle RegisterListener(std::function<bool(const T&)> listener) {
			auto id = GetNextId();
			GetHandlers()[id] = listener;
			return EventListenerHandle{ id };
		}

		static void RemoveListener(EventListenerHandle handle) {
			GetHandlers().erase(handle.id);
		}

		static void TriggerEvent(T e) {
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
	class EventHandler<T*> {};

	template<typename T>
	class EventHandler<T&> {};

	template <typename T>
	class EventHandler<const T> {};

	// For convenience
	template <typename T>
	void DispatchEvent(const T& t) {
		EventHandler<T>::TriggerEvent(t);
	}

	// To let the caller pass pointers as the event object
	template <typename T>
	void DispatchEvent(T* t) {
		DispatchEvent<T>(*t);
	}
}

#endif