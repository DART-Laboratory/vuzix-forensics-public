#pragma once

#include "Event.h"

class Log {
public:
	virtual std::optional<Event> generate_event() = 0;
};

class LogcatLog : public Log {
public:
	enum class Type {
		Fatal, Error, Warning, Info, Debug, Verbose
	} type;

	constexpr static std::array<char, 6> log_type_chars{
		'F', 'E', 'W', 'I', 'D', 'V'
	};
	constexpr static std::array<Type, 6> log_type_enums{
		Type::Fatal,	
		Type::Error,
		Type::Warning,
		Type::Info,
		Type::Debug,
		Type::Verbose,
	};

	static std::optional<std::shared_ptr<LogcatLog>> read_log(const std::string& l);
	std::optional<Event> generate_event() override;
	
	std::chrono::month_day date;
	std::chrono::milliseconds time;
	std::string process_name;
	std::string description;

private:
	LogcatLog() = default;
};

class SensorDumpLog : public Log {
public:
	static std::optional<std::shared_ptr<SensorDumpLog>> read_log(const std::string& l);
	std::optional<Event> generate_event() override;

	std::chrono::milliseconds time;
	bool register_or_not;
	uint32_t sensor_id;
	PID pid;
	UID uid;
	Package package;

private:
	SensorDumpLog() = default;
};

