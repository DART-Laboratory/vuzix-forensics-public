#pragma once

#include "TimelineGenerator.h"
#include <map>
#include <functional>
#include <set>

class Builder {
public:
	Builder() = delete;

	//static std::string build_graph(const std::vector<ProcRelation>& relations);
	static std::string build_graph(const Timeline& timeline);
	
private:
	static std::optional<std::string> get_node_name(const Proc& proc) noexcept;
	static std::optional<std::string> get_node_name_until_pid(const Proc& proc) noexcept;

	inline static const std::string HEADER{
		"digraph G {\n"
		//"\tlayout=fdp\n"
		"\tsubgraph cluster_legend {"
			"\t\tlabel=\"Legend\" style=dotted fontsize=9;"
			"\t\tlegend_process  [label=\"Activity/Service/Application/Content Provider\" shape=ellipse];"
			"\t\tlegend_activity [label=\"Broadcast\" shape=hexagon];"
			"\t\tlegend_package  [label=\"Java class\" shape=trapezium];"
		"\t}"
	};

	inline static const std::string FOOTER{
		"}\n"
	};

	inline static const std::map<Proc::Type, std::string> type_to_shape{
		{Proc::Type::Activity, "ellipse"},
		{Proc::Type::Service, "ellipse"},
		{Proc::Type::Broadcast, "hexagon"},
		{Proc::Type::Application, "ellipse"},
		{Proc::Type::ContentProvider, "ellipse"},
		{Proc::Type::JavaClass, "trapezium"},
	};
};

