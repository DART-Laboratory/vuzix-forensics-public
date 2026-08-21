#include "Statistics.h"
#include <filesystem>
#include <print>

void Statistics::print() const noexcept {
	std::println(
		"Run time:                            {}\n"
		"# nodes:                             {}\n"
		"# edges:                             {}\n"
		"# graph components (nodes+edges):    {}\n"
		"Bugreport file size:                 {}KB\n"
		"Bugreport file size/node:            {:.1f} Bytes/node\n"
		"Bugreport file size/graph component: {:.1f} Bytes/graph component\n"
		"Missing items:                       {}/{} ({:.2f}%)",
		timer.run_time, num_of_nodes, num_of_edges,
		num_of_nodes+num_of_edges,
		bugreport_size, bugreport_size_per_node,
		bugreport_size_per_graph_component,
		missing_items, total_possible_items,
		(double)missing_items/total_possible_items*100
	);
}

void Statistics::time_start() {
	timer.start_time = std::chrono::system_clock::now();
}

void Statistics::time_end() {
	timer.end_time = std::chrono::system_clock::now();
	timer.run_time = std::chrono::duration_cast<std::chrono::milliseconds>(timer.end_time - timer.start_time);
}

void Statistics::calculate_graph_stats(const std::string& input_file) {
	bugreport_size = std::filesystem::file_size(input_file);
	bugreport_size_per_node = bugreport_size/(double)num_of_nodes;
	bugreport_size_per_graph_component = bugreport_size/(double)(num_of_nodes+num_of_edges);
}

