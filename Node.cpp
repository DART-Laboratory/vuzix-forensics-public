#include "Node.h"
#include "GraphComponent.h"

std::optional<std::string> Proc::get_node_name() const noexcept {
	if (name.has_value()) {
		return name.value();
	} else if (ids.has_value()) {
		if (ids.value().pid.has_value()) {
			return "PID " + std::to_string(ids.value().pid.value());
		} else {
			return "UID " + std::to_string(ids.value().uid);
		}
	} else if (package.has_value()) {
		return "cluster_"+package.value();
	} else {
		return std::nullopt;
	}
}

std::optional<std::string> Proc::get_node_name_until_pid() const noexcept {
	if (name.has_value()) {
		return name.value();
	} else if (ids.has_value() && ids.value().pid.has_value()) {
		return std::to_string(ids.value().pid.value());
	} else {
		return std::nullopt;
	}
}

std::vector<GraphComponent> Proc::get_graph_components() const {
	std::vector<GraphComponent> ret{};

	//std::optional<std::string> node_name{get_node_name()};
	std::optional<std::string> node_name{get_node_name_until_pid()};
	if (!node_name) return ret;

	if (ids.has_value()) {
		ret.emplace_back(std::format(
			"\t\t\"{}\" -> \"UID {}\" [label=\"UID belongs to\"]",
			node_name.value(), ids.value().uid
		));
	}

	std::string def_str{};
	if (package.has_value()) {
		def_str = std::format(
			"\tsubgraph \"cluster_{}\" {{ label=\"{}\"",
			package.value(), package.value()
		);
	}

	def_str += std::format("\"{}\" [shape=ellipse] {}",
		node_name.value(),
		(package.has_value() ? "}" : "")
	);

	ret.emplace_back(def_str, GraphComponent::Type::NodeDefinition);
	return ret;
}

std::optional<std::string> Sensor::get_node_name() const noexcept {
	return name;
}

std::vector<GraphComponent> Sensor::get_graph_components() const { return {}; }

std::optional<std::string> JavaClass::get_node_name() const noexcept {
	return name;
}

std::vector<GraphComponent> JavaClass::get_graph_components() const { return {}; }

std::optional<std::string> Broadcast::get_node_name() const noexcept {
	return name;
}

std::vector<GraphComponent> Broadcast::get_graph_components() const { return {}; }

GraphComponent Event::get_graph_component() const {
	if (child == nullptr) throw std::runtime_error{"Event child is nullptr."};
	
	if (parent != nullptr) {
		std::optional<std::string> parent_name{parent->get_node_name()};
		std::optional<std::string> child_name{child->get_node_name()};
		if (!parent || !child) throw std::runtime_error{"Cannot get node names"};
		return GraphComponent{
			std::format(
				"\t\"{}\" -> \"{}\" [label=\"{}\"]",
				parent_name.value(),
				child_name.value(),
				relation_to_str.at(relation)
			),
			GraphComponent::Type::EventRelation
		};
	} else {
		std::optional<std::string> child_name{child->get_node_name()};
		if (!child) throw std::runtime_error{"Cannot get child node name in non-parented event"};
		return GraphComponent{
			std::format(
				"\t\"{}\" [label=\"{}\"]",
				child_name.value(),
				relation_to_str.at(relation)
			),
			GraphComponent::Type::EventRelation
		};
	}
}

