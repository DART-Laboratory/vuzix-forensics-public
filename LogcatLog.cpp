#include "Log.h"
#include "Node.h"
#include <regex>
#include <ranges>
#include <algorithm>
#include <print>
#include <iostream>

static std::pair<std::optional<Package>, std::string> get_package_and_name(const std::string& str, const std::vector<char> seperator = {'/', ':'}) {
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
		throw std::runtime_error{"Name empty when splitting"};
	}

	return ret;
}

static bool is_number(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

static uint64_t get_uid(const std::string& str) noexcept {
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

std::optional<std::shared_ptr<LogcatLog>> LogcatLog::read_log(const std::string& l) {
	LogcatLog log{};

	const std::regex r{
		"^([0-9]{2}-[0-9]{2}) +([0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3})( +[0-9]{4})+ +(F|E|W|I|D|V) (.*?(?= *:+ )) *:+ (.*)"
	};

	std::smatch m{};
	std::regex_match(l, m, r);

	if (m.empty()) [[unlikely]] return std::nullopt;

	std::istringstream ss{m[1].str()};
	ss >> std::chrono::parse("%m-%d", log.date);
	ss = std::istringstream{m[2].str()};
	ss >> std::chrono::parse("%T", log.time);

	if (!ss || !log.date.ok()) { [[unlikely]]
		return std::nullopt;
	}

	log.process_name = (m.begin()+5)->str();
	log.description = (m.begin()+6)->str();

	log.type = LogcatLog::log_type_enums[std::distance(
		LogcatLog::log_type_chars.begin(),
		std::ranges::find(LogcatLog::log_type_chars, (m.begin()+4)->str()[0])
	)];

	return std::make_shared<LogcatLog>(log);
}

std::optional<Event> LogcatLog::generate_event() {
	if (process_name == "ActivityTaskManager") {
		return parse_activity_task_manager();
	} else if (process_name == "ActivityManager") {
		return parse_activity_manager();
	} else if (process_name == "CameraService") {
		return parse_camera_service();
	}
	
	return std::nullopt;
}

std::optional<Event> LogcatLog::parse_activity_task_manager() {
	std::regex r{"START u\\d+ \\{(?:.*?\\bpkg=([^ \\}\\r\\n]+))?.*?\\bcmp=([^ \\}\r\n]+)(?: \\(has extras\\))?\\} from uid (.*)"};
	std::smatch m{};
	std::regex_match(description, m, r);
	if (m.empty()) return std::nullopt;

	std::string pkg{m[1].str()};
	std::pair<std::optional<Package>, std::string> cmp{get_package_and_name(m[2].str())};
	
	return Event{
		date,
		time,
		std::make_shared<Proc>(
			std::nullopt,
			std::nullopt,
			Proc::IDs{
				get_uid(m[3].str()),
				std::nullopt
			}
		),
		Event::Relation::Started,
		std::make_shared<Activity>(
			(!pkg.empty() ? pkg : cmp.first),
			cmp.second
		)
	};
}

std::optional<Event> LogcatLog::parse_activity_manager() {
	std::regex r{
		"Start proc ([0-9]{4}):([^/]+)\\/((u0ai?)?[0-9]+)( \\[[^\\]]+\\])? for ((service)|(pre-top-activity)|(top-activity)|(broadcast)|(content provider)|( )|(added application)|(null)) ?\\{?(.*?(?=\\}|$|\n|\r))\\}?"
	};
	std::smatch m{};
	std::regex_match(description, m, r);
	if (m.empty()) return std::nullopt;

	if (m[6] == "content_provider") {
		const auto parent{get_package_and_name(m[15].str())};
		const auto child{get_package_and_name(m[2].str())};

		std::shared_ptr<Proc> parent_proc{std::make_shared<Proc>(
			parent.first,
			parent.second,
			std::nullopt
		)};
		std::shared_ptr<Proc> child_proc{std::make_shared<Proc>(
			child.first,
			child.second,
			Proc::IDs{
				get_uid(m[3].str()),
				(PID)std::stoi(m[1].str())
			}
		)};

		return Event{
			date, time, parent_proc, Event::Relation::Started, child_proc
		};
	} else if (m[6] == "service") {
		const auto service_name{get_package_and_name(m[15].str())};
		std::shared_ptr<Service> service{std::make_shared<Service>(
			service_name.first,
			service_name.second
		)};

		const auto proc_name{get_package_and_name(m[2].str())};
		std::shared_ptr<Proc> proc{std::make_shared<Proc>(
			proc_name.first,
			proc_name.second,
			Proc::IDs{
				get_uid(m[3].str()),
				(PID)std::stoi(m[1].str())
			}
		)};
		return Event{
			date, time, proc, Event::Relation::StartedForService, service
		};
	} else if (m[6] == "broadcast") {
		const auto broadcast_name{get_package_and_name(m[15].str())};
		std::shared_ptr<BroadcastReceiver> broadcast{std::make_shared<BroadcastReceiver>(
			broadcast_name.first,
			broadcast_name.second
		)};

		const auto proc_name{get_package_and_name(m[2].str())};
		std::shared_ptr<Proc> proc{std::make_shared<Proc>(
			proc_name.first,
			proc_name.second,
			Proc::IDs{
				get_uid(m[3].str()),
				(PID)std::stoi(m[1].str())
			}
		)};
		return Event{
			date, time, proc, Event::Relation::StartedForBroadcast, broadcast
		};
	} else if (m[6] == "pre-top-activity" || m[6] == "top-activity") {
		const auto activity_name{get_package_and_name(m[15].str())};
		std::shared_ptr<Activity> activity{std::make_shared<Activity>(
			activity_name.first,
			activity_name.second
		)};

		const auto proc_name{get_package_and_name(m[2].str())};
		std::shared_ptr<Proc> proc{std::make_shared<Proc>(
			proc_name.first,
			proc_name.second,
			Proc::IDs{
				get_uid(m[3].str()),
				(PID)std::stoi(m[1].str())
			}
		)};
		return Event{
			date, time, activity, Event::Relation::Started, proc
		};
	} else if (m[6] == "null") {
		std::shared_ptr<Node> parent{nullptr};
		if (!m[5].str().empty()) {
			parent = std::make_shared<JavaClass>(
				m[5].str()
			);
		}

		const auto child{get_package_and_name(m[2].str())};
		return Event{
			date,
			time,
			parent,
			Event::Relation::Started,
			std::make_shared<Proc>(
				child.first,
				child.second,
				Proc::IDs{
					get_uid(m[3]),
					(PID)std::stoi(m[1].str())
				}
			)
		};
	} else if (m[3].str().contains("ai")) {
		const auto parent{get_package_and_name(m[15].str())};
		const auto child{get_package_and_name(m[2].str())};
		return Event{
			date,
			time,
			std::make_shared<Proc>(
				std::nullopt,
				parent.first,
				std::nullopt
			),
			Event::Relation::Started,
			std::make_shared<Proc>(
				child.first,
				child.second,
				Proc::IDs{
					get_uid(m[3]),
					(PID)std::stoi(m[1].str())
				}
			)
		};
	} else if (m[6] == "added application") {
		const auto child{get_package_and_name(m[2].str())};
		return Event{
			date,
			time,
			nullptr,
			Event::Relation::Started,
			std::make_shared<Proc>(
				child.first,
				child.second,
				Proc::IDs{
					get_uid(m[3]),
					(PID)std::stoi(m[1].str())
				}
			)
		};
	}
	
	return std::nullopt;
}

std::optional<Event> LogcatLog::parse_camera_service() {
	std::regex r{"CameraService::connect call \\(PID -?\\d+ \"([^\"]+)\", camera ID \\d+\\).*"};
	std::smatch m{};
	std::regex_match(description, m, r);
	if (m.empty()) return std::nullopt;

	std::println(std::cout, "{}", m[1].str());

	std::string pkg{m[1].str()};
	//std::pair<std::optional<Package>, std::string> cmp{get_package_and_name(m[2].str())};
	
	return Event{
		date,
		time,
		std::make_shared<Proc>( // All I know is the package
			//cmp.first, cmp.second, std::nullopt
			m[1].str(), std::nullopt, std::nullopt
		),
		Event::Relation::ConnectedTo,
		std::make_shared<Component>(
			"Camera"
		)
	};
}

