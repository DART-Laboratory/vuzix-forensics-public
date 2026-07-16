#include "CLI/App.hpp"
#include "Cleaner.h"
#include "Statistics.h"

class App {
public:
	App(const int argc, char** argv);

	int run();

private:
	CLI::App app;
	const int argc;
	char** argv;

	std::string input_file;
	std::string output_file{"graph"};
	bool save_dot_file{false};
	CleanerOptions cleaner_options{};
	Statistics stats{};
	bool print_stats{false};
};

