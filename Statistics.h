#pragma once

#include <chrono>
#include <print>

struct Statistics {
	std::chrono::milliseconds run_time{};
	size_t num_of_nodes{};
	size_t num_of_edges{};
	uintmax_t bugreport_size{};
	double bugreport_size_per_node{};
	double bugreport_size_per_graph_component{};
	
	void print() const noexcept {
		std::println(
			"Run time:                            {}\n"
			"# nodes:                             {}\n"
			"# edges:                             {}\n"
			"# graph components (nodes+edges):    {}\n"
			"Bugreport file size:                 {}KB\n"
			"Bugreport file size/node:            {:.1f}KB/node\n"
			"Bugreport file size/graph component: {:.1f}KB/graph component\n",
			run_time, num_of_nodes, num_of_edges,
			num_of_nodes+num_of_edges,
			bugreport_size, bugreport_size_per_node,
			bugreport_size_per_graph_component
		);
	}
};

