#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

class App {
public:
	App(const int argc, char** argv);

	int run();

private:
	CLI::App app;
	const int argc;
	char** argv;

	std::string input_file;
	std::optional<std::string> output_file;
};

