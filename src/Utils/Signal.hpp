#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

template <typename... Args>
class Signal {
public:
	using Callback = std::function<void(Args...)>;
	using Handle = std::size_t;

private:
	struct Listener {
		Handle id;
		Callback callback;
	};

	std::vector<Listener> listeners;
	Handle nextId = 1;

public:
	Handle connect(Callback cb) {
		if (!cb)
			return 0;
		Handle id = nextId++;
		listeners.push_back({id, std::move(cb)});
		return id;
	}

	template <typename Fn>
	requires (sizeof...(Args) > 0 && !std::is_invocable_v<std::decay_t<Fn>&, Args...> && std::is_invocable_v<std::decay_t<Fn>&>)
	Handle connect(Fn&& fn) {
		return connect(Callback([f = std::forward<Fn>(fn)](Args...) mutable {
			std::invoke(f);
		}));
	}

	template <typename Obj, typename Method, typename... BoundArgs>
	requires (std::is_member_function_pointer_v<Method>)
	Handle connect(Obj* obj, Method method, BoundArgs&&... boundArgs) {
		if (!obj)
			return 0;
		return connect(Callback([obj, method, ...bound = std::forward<BoundArgs>(boundArgs)](Args... args) mutable {
			if constexpr (sizeof...(Args) > 0 && std::is_invocable_v<Method, Obj*, std::decay_t<BoundArgs>&..., Args...>) {
				std::invoke(method, obj, bound..., std::forward<Args>(args)...);
			} else {
				std::invoke(method, obj, bound...);
			}
		}));
	}

	void disconnect(Handle id) {
		listeners.erase(
			std::remove_if(listeners.begin(), listeners.end(),
				[id](const Listener& listener) { return listener.id == id; }),
			listeners.end()
		);
	}

	void clear() {
		listeners.clear();
	}

	bool empty() const {
		return listeners.empty();
	}

	void emit(Args... args) const {
		auto snapshot = listeners;
		for (const auto& listener : snapshot) {
			if (listener.callback) {
				listener.callback(args...);
			}
		}
	}

	void operator()(Args... args) const {
		emit(std::forward<Args>(args)...);
	}
};
