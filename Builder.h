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
		"digraph G {\n"
		//"\tlayout=fdp\n"
		"\tsubgraph cluster_legend {\n"
			"\t\tlabel=\"Legend\" style=dotted fontsize=9;\n"
			"\t\tlegend_process  [label=\"Activity/Service/Application/Content Provider\" shape=ellipse];\n"
			"\t\tlegend_activity [label=\"Broadcast\" shape=hexagon];\n"
			"\t\tlegend_package  [label=\"Java class\" shape=trapezium];\n"
		"\t}\n"
	};

	inline static const std::string FOOTER{
		"}\n"
	};
};

