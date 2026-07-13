#pragma once

#include "TimelineGenerator.h"
#include <map>

class Builder {
public:
	Builder() = delete;

	//static std::string build_graph(const std::vector<ProcRelation>& relations);
	static std::string build_graph(Timeline& timeline);
	
private:
	static std::optional<std::string> get_node_name(const Proc& proc) noexcept;
	static std::optional<std::string> get_node_name_until_pid(const Proc& proc) noexcept;

	inline static const std::string HEADER{
		"digraph Provenance {\n"
		//"\tlayout=fdp\n"
		"\tgraph [ranksep=\"2\"]\n"
		"\tnode [style=\"filled\" fillcolor=\"#fef9e7\"]\n"
		"\tsubgraph cluster_legend {\n"
			"\t\tlabel=\"Legend\" style=dotted fontsize=9;\n"
			"\t\tlegend_process  [label=\"Process/Activity/Service/Application/Content Provider/Unknown\" shape=ellipse fillcolor=\"#d6eaf8\"];\n"
			"\t\tlegend_package  [label=\"Sensor\" shape=box fillcolor=\"#fdebd0\"];\n"
			"\t\tlegend_activity [label=\"Broadcast/Broadcast receiver\" shape=hexagon fillcolor=\"#fadbd8\"];\n"
			"\t\tlegend_package  [label=\"Java class\" shape=trapezium fillcolor=\"#e8daef\"];\n"
		"\t}\n"
	};

	inline static const std::string FOOTER{
		"}\n"
	};
};

