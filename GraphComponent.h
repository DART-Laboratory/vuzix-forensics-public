#pragma once

#include <string>

class GraphComponent {
public:
	std::string text;
	enum class Type {
		UIDRelation,
		EventRelation,
		NodeDefinition
	} type;
};

