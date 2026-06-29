#pragma once

#include "TimelineGenerator.h"
#include <map>
#include <functional>
#include <set>

class Builder {
public:
	Builder() = delete;

	//static std::string build_graph(const std::vector<ProcRelation>& relations);
	static std::string build_graph(const Timeline& timeline);
	
private:
	template <typename T>
	static std::map<std::optional<T>, std::set<Proc>> sort_into(const Timeline& timeline, const std::function<std::optional<T>(const Proc&)> getter) {
		std::map<std::optional<T>, std::set<Proc>> package_procs{};
		for (const std::unique_ptr<Event>& event : timeline.events) {
			package_procs[getter(event->parent)].insert(event->parent);
			package_procs[getter(event->child)].insert(event->child);
		}

		return package_procs;
	}
	static std::map<std::optional<Package>, std::set<Proc>> sort_into_packages(const Timeline& timeline) {
		return sort_into<Package>(timeline, [](const Proc& p) {return p.package;});
	}
	static std::map<std::optional<UID>, std::set<Proc>> sort_into_uid(const Timeline& timeline) {
		return sort_into<UID>(timeline, [](const Proc& p) -> std::optional<UID> {
			if (p.ids.has_value()) {
				return p.ids.value().uid;
			} else {
				return std::nullopt;
			}
		});
	}

	inline static const std::string HEADER{
		"digraph G {\n"
		"layout=fdp\n"
	};

	inline static const std::string FOOTER{
		"}\n"
	};

	inline static const std::string NO_PACKAGE{"No package"};
	inline static const std::string NO_NAME{"No name"};
};

