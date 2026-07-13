#include "Builder.h"
#include "GraphComponent.h"
#include <sstream>
#include <print>
#include <iostream>

std::string Builder::build_graph(Timeline& timeline) {
	std::stringstream out_ss{};
	out_ss << HEADER;

	std::vector<std::shared_ptr<Node>> nodes{};
	nodes.reserve(timeline.events.size()*2);
	for (const Event& event : timeline.events) {
		nodes.emplace_back(event.child);
		nodes.emplace_back(event.parent);
	}

	for (const std::shared_ptr<Node>& node : nodes) {
		for (const GraphComponent& comp : node->get_graph_components()) {
			if (comp.type == GraphComponent::Type::NodeDefinition) {
				std::println(out_ss, "{}", comp.text);
			}
		}
	}
	std::println(std::cout, "Builder: Node graph components building passed");

	for (const Event& event : timeline.events) {
		std::println(out_ss, "{}", event.get_graph_component().text);
	}
	std::println(std::cout, "Builder: Event graph components building passed");

	for (const std::shared_ptr<Node>& node : nodes) {
		for (const GraphComponent& comp : node->get_graph_components()) {
			if (comp.type == GraphComponent::Type::UIDRelation) {
				std::println(out_ss, "{}", comp.text);
			}
		}
	}
	std::println(std::cout, "Builder: UID graph components building passed");
	
	out_ss << FOOTER;
	return out_ss.str();
}

