#include "TimelineGenerator.h"
#include <regex>
#include <stdexcept>
#include "trim.h"

Timeline TimelineGenerator::generate_timeline(const std::vector<std::shared_ptr<LogcatLog>> &logs) {
	Timeline timeline{};
	
	for (const std::shared_ptr<LogcatLog>& log : logs) {
		if (log->process_name == "ActivityTaskManager") {
			std::regex r{"START u\\d+ \\{(?:.*?\\bpkg=([^ \\}\\r\\n]+))?.*?\\bcmp=([^ \\}\r\n]+)(?: \\(has extras\\))?\\} from uid (.*)"};
			std::smatch m{};
			std::regex_match(log->description, m, r);
			if (m.empty()) continue;

			std::string pkg{m[1].str()};
			std::pair<std::optional<Package>, std::string> cmp{get_package_and_name(m[2].str())};
			
			timeline.events.emplace_back(std::make_unique<ActivityStartEvent>(
				log->date,
				log->time,
				Proc{
					std::nullopt,
					std::nullopt,
					Proc::IDs{
						get_uid(m[3].str()),
						std::nullopt
					},
					std::nullopt
				},
				Proc{
					(!pkg.empty() ? pkg : cmp.first),
					cmp.second,
					std::nullopt,
					Proc::Type::Activity
				}
			));
		} else if (log->process_name == "ActivityManager") {
			std::regex r{
				"Start proc ([0-9]{4}):([^/]+)\\/((u0ai?)?[0-9]+)( \\[[^\\]]+\\])? for ((service)|(pre-top-activity)|(top-activity)|(broadcast)|(content provider)|( )|(added application)|(null)) ?\\{?(.*?(?=\\}|$|\n|\r))\\}?"
			};
			std::smatch m{};
			std::regex_match(log->description, m, r);
			if (m.empty()) continue;

			if (m[6] == "service" || m[6] == "pre-top-activity" ||
				m[6] == "top-activity" || m[6] == "content provider" ||
				m[6] == "broadcast") {
				const auto parent{get_package_and_name(m[15].str())};
				const auto child{get_package_and_name(m[2].str())};

				Proc parent_proc{
					parent.first,
					parent.second,
					std::nullopt,
					Proc::log_to_type.at(m[6].str())
				};
				Proc child_proc{
					child.first,
					child.second,
					Proc::IDs{
						get_uid(m[3]),
						(PID)std::stoi(m[1].str())
					},
					Proc::Type::Activity
				};

				if (m[6] == "broadcast") {
					std::swap(parent_proc, child_proc);
					timeline.events.emplace_back(std::make_unique<ActivityStartForBroadcastEvent>(
						log->date, log->time, parent_proc, child_proc
					));
				} else {
					timeline.events.emplace_back(std::make_unique<ActivityStartEvent>(
						log->date, log->time, parent_proc, child_proc
					));
				}

			} else if (m[6] == "null") {
				Proc parent{};
				if (!m[5].str().empty()) {
					parent.name = m[5].str();
					parent.type = Proc::Type::JavaClass;
				}

				const auto child{get_package_and_name(m[2].str())};
				timeline.events.emplace_back(std::make_unique<ActivityStartEvent>(
					log->date,
					log->time,
					parent,
					Proc{
						child.first,
						child.second,
						Proc::IDs{
							get_uid(m[3]),
							(PID)std::stoi(m[1].str())
						},
						Proc::Type::Activity
					}
				));
			}
		}
	}

	return timeline;
}

std::pair<std::optional<Package>, std::string> TimelineGenerator::get_package_and_name(const std::string& str, const std::vector<char> seperator) {
	std::pair<std::optional<Package>, std::string> ret{};
	for (char c : seperator) {
		const size_t sep{str.find(c)};
		std::pair<std::optional<std::string>, std::string> pair{std::make_pair(std::nullopt, "")};
		if (sep != std::string::npos) {
			pair.first = str.substr(0, sep);
			pair.second = str.substr(sep+1);
		} else {
			pair.second = str;
		}

		if (!ret.first.has_value() && !pair.second.empty()) {
			ret = pair;
		}
	}

	if (ret.second.empty()) {
		throw std::runtime_error{"Package name empty when splitting"};
	}

	return ret;
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

