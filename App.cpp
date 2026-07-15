#include <print>
#include <fstream>
#include "App.h"
#include "Reader.h"
#include "TimelineGenerator.h"
#include "Builder.h"

App::App(const int argc, char** argv) : app{"HindSight"}, argc{argc}, argv{argv} {
    argv = app.ensure_utf8(argv);

	app.add_option("file", input_file, "Bug report file name")->check(CLI::ExistingFile)->required();
	app.add_option("-o,--out", output_file, "Output file (without extension)");
	app.add_flag("-d,--dot,!--no-dot", save_dot_file, "Save dot file");
	app.add_flag("--cm,--cleaner-map-sensor-ids", cleaner_options.map_sensor_ids, "Map sensor ids to sensor names");
	app.add_flag("!--no-ce,!--no-cleaner-extract-package-names", cleaner_options.extract_package_names, "Find if any node names begin with any known packages and set node package if so");
}

int App::run() {
	CLI11_PARSE(app, argc, argv);

	std::ifstream in{input_file};
	if (!in.is_open()) {
		std::println(std::cerr, "Could not open file.");
		return 1;
	}

	std::vector<std::string> lines{};
	lines.reserve(10000);
	std::string l{};
	while (std::getline(in, l)) {
		lines.emplace_back(l);
	}
	in.close();

	std::vector<std::shared_ptr<Log>> logs{};
	try {
		logs = Reader::read_bugreport(lines);
	} catch (std::exception& e) {
		std::println(std::cerr, "Reader exception: {}", e.what());
		return 1;
	}
	std::println("Reader passed.");

	Timeline timeline{};
	try {
		timeline = TimelineGenerator::generate_timeline(logs);
	} catch (std::exception& e) {
		std::println(std::cerr, "Timeline generator exception: {}", e.what());
		return 1;
	}
	std::println("Timeline generator passed.");

	try {
		Cleaner::clean_relations(timeline, logs, cleaner_options);
	} catch (std::exception& e) {
		std::println(std::cerr, "Cleaner exception: {}", e.what());
		return 1;
	}
	std::println("Cleaner passed.");

	std::string file_name{output_file};
	std::ofstream out{file_name+".dot"};
	try {
		out << Builder::build_graph(timeline);
	} catch (const std::exception& e) {
		std::println(std::cerr, "Builder exception: {}", e.what());
		return 1;
	}
	std::println("Builder passed.");

	system(std::format("dot -Tsvg {}.dot > {}.svg", file_name, file_name, file_name).c_str());
	if (!save_dot_file) {
		system(std::format("rm {}.dot", file_name).c_str());
	} else {
		std::println("Created graph dot file ({}.dot).", file_name);
	}
	std::println("Created graph image ({}.svg).", file_name);

	return 0;
}

