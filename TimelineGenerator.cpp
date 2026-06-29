#include "TimelineGenerator.h"
#include <regex>

Timeline TimelineGenerator::generate_timeline(const std::vector<std::shared_ptr<LogcatLog>> &logs) {
	Timeline timeline{};
	
	for (const std::shared_ptr<LogcatLog>& log : logs) {
		log->print();
		if (log->process_name == "ActivityManager") {
			std::regex r{
				"Start proc ([0-9]{4}):([^/]+)\\/((u0ai?)?[0-9]+) for ((service)|(pre-top-activity)|(top-activity)|(broadcast)|(content provider)|( )|(added application)|(null)) ?\\{?(.*?(?=\\}|$|\n|\r))\\}?"
			};
			std::smatch m{};
			std::regex_match(log->description, m, r);
			if (m.empty()) continue;

			if (m[5] == "service" || m[5] == "pre-top-activity" ||
				m[5] == "top-activity" || m[5] == "content provider" ||
				m[5] == "broadcast") {
				const auto parent{get_package_and_name(m[14].str())};
				const auto child{get_package_and_name(m[2].str(), ':')};
				timeline.events.emplace_back(std::make_unique<ActivityStartEvent>(
					log->date,
					log->time,
					Proc{
						parent.first,
						parent.second,
						std::nullopt
					},
					Proc{
						child.first,
						child.second,
						Proc::IDs{
							get_uid(m[3]),
							(PID)std::stoi(m[1].str())
						}
					}
				));
			}
		}
	}

	return timeline;
}

std::pair<std::optional<std::string>, std::string> TimelineGenerator::get_package_and_name(const std::string& str, const char seperator) {
	const size_t slash{str.find(seperator)};
	std::pair<std::optional<std::string>, std::string> pair{std::make_pair(std::nullopt, "")};
	if (slash != std::string::npos) {
		pair.first = str.substr(0, slash);
		pair.second = str.substr(slash+1);
	} else {
		pair.second = str;
	}
	std::cout << pair.first.value_or("NO PKG") << ' ' << pair.second << std::endl;
	return pair;
}

static bool is_number(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

uint64_t TimelineGenerator::get_uid(const std::string& str) noexcept {
	if (is_number(str)) {
		return std::stoul(str);
	} if (str.starts_with("u0ai")) {
		return 90000ul+std::stoul(str.substr(4));
	} else if (str.starts_with("u0a")) {
		return 10000ul + std::stoul(str.substr(3));
	} else {
		return -1;
	}
}

