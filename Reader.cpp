#include "Reader.h"
#include <regex>
#include <ranges>
#include <algorithm>

std::vector<std::shared_ptr<Log>> Reader::read_bugreport(const std::vector<std::string>& lines) {
	return read_logcat(lines) | std::views::transform([](const std::shared_ptr<LogcatLog>& log) -> std::shared_ptr<Log> {
		return std::static_pointer_cast<Log>(log);
	}) | std::ranges::to<std::vector<std::shared_ptr<Log>>>();

	/*auto logcat_begin{std::ranges::find_if(lines, [](const std::string& line) { return line.find("logcat"); })};
	if (logcat_begin != lines.end() && logcat_end
	return std::vector<std::string>{*/
}

std::vector<std::shared_ptr<LogcatLog>> Reader::read_logcat(const std::vector<std::string>& lines) {
	std::vector<std::shared_ptr<LogcatLog>> logs{};
	logs.reserve(lines.size());

	for (const std::string& l : lines) {
		std::optional<std::shared_ptr<LogcatLog>> log{read_logcat_line(l)};
		if (log) logs.emplace_back(std::move(*log));
	}

	return logs;
}

std::optional<std::shared_ptr<LogcatLog>> Reader::read_logcat_line(const std::string& l) {
	LogcatLog log{};

	const std::regex r{
		"^([0-9]{2}-[0-9]{2}) +([0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3})( +[0-9]{4}){2} +(F|E|W|I|D|V) (.*?(?= *:+ )) *:+ (.*)"
	};

	std::smatch m{};
	std::regex_match(l, m, r);

	if (m.empty()) [[unlikely]] return std::nullopt;

	std::istringstream ss{(m.begin()+1)->str()};
	ss >> std::chrono::parse("%m-%d", log.date);

	std::chrono::milliseconds ms{};
	ss = std::istringstream{(m.begin()+2)->str()};
	ss >> std::chrono::parse("%T", ms);
	log.time = std::chrono::time_point<std::chrono::system_clock>{ms};

	log.process_name = (m.begin()+5)->str();
	log.description = (m.begin()+6)->str();

	log.type = LogcatLog::log_type_enums[std::distance(
		LogcatLog::log_type_chars.begin(),
		std::ranges::find(LogcatLog::log_type_chars, (m.begin()+4)->str()[0])
	)];

	return std::make_unique<LogcatLog>(log);
}

