#include "Node.h"
#include "CLI/CLI.hpp"
#include "GraphComponent.h"

std::optional<std::string> Node::get_node_name() const noexcept {
	return name;
}

std::vector<GraphComponent> Node::get_graph_components() const {
	std::optional<std::string> node_name{get_node_name()};
	if (!node_name) return {};
	return {get_node_def_graph_component(node_name.value())};
}

std::string Node::package_cluster(const std::optional<Package>& package, const std::string& cluster_contents) {
	if (package.has_value()) {
		return std::format(
			"\tsubgraph \"cluster_{}\" {{ label=\"PACKAGE {}\" {} }}",
			package.value(), package.value(), cluster_contents
		);
	} else {
		return '\t' + cluster_contents;
	}
}

GraphComponent Node::get_node_def_graph_component(const std::string& node_name) const {
	std::stringstream color_ss;
	color_ss << std::hex << get_color();


	return GraphComponent{
		package_cluster(package, std::format("\"{}\" [shape={} fillcolor=\"{}\"]",
			node_name,
			shape_to_str.at(get_shape()),
			"#" + color_ss.str()
		)),
		GraphComponent::Type::NodeDefinition
	};
}

void Node::prefix_package_on_name() noexcept {
	if (!name || !package) return;
	if (!name->starts_with(package.value())) {
		if (!name.value().starts_with('.') && !package->ends_with('.')) {
			name->insert(name->begin(), '.');
		}
		name = package.value() + name.value();
	}
}

bool Node::similar(const std::shared_ptr<Node>& node) const {
	return (name.has_value() && name == node->name);
}

void Node::relate(const std::shared_ptr<Node>& node) {
	auto relate_one_way{[](const Node& in, Node& out) {
		if (in.name) out.name = in.name;
		if (in.package) out.package = in.package;
	}};

	relate_one_way(*this, *node);
	relate_one_way(*node, *this);
}

// Order of importance:
// (NAME) PID (PID)
// (NAME)
// PID (PID)
// UID (UID)
// (PACKAGE)
// std::nullopt
std::optional<std::string> Proc::get_node_name() const noexcept {
	if (name) {
		if (ids && ids->pid) {
			return name.value() + " PID " + std::to_string(ids->pid.value());
		} else {
			return name.value();
		}
	} else if (ids) {
		if (ids->pid) {
			return "PID " + std::to_string(ids->pid.value());
		} else {
			return "UID " + std::to_string(ids->uid);
		}
	} else if (package) {
		return dummy_package_node_prefix+package.value();
	}

	return std::nullopt;
}

// Order of importance:
// (NAME) PID (PID)
// (NAME)
// PID (PID)
// std::nullopt
std::optional<std::string> Proc::get_node_name_until_pid() const noexcept {
	if (name) {
		if (ids && ids->pid) {
			return name.value() + " PID " + std::to_string(ids->pid.value());
		} else {
			return name.value();
		}
	} else if (ids) {
		if (ids->pid) {
			return "PID " + std::to_string(ids->pid.value());
		}
	}

	return std::nullopt;
}

std::vector<GraphComponent> Proc::get_graph_components() const {
	std::vector<GraphComponent> ret{};

	//std::optional<std::string> node_name{get_node_name()};
	std::optional<std::string> node_name{get_node_name_until_pid()};
	if (!node_name) return ret;

	ret.emplace_back(get_node_def_graph_component(node_name.value()));
	if (ids.has_value()) {
		ret.emplace_back(std::format(
			"\t\t\"{}\" -> \"UID {}\" [label=\"UID belongs to\"]",
			node_name.value(), ids.value().uid
		), GraphComponent::Type::UIDRelation);
	}

	return ret;
}

template <typename T1, typename T2> requires std::equality_comparable_with<T1, T2>
static int optional_value_compare(const std::optional<T1>& lhs, const std::optional<T2>& rhs) {
	bool lhs_has_value{lhs.has_value()};
	bool rhs_has_value{rhs.has_value()};
	if (!lhs_has_value) return 0;
	if (!rhs_has_value) return 1;
	if (*lhs == *rhs) return 2;
	else return 3;
}

bool Proc::similar(const std::shared_ptr<Node>& node) const {
	if (auto proc{std::dynamic_pointer_cast<Proc>(node)}) {
		if (ids && proc->ids) {
			int pid_comp{optional_value_compare(ids->pid, proc->ids->pid)};

			if (pid_comp == 3) return false;
			if (ids->uid != proc->ids->uid) return false;

			if (pid_comp == 2) return true;
		}

		int name_comp{optional_value_compare(name, proc->name)};
		if (name_comp == 3) return false;
		if (name_comp == 2) return true;

		int pkg_comp{optional_value_compare(package, proc->package)};
		if (pkg_comp == 2) return true;
	}

	return false;
}

void Proc::relate(const std::shared_ptr<Node>& node) {
	auto proc{std::static_pointer_cast<Proc>(node)};

	Node::relate(node);
	if (proc->ids) ids = proc->ids;
	if (ids) proc->ids = ids;
}

bool Sensor::similar(const std::shared_ptr<Node>& node) const {
	if (!Node::similar(node)) return false;
	if (auto sensor{std::dynamic_pointer_cast<Sensor>(node)}) {
		if (id == sensor->id) return true;
	}

	return false;
}

GraphComponent Event::get_graph_component() const {
	if (child == nullptr) throw std::runtime_error{"Event child is nullptr."};
	
	if (parent != nullptr) {
		std::optional<std::string> parent_name{parent->get_node_name()};
		std::optional<std::string> child_name{child->get_node_name()};
		if (!parent_name || !child_name) throw std::runtime_error{"Cannot get node names"};

		return GraphComponent{
			std::format(
				"\t\"{}\" -> \"{}\" [label=\"{}\"{}]",
				parent_name.value(),
				child_name.value(),
				relation_to_str.at(relation),
				(parent_name->starts_with(Node::dummy_package_node_prefix) ?
					" ltail=\"cluster_" + parent->package.value() + '\"' : ""
				)
			),
			GraphComponent::Type::EventRelation
		};
	} else {
		std::optional<std::string> child_name{child->get_node_name()};
		if (!child) throw std::runtime_error{"Cannot get child node name in non-parented event"};

		return GraphComponent{
			std::format(
				"\t\"{}\"",
				child_name.value()
			),
			GraphComponent::Type::EventRelation
		};
	}
}

bool Event::similar(const Event& event) const noexcept {
	return (parent == event.parent && child == event.child && relation == event.relation);
}

