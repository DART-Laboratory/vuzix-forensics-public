#pragma once

#include "TimelineGenerator.h"
#include <map>

class Builder {
public:
	Builder() = delete;

	//static std::string build_graph(const std::vector<ProcRelation>& relations);
	static std::string build_graph(const Timeline& timeline);
	
private:
	static std::map<std::optional<Package>, std::vector<Proc>> sort_into_packages(const Timeline& timeline);

	inline static const std::string HEADER{
		"digraph G {\n"
		"layout=fdp\n"
	};

	inline static const std::string FOOTER{
		"}\n"
	};

	inline static const std::string NO_PACKAGE{"No package"};
	inline static const std::string NO_NAME{"No name"};
};

