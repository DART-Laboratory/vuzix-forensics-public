#include "Reader.h"
#include <functional>
#include <iostream>
#include <print>

std::vector<std::shared_ptr<Log>> Reader::read_bugreport(const std::vector<std::string>& lines) {
	std::vector<std::shared_ptr<Log>> cumulative_logs{};

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

