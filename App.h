#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include "Cleaner.h"

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
};

