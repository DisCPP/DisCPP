#ifndef DISCORDPP_EVENT_FUNC_TYPE_H
#define DISCORDPP_EVENT_FUNC_TYPE_H

#include <functional>
#include <future>
#include <iostream>
#include <tuple>
#include <vector>

template <typename... Funcs>
struct Events {
	// The tuple vector is layed out like: func, <run_amount, ran_amount>
	std::tuple<std::vector<std::pair<std::function<Funcs>, std::pair<int,int>>>...> tuple;

	template <size_t index, typename Func>
	void add(Func&& func, int run_amount = -1) {
		std::get<index>(tuple).push_back({ std::forward<Func>(func), { run_amount, 0 } });
	}

	template <size_t index, typename... Args>
	void call(std::vector<std::future<void>>& future_lst, bool ready, Args&&... args) {
		if (!ready) {
			return;
		}

		int for_i = 0;
		for (auto& func : std::get<index>(tuple)) {
			future_lst.push_back(std::async(std::launch::async, func.first, std::forward<Args>(args)...));

			if (func.second.first != -1) {
				if (func.second.first == func.second.second) {
					std::get<index>(tuple).erase(std::get<index>(tuple).begin() + for_i);
				} else {
					func.second.second++;
				}
			}

			for_i++;
		}
	}
};

#endif