#pragma once

#include "Reader.h"
#include <optional>
#include <chrono>
#include <map>
#include <set>
#include <functional>

using UID = uint64_t;
using PID = uint16_t;
using Package = std::string;

struct Proc {
	std::optional<Package> package;
	std::optional<std::string> name;

	struct IDs {
		UID uid;
		std::optional<PID> pid;

		auto operator<=>(const IDs&) const noexcept = default;
	};
	std::optional<IDs> ids;

	enum class Type {
		Activity, Service, Broadcast, Application, ContentProvider, JavaClass
	};
	std::optional<Type> type;

	inline static const std::map<std::string, Type> log_to_type{
		{"pre-top-activity", Type::Activity},
		{"top-activity", Type::Activity},
		{"service", Type::Service},
		{"broadcast", Type::Broadcast},
		{"content provider", Type::ContentProvider},
	};

	auto operator<=>(const Proc&) const noexcept = default;
};

struct Event {
	std::chrono::month_day date;
	std::chrono::milliseconds time;

	Proc parent;
	Proc child;

	Event(
		const std::chrono::month_day& date,
		const std::chrono::milliseconds& time,
		const Proc& parent, const Proc& child
	) : date{date}, time{time}, parent{parent}, child{child} { }

	virtual inline std::string get_relation() const noexcept = 0;
};

struct ActivityStartEvent : public Event {
	ActivityStartEvent(
		const std::chrono::month_day& date,
		const std::chrono::milliseconds& time,
		const Proc& parent, const Proc& child
	) : Event{date, time, parent, child} { }

	inline std::string get_relation() const noexcept override {
		return "Started";
	}
};

struct ActivityStartForBroadcastEvent : public Event {
	ActivityStartForBroadcastEvent(
		const std::chrono::month_day& date,
		const std::chrono::milliseconds& time,
		const Proc& parent, const Proc& child
	) : Event{date, time, parent, child} { }

	inline std::string get_relation() const noexcept override {
		return "Started for broadcast";
	}
};

struct Timeline {
	std::vector<std::unique_ptr<Event>> events;

	template <typename T>
	std::map<std::optional<T>, std::set<Proc*>> sort_into(const std::function<std::optional<T>(const Proc&)> getter) const noexcept {
		std::map<std::optional<T>, std::set<Proc*>> package_procs{};
		for (const std::unique_ptr<Event>& event : events) {
			package_procs[getter(event->parent)].insert(&event->parent);
			package_procs[getter(event->child)].insert(&event->child);
		}

		return package_procs;
	}
	std::map<std::optional<Package>, std::set<Proc*>> sort_into_packages() const noexcept {
		return sort_into<Package>([](const Proc& p) {return p.package;});
	}
	std::map<std::optional<UID>, std::set<Proc*>> sort_into_uid() const noexcept {
		return sort_into<UID>([](const Proc& p) -> std::optional<UID> {
			if (p.ids.has_value()) {
				return p.ids.value().uid;
			} else {
				return std::nullopt;
			}
		});
	}

};

class TimelineGenerator {
public:
	TimelineGenerator() = delete;
	
	static Timeline generate_timeline(const std::vector<std::shared_ptr<LogcatLog>>& logs);

private:
	static std::pair<std::optional<Package>, std::string> get_package_and_name(const std::string& str, const std::vector<char> seperators = {':', '/'});

	static uint64_t get_uid(const std::string& str) noexcept;
};

