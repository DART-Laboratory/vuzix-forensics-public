#pragma once

#include "Reader.h"
#include <optional>
#include <chrono>

using UID = uint64_t;
using PID = uint16_t;
using Package = std::string;

struct Proc {
	std::optional<Package> package;
	std::optional<std::string> name;

	struct IDs {
		UID uid;
		PID pid;
	};
	std::optional<IDs> ids;
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

struct Timeline {
	std::vector<std::unique_ptr<Event>> events;
};

class TimelineGenerator {
public:
	TimelineGenerator() = delete;
	
	static Timeline generate_timeline(const std::vector<std::shared_ptr<LogcatLog>>& logs);

private:
	static std::pair<std::optional<std::string>, std::string> get_package_and_name(const std::string& str, const char seperator = '/');

	static uint64_t get_uid(const std::string& str) noexcept;
};

