#pragma once

#include "Node.h"

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

	std::optional<Event> parse_activity_task_manager();
	std::optional<Event> parse_activity_manager();
	std::optional<Event> parse_camera_service();
};

class SensorRegisterLog : public Log {
public:
	static std::optional<std::shared_ptr<SensorRegisterLog>> read_log(const std::string& l);
	std::optional<Event> generate_event() override;

	std::chrono::milliseconds time;
	bool register_or_not;
	uint32_t sensor_id;
	PID pid;
	UID uid;
	Package package;

private:
	SensorRegisterLog() = default;
};

class SensorInfoLog : public Log {
public:
	static std::optional<std::shared_ptr<SensorInfoLog>> read_log(const std::string& l);
	std::optional<Event> generate_event() override { return std::nullopt; }

	uint32_t sensor_id;
	std::string component_name;
	std::string type;
	
private:
	SensorInfoLog() = default;
};

