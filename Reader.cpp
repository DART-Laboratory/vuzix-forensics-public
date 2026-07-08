#include "Reader.h"
#include <functional>
#include <iostream>
#include <print>

std::vector<std::shared_ptr<Log>> Reader::read_bugreport(const std::vector<std::string>& lines) {
	std::vector<std::shared_ptr<Log>> cumulative_logs{};

	/*std::optional<
		std::function<std::optional<std::shared_ptr<Log>>(const std::string&)>
	> current_reader{};

	for (const std::string& l : lines) {
		std::optional<std::shared_ptr<Log>> log{};
		if (l.find(" was the duration of ") != std::string::npos) {
			current_reader = std::nullopt;
		} else if (l.starts_with("DUMP OF SERVICE CRITICAL")) {
			current_reader = SensorRegisterLog::read_log;
		} else if (l.starts_with("------ SYSTEM LOG (logcat")) {
			current_reader = LogcatLog::read_log;
		}

		if (current_reader.has_value()) {
			if (const auto& log{current_reader.value()(l)}; log.has_value())
				cumulative_logs.emplace_back(log.value());
		}
	}*/

	static const std::array<
		std::function<std::optional<std::shared_ptr<Log>>(const std::string&)>, 3
	> readers{
		LogcatLog::read_log, SensorRegisterLog::read_log, SensorInfoLog::read_log
	};

	for (const std::string& l : lines) {
		for (const auto& reader : readers) {
			if (const auto log{reader(l)}) {
				cumulative_logs.emplace_back(log.value());
				continue;
			}
		}
	}

	return cumulative_logs;
}

