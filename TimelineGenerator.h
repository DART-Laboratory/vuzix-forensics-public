#pragma once

#include <set>
#include <functional>
#include "Log.h"

struct Timeline {
	std::vector<Event> events;

	template <typename T>
	std::map<std::optional<T>, std::set<std::shared_ptr<Node>>> sort_into(const std::function<const std::optional<const T>(const std::shared_ptr<Node>&)> getter) noexcept {
		std::map<std::optional<T>, std::set<Proc*>> package_procs{};
		for (Event& event : events) {
			if (event.parent != nullptr) {
				package_procs[getter(event.parent)].insert(event.parent);
			}
			if (event.child != nullptr) {
				package_procs[getter(event.child)].insert(event.child);
			}
		}

		return package_procs;
	}
	/*std::map<std::optional<Package>, std::set<std::shared_ptr<Node>>> sort_into_packages() noexcept {
		return sort_into<Package>([](const std::shared_ptr<Node>& n) {
			if (std::dynamic_pointer_cast<
			return p.package;
		});
	}
	std::map<std::optional<UID>, std::set<Proc*>> sort_into_uid() noexcept {
		return sort_into<UID>([](const Proc& p) -> const std::optional<const UID> {
			if (p.ids.has_value()) {
				return p.ids.value().uid;
			} else {
				return std::nullopt;
			}
		});
	}*/
};

class TimelineGenerator {
public:
	TimelineGenerator() = delete;
	
	static Timeline generate_timeline(const std::vector<std::shared_ptr<Log>>& logs);
};

