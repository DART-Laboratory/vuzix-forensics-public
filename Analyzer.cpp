#include "Analyzer.h"
#include <regex>
#include <set>
#include <ranges>
#include <algorithm>
#include "trim.h"

template <typename T, typename proj>
static std::optional<Proc> find_proc_by(const std::vector<Proc>& procs, const T& var, const proj& func) {
	auto it{std::ranges::find(procs, var, &func)};
	if (it == procs.end()) return std::nullopt;
	else return *it;
}

std::vector<ProcRelation> Analyzer::analyze_logcat(const std::vector<std::shared_ptr<LogcatLog>>& logs) {
	std::vector<ProcRelation> relations{};
	std::set<std::unique_ptr<Proc>> procs{};

	for (const std::shared_ptr<LogcatLog>& log : logs) {
		if (log->process_name == "ActivityTaskManager") {
			std::regex r{"START u0 \\{.* cmp=(.*)\\} from uid (((u0ai?)?[0-9]+))"};
			std::smatch m{};
			std::regex_match(log->description, m, r);
			if (m.empty()) continue;

			Proc* parent{procs.insert(std::make_unique<Proc>(std::nullopt, std::nullopt, get_uid(m[2]))).first->get()};
			Proc* child{procs.insert(std::make_unique<Proc>(m[1].str(), std::nullopt, std::nullopt)).first->get()};

			relations.emplace_back(ProcRelation{
				*parent,
				ProcRelation::Relation::Started,
				*child
			});

			std::println(std::cout, "Found ActivityTaskManager log.");
		} else if (log->process_name == "ActivityManager") {
			std::regex r{"Start proc ([0-9]{4}):([^/]+)\\/((u0ai?)?[0-9]+) for[^\\{]+\\{(.*)}"};
			std::smatch m{};
			std::regex_search(log->description, m, r);
			if (m.empty()) continue;

			uint16_t pid{(uint16_t)std::stoi(m[1].str())};
			uint64_t uid{get_uid(m[3].str())};
			Proc* child{procs.insert(std::make_unique<Proc>(m[2], pid, uid)).first->get()};
			Proc* parent{procs.insert(std::make_unique<Proc>(m[5], std::nullopt, std::nullopt)).first->get()};

			relations.emplace_back(ProcRelation{
				*parent,
				ProcRelation::Relation::Started,
				*child
			});

			std::println(std::cout, "Found ActivityManager log.");
		}
	}

	return relations;
}

std::vector<ProcRelation> Analyzer::analyze_bugreport(const std::vector<std::shared_ptr<Log>>& logs) {
	return analyze_logcat(logs | std::views::transform([](const std::shared_ptr<Log>& log) -> std::shared_ptr<LogcatLog> {
		return std::static_pointer_cast<LogcatLog>(log);
	}) | std::ranges::to<std::vector<std::shared_ptr<LogcatLog>>>());
}

static bool is_number(const std::string& s) {
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

uint64_t Analyzer::get_uid(const std::string& str) noexcept {
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

