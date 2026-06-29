#include <print>
#include <fstream>
#include "App.h"
#include "Reader.h"
#include "TimelineGenerator.h"
#include "Builder.h"

App::App(const int argc, char** argv) : app{"Log parser"}, argc{argc}, argv{argv} {
    argv = app.ensure_utf8(argv);

	app.add_option("file", input_file, "Bug report file name (without extension)")->check(CLI::ExistingFile)->required();
	app.add_option("-o,--out", output_file, "Output file");
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

	/*std::vector<ProcRelation> relations{};
	try {
		relations = RelationGenerator::generate_bugreport_relations(logs);
	} catch (std::exception& e) {
		std::println(std::cerr, "Relation generator exception: {}", e.what());
		return 1;
	}
	std::println("Relation generator passed.");

	try {
		relations = Cleaner::clean_relations(relations);
	} catch (std::exception& e) {
		std::println(std::cerr, "Cleaner exception: {}", e.what());
		return 1;
	}
	std::println("Cleaner passed.");*/

	std::string file_name{output_file.value_or("graph")};
	std::ofstream out{file_name+".dot"};
	out << Builder::build_graph(timeline);
	system(std::format("dot -Tsvg {}.dot > {}.svg"/* && rm {}.dot"*/, file_name, file_name, file_name).c_str());
	std::println("Created graph image (graph.svg).\n");

	return 0;
}

