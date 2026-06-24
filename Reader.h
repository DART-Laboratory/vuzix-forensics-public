#pragma once

#include <vector>
#include <iostream>
#include <chrono>
#include <memory>

struct Log {
	~Log() = default;
};

struct LogcatLog : public Log {
	std::chrono::month_day date;
	std::chrono::time_point<std::chrono::system_clock> time;
	std::string process_name;
	std::string description;

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

	void print() const noexcept {
		std::cout
			<< "\n\tDate: " << date
			<< "\n\tTime: " << std::chrono::hh_mm_ss{time.time_since_epoch()}
			<< "\n\tType: " << log_type_chars[std::distance(log_type_enums.begin(), std::ranges::find(log_type_enums, type))]
			<< "\n\tProcess name: " << process_name
			<< "\n\tDescription: " << description
			<< "\n";
	}
};

class Reader {
public:
	Reader() = delete;

	static std::vector<std::shared_ptr<Log>> read_bugreport(const std::vector<std::string>& lines);
	static std::vector<std::shared_ptr<LogcatLog>> read_logcat(const std::vector<std::string>& lines);

private:
	static std::optional<std::shared_ptr<LogcatLog>> read_logcat_line(const std::string& l);
};

