#include "Builder.h"
#include "GraphComponent.h"
#include <sstream>
#include <print>

std::string Builder::build_graph(Timeline& timeline, Statistics& stats) {
	std::stringstream out_ss{};
	out_ss << HEADER;

	std::set<std::shared_ptr<Node>> nodes{};
	for (const Event& event : timeline.events) {
		if (event.child != nullptr) nodes.insert(event.child);
		if (event.parent != nullptr) nodes.insert(event.parent);
	}

	for (const std::shared_ptr<Node>& node : nodes) {
		stats.missing_items += node->get_missing_items();
		stats.total_possible_items += node->get_total_possible_items();
	}

	for (const std::shared_ptr<Node>& node : nodes) {
		if (node->package) {
			std::println(out_ss, "{}", Node::package_cluster(
				node->package,
				std::format("\"{}\" [shape=point style=invis]",
					Node::dummy_package_node_prefix+node->package.value()
				)
			));
		}
	}

	for (const std::shared_ptr<Node>& node : nodes) {
		for (const GraphComponent& comp : node->get_graph_components()) {
			if (comp.type == GraphComponent::Type::NodeDefinition) {
				std::println(out_ss, "{}", comp.text);
				stats.num_of_nodes++;
			}
		}
	}
	std::println("Builder: Node graph components building passed");

	for (const Event& event : timeline.events) {
		std::println(out_ss, "{}", event.get_graph_component().text);
		stats.num_of_edges++;
	}
	std::println("Builder: Event graph components building passed");

	for (const std::shared_ptr<Node>& node : nodes) {
		for (const GraphComponent& comp : node->get_graph_components()) {
			if (comp.type == GraphComponent::Type::UIDRelation) {
				std::println(out_ss, "{}", comp.text);
				stats.num_of_edges++;
			}
		}
	}
	std::println("Builder: UID graph components building passed");
	
	out_ss << FOOTER;
	return out_ss.str();
}

