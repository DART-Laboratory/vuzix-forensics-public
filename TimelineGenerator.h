#pragma once

#include <set>
#include <functional>
#include "Log.h"

struct Timeline {
	std::vector<Event> events;

	template <typename T>
	std::map<std::optional<T>, std::set<std::shared_ptr<Node>>> sort_into(const std::function<const std::optional<const T>(const std::shared_ptr<Node>&)> getter) noexcept {
		std::map<std::optional<T>, std::set<std::shared_ptr<Node>>> nodes{};
		for (Event& event : events) {
			if (event.parent != nullptr) {
				nodes[getter(event.parent)].insert(event.parent);
			}
			if (event.child != nullptr) {
				nodes[getter(event.child)].insert(event.child);
			}
		}

		return nodes;
	}
	std::map<std::optional<Package>, std::set<std::shared_ptr<Node>>> sort_into_packages() noexcept {
		return sort_into<Package>([](const std::shared_ptr<Node>& n) {
			return n->package;
		});
	}
};

class TimelineGenerator {
public:
	TimelineGenerator() = delete;
	
	static Timeline generate_timeline(const std::vector<std::shared_ptr<Log>>& logs);
};

