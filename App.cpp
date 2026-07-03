#include <print>
#include <fstream>
#include "App.h"
#include "Reader.h"
#include "TimelineGenerator.h"
#include "Builder.h"

App::App(const int argc, char** argv) : app{"Log parser"}, argc{argc}, argv{argv} {
    argv = app.ensure_utf8(argv);

	app.add_option("file", input_file, "Bug report file name")->check(CLI::ExistingFile)->required();
	app.add_option("-o,--out", output_file, "Output file (without extension)");
	app.add_flag("-d,--dot,!--no-dot", save_dot_file, "Save dot file");
	app.add_flag("--cm,--cleaner-merge", cleaner_options.merge_procs_starting_with_dot, "Merge processes starting with dot in its name");
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

	std::vector<std::shared_ptr<LogcatLog>> logs{};
	try {
		logs = Reader::read_logcat(lines);
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
		Cleaner::clean_relations(timeline, cleaner_options);
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
	}

	system(std::format("dot -Tsvg {}.dot > {}.svg", file_name, file_name, file_name).c_str());
	if (!save_dot_file) {
		system(std::format("rm {}.dot", file_name).c_str());
	} else {
		std::println("Created graph dot file ({}.dot).", file_name);
	}
	std::println("Created graph image ({}.svg).", file_name);

	return 0;
}

