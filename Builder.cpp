#include "Builder.h"
#include "GraphComponent.h"
#include <sstream>
#include <print>
#include <stdexcept>
#include <iostream>

/*std::string Builder::build_graph(const std::vector<ProcRelation>& relations) {
	std::stringstream out_ss{};
	out_ss <<	"digraph G {\n"
				"layout=neato\n"
				"overlap=false;\n"
				"sep=\"+6\";\n";

	for (const ProcRelation& rel : relations) {
		std::println(out_ss, "\t\"{}\" -> \"{}\" [label=\"{}\"];", *rel.lhs, *rel.rhs, rel.relation);
	}
	out_ss << "}\n";

	return out_ss.str();
}*/

std::string Builder::build_graph(Timeline& timeline) {
	std::stringstream out_ss{};
	out_ss << HEADER;

	
	/*const std::map<std::optional<Package>, std::set<Proc*>> pkg_procs{timeline.sort_into_packages()};
	for (const std::pair<std::optional<Package>, std::set<Proc*>>& pkg : pkg_procs) {
		if (pkg.first.has_value()) {
			std::println(out_ss, "\tsubgraph \"cluster_{}\" {{", pkg.first.value());
		}
		for (const Proc* proc : pkg.second) {
			if (proc == nullptr) continue;
			std::optional<std::string> node{get_node_name_until_pid(*proc)};
			if (!node) continue;

			if (pkg.first.has_value()) std::print(out_ss, "\t");
			std::println(out_ss, "\t\"{}\" [shape={}]",
				node.value(),
				type_to_shape.at(proc->type.value_or(Proc::Type::Activity))
			);
		}
		
		if (pkg.first.has_value()) {
			std::println(out_ss, "\t\tlabel=\"{}\"", pkg.first.value());
			std::println(out_ss, "\t}}");
		}
	}*/

	std::vector<std::shared_ptr<Node>> nodes{};
	nodes.reserve(timeline.events.size()*2);
	for (const Event& event : timeline.events) {
		nodes.emplace_back(event.child);
		nodes.emplace_back(event.parent);
	}

	std::println(std::cout, "# nodes: {}", nodes.size());

	for (const std::shared_ptr<Node>& node : nodes) {
		for (const GraphComponent& comp : node->get_graph_components()) {
			if (comp.type == GraphComponent::Type::NodeDefinition) {
				std::println(out_ss, "{}", comp.text);
			}
		}
	}

	std::println(std::cout, "# events: {}", timeline.events.size());

	for (const Event& event : timeline.events) {
		std::println(out_ss, "{}", event.get_graph_component().text);
	}

	for (const std::shared_ptr<Node>& node : nodes) {
		for (const GraphComponent& comp : node->get_graph_components()) {
			if (comp.type == GraphComponent::Type::UIDRelation) {
				std::println(out_ss, "{}", comp.text);
			}
		}
	}

	/*std::println(out_ss, "\tsubgraph uids {{")
	//std::println(out_ss, "\t\tedge [dir=none]");
	std::println(out_ss, "\t\tedge [color=grey50, style=dashed]");
	std::println(out_ss, "\tnode [shape=diamond]");
	const std::map<std::optional<UID>, std::set<Proc*>> uid_procs{timeline.sort_into_uid()};
	for (const std::pair<std::optional<UID>, std::set<Proc*>>& uid : uid_procs) {
		if (uid.second.size() == 1 || !uid.first.has_value()) continue;
		for (const Proc* proc : uid.second) {
			std::optional<std::string> node{get_node_name_until_pid(*proc)};
			if (!node) continue;

			std::println(out_ss, "\t\t\"{}\" -> \"UID {}\" [label=\"UID belongs to\"]",
				node.value(), uid.first.value()
			);
		}
	}
	std::println(out_ss, "\t}}");*/
	
	out_ss << FOOTER;
	return out_ss.str();
}

