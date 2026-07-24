#pragma once

#include <chrono>

struct Statistics {
	struct Timer {
		std::chrono::time_point<std::chrono::system_clock> start_time;
		std::chrono::milliseconds run_time;
		std::chrono::time_point<std::chrono::system_clock> end_time;
	} timer;
	size_t num_of_nodes;
	size_t num_of_edges;
	uintmax_t bugreport_size;
	double bugreport_size_per_node;
	double bugreport_size_per_graph_component;
	size_t missing_items;
	size_t total_possible_items;
	
	void print() const noexcept;

	void time_start();
	void time_end();

	void calculate_graph_stats(const std::string& input_file);
};

