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
		"\tgraph [nodesep=\"1\" ranksep=\"2\"]\n"
		"\tnode [style=\"filled\" fillcolor=\"#fef9e7\"]\n"
		"\tcompound=true\n"
		"\tsubgraph cluster_legend {\n"
			"\t\tlabel=\"Legend\" style=dotted fontsize=9;\n"
			"\t\tlegend_process  [label=\"Process/Content Provider/Unknown\" shape=ellipse fillcolor=\"#d6eaf8\"];\n"
			"\t\tlegend_service  [label=\"Service\" shape=octagon fillcolor=\"#abebc6\"];\n"
			"\t\tlegend_activity  [label=\"Activity\" shape=ellipse fillcolor=\"#d5f5e3\"];\n"
			"\t\tlegend_sensor  [label=\"Sensor\" shape=box fillcolor=\"#fdebd0\"];\n"
			"\t\tlegend_component  [label=\"Component (like camera)\" shape=box fillcolor=\"#f4d4ed\"];\n"
			"\t\tlegend_broadcast [label=\"Broadcast receiver\" shape=hexagon fillcolor=\"#fadbd8\"];\n"
			"\t\tlegend_class  [label=\"Java class\" shape=octagon fillcolor=\"#e8daef\"];\n"
		"\t}\n"
	};

	inline static const std::string FOOTER{
		"}\n"
	};
};

