#include "RelationGenerator.h"
#include <regex>
#include <set>
#include <ranges>
#include <algorithm>
#include "trim.h"

std::vector<ProcRelation> RelationGenerator::generate_logcat_relations(const std::vector<std::shared_ptr<LogcatLog>>& logs) {
	std::vector<ProcRelation> relations{};
	std::set<std::shared_ptr<Proc>> procs{};

	for (const std::shared_ptr<LogcatLog>& log : logs) {
		if (log->process_name == "ActivityTaskManager") {
			std::regex r{"START u0 \\{.* cmp=(.*)\\} from uid (((u0ai?)?[0-9]+))"};
			std::smatch m{};
			std::regex_match(log->description, m, r);
			if (m.empty()) continue;

			const uint64_t uid{get_uid(m[2].str())};
			const std::shared_ptr<Proc> parent{*procs.insert(std::make_shared<Proc>(std::nullopt, std::nullopt, uid)).first};
			const std::shared_ptr<Proc> child{*procs.insert(std::make_shared<Proc>(m[1].str(), std::nullopt, std::nullopt)).first};

			relations.emplace_back(ProcRelation{
				parent,
				ProcRelation::Relation::Started,
				child
			});
		} else if (log->process_name == "ActivityManager") {
			std::regex r{
				"Start proc ([0-9]{4}):([^/]+)\\/((u0ai?)?[0-9]+) for ((service)|(pre-top-activity)|(top-activity)|(broadcast)|(content provider)|( )|(added application)|(null)) ?(.*?(?=}|$|\n|\r))}?"
			};
			std::smatch m{};
			std::regex_match(log->description, m, r);
			if (m.empty()) continue;

			const uint16_t pid{(uint16_t)std::stoi(m[1].str())};
			const uint64_t uid{get_uid(m[3].str())};
			std::shared_ptr<Proc> parent{nullptr};
			std::shared_ptr<Proc> child{nullptr};

			ProcRelation::Relation rel{};

			auto create_lhs{[&]() {
				return *procs.insert(std::make_shared<Proc>(m[14], std::nullopt, std::nullopt)).first;
			}};
			auto create_rhs{[&]() {
				return *procs.insert(std::make_shared<Proc>(m[2], pid, uid)).first;
			}};
			if (m[5] == "service") {
				auto split{split_activity_string(m[14])};
				if 
				*procs.insert(std::make_shared<Proc>(m[14], std::nullopt, std::nullopt)).first;
				rel = ProcRelation::Relation::NeededToStart;
				child = create_lhs();
			} else if (m[5] == "pre-top-activity") {
				parent = create_lhs();
				rel = ProcRelation::Relation::Hosted;
				child = create_rhs();
			}

			relations.emplace_back(ProcRelation{parent, rel, child});

			//std::println(std::cout, "Found ActivityManager log.");
			if (parent->name == "") std::println(std::cout, "Created parent with empty name.");
		}
	}

	return relations;
}

std::vector<ProcRelation> RelationGenerator::generate_bugreport_relations(const std::vector<std::shared_ptr<Log>>& logs) {
	return generate_logcat_relations(logs | std::views::transform([](const std::shared_ptr<Log>& log) -> std::shared_ptr<LogcatLog> {
		return std::static_pointer_cast<LogcatLog>(log);
	}) | std::ranges::to<std::vector<std::shared_ptr<LogcatLog>>>());
}

static bool is_number(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

uint64_t RelationGenerator::get_uid(const std::string& str) noexcept {
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

std::variant<std::pair<std::string, std::string>, std::string> RelationGenerator::split_activity_string(const std::string& str) noexcept {
	if (size_t i{str.find('/')}; i != std::string::npos) {
		return std::make_pair<std::string, std::string>(
			str.substr(0, i),
			str.substr(i+1)
		);
	} else {
		return str;
	}
}

