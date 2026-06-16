#include <iostream>
#include <fstream>
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include <optional>
#include <regex>
#include <ranges>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <exception>
#include <sstream>
#include <eigen3/Eigen/Eigen>
#include "trim.h"

void parse_systrace(std::ifstream& in) {
	std::map<std::string, size_t> process_count{};

	std::cout << "Counting process occurances...\n";

	std::string l{};
	while (std::getline(in, l)) {
		if (l[0] == '#') continue;
		if (l.find("<idle>") != std::string::npos) continue;

		const std::regex r{"^ *[^-]+"};
		std::smatch m{};
		std::regex_search(l, m, r);
		if (!m.empty()) {
			process_count[trim_copy(m.str())]++;
		}
	}

	{
		std::cout << "Process counts:\n";
		const size_t largest_name{
			std::ranges::max(
				(process_count | std::views::keys),
				{}, &std::string::size
			).size()
		};
		for (const std::pair<std::string, size_t> p : process_count) {
			std::cout << '\t' << std::setw(largest_name) << p.first << ": " << p.second << std::endl;
		}
	}
}

struct Log {
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
		Log::Type::Fatal,	
		Log::Type::Error,
		Log::Type::Warning,
		Log::Type::Info,
		Log::Type::Debug,
		Log::Type::Verbose,
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

void build_graph(const std::vector<Log>& logs) {
	std::ofstream out{"graph.dot"};

	out <<	"digraph G {\n"
			"layout=neato\n"
    		"overlap=false;\n"
    		"sep=\"+6\";\n";

	auto build_relation = [&](const std::string& lhs, const std::string& relation, const std::string& rhs) {
		out << "\t\"" << lhs << "\" -> \"" << rhs << "\"";
		if (!relation.empty()) {
			out << " [label=\"" << relation << "\"];\n";
		} else {
			out << ";\n";
		}
	};

	for (const Log& log : logs) {
		if (log.process_name == "ActivityManager") {
			std::regex r{"Start proc ([0-9]+):([^/]+)"};
			std::smatch m{};
			std::regex_search(log.description, m, r);
			if (!m.empty()) {
				build_relation("ActivityManager", "started", (m.begin()+2)->str());
				continue;
			}

			r = std::regex{"Process ([^ ]+) \\(pid ([0-9]{4})\\) has died"};
			std::regex_search(log.description, m, r);
			if (!m.empty()) {
				continue;
			}

			r = std::regex{"Scheduling restart of crashed service ([^/]+)"};
			std::regex_search(log.description, m, r);
			if (!m.empty()) {
				build_relation("ActivityManager", "scheduled restart of", (m.begin()+1)->str());
				continue;
			}

			r = std::regex{"Killing ([0-9]{4}):([^/]+)"};
			std::regex_search(log.description, m, r);
			if (!m.empty()) {
				build_relation("ActivityManager", "killed", (m.begin()+2)->str());
				continue;
			}
		} else if (log.process_name == "Zygote") {
			std::regex r{"Forked child process ([0-9]{4})"};
			std::smatch m{};
			std::regex_match(log.description, m, r);
			if (!m.empty()) {
				//build_relation("Zygote", "forked", (m.begin()+1)->str());
				continue;
			}

			r = std::regex{"Process ([0-9]{4}) exited due to signal ([1-9]+ \\(\\w+\\))"};
			std::regex_search(log.description, m, r);
			if (!m.empty()) {
				continue;
			}

			r = std::regex{"Process ([0-9]{4}) exited cleanly (0)"};
			std::regex_search(log.description, m, r);
			if (!m.empty()) {
				continue;
			}

		} else if (log.process_name == "ActivityTaskManager") {
			std::regex r{"cmp=([^/]+)"};
			std::smatch m{};
			std::regex_search(log.description, m, r);
			if (!m.empty()) {
				//build_relation("ActivityTaskManager", "started", (m.begin()+1)->str());
				continue;
			}
		}
	}

	out << "}\n";

	out.close();

	system("dot -Tsvg graph.dot > graph.svg && rm graph.dot");
	std::cout << "Created graph image (graph.svg).\n";
}

void parse_logcat(std::ifstream& in) {
	std::cout << "Finding process relationships...\n";

	std::vector<Log> logs{};
	logs.reserve(10000);

	std::string l{};
	size_t i{1};
	while (std::getline(in, l)) {
		Log log{};

		const std::regex r{
			"^([0-9]{2}-[0-9]{2}) +([0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3})( +[0-9]{4}){2} +(F|E|W|I|D|V) (.*?(?= *:+ )) *:+ (.*)"
		};

		std::smatch m{};
		std::regex_match(l, m, r);

		if (m.empty()) continue;

		std::istringstream ss{(m.begin()+1)->str()};
		ss >> std::chrono::parse("%m-%d", log.date);

		std::chrono::milliseconds ms{};
		ss = std::istringstream{(m.begin()+2)->str()};
		ss >> std::chrono::parse("%T", ms);
		log.time = std::chrono::time_point<std::chrono::system_clock>{ms};

		log.process_name = (m.begin()+5)->str();
		log.description = (m.begin()+6)->str();

		log.type = Log::log_type_enums[std::distance(
			Log::log_type_chars.begin(),
			std::ranges::find(Log::log_type_chars, (m.begin()+4)->str()[0])
		)];

		logs.emplace_back(log);
	}

	build_graph(logs);
}

void parse_sensor(std::ifstream& in) {

}

int main(int argc, char* argv[]) {
	std::ifstream systrace{};
	std::ifstream logcat{};
	std::ifstream sensor{};

	CLI::App app{"Log parser"};
    argv = app.ensure_utf8(argv);

	std::optional<std::string> systrace_name{};
	app.add_option("-t,--trace", systrace_name, "Systrace file name")->check(CLI::ExistingFile);

	std::optional<std::string> logcat_name{};
	app.add_option("-l,--logcat", logcat_name, "Logcat file name")->check(CLI::ExistingFile);

	std::optional<std::string> sensor_name{};
	app.add_option("-s,--sensor", sensor_name, "Sensor log file name")->check(CLI::ExistingFile);

	/*try {
		app.parse(argc, argv);
	} catch (const CLI::ParseError& err) {
		std::cerr << err.what() << std::endl;
		return 1;
	}*/
	
	CLI11_PARSE(app, argc, argv);

	if (!systrace_name && !logcat_name && !sensor_name) {
		std::cerr << "Must specify at least one of the following: systrace file, logcat file, sensor file.\n";
		return 1;
	}

	if (systrace_name) {
		try {
			systrace.open(systrace_name.value());
			parse_systrace(systrace);
		} catch (const std::exception& e) {
			std::cerr << "systrace parsing failed: " << e.what() << std::endl;
		}
	}
	if (logcat_name) {
		try {
			logcat.open(logcat_name.value());
			parse_logcat(logcat);
		} catch (const std::exception& e) {
			std::cerr << "logcat parsing failed: " << e.what() << std::endl;
		}
	}
	if (sensor_name) {
		try {
			sensor.open(sensor_name.value());
			parse_sensor(sensor);
		} catch (const std::exception& e) {
			std::cerr << "sensor parsing failed: " << e.what() << std::endl;
		}
	}

	return 0;
}

