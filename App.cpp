#include <print>
#include "App.h"
#include "Reader.h"
#include "Analyzer.h"

App::App(const int argc, char** argv) : app{"Log parser"}, argc{argc}, argv{argv} {
    argv = app.ensure_utf8(argv);

	app.add_option("file", input_file, "Bug report file")->check(CLI::ExistingFile)->required();
	app.add_option("-o,--out", output_file, "Output file");
}

int App::run() {
	CLI11_PARSE(app, argc, argv);


	std::ifstream in{input_file};
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
	}
	std::println("Reader passed.");

	std::vector<ProcRelation> relations{};
	try {
		relations = Analyzer::analyze_bugreport(logs);
	} catch (std::exception& e) {
		std::println(std::cerr, "Analyzer exception: {}", e.what());
	}
	std::println("Analyzer passed.");

	for (const ProcRelation& relation : relations) {
		std::println("{}", relation);
	}

	return 0;
}

