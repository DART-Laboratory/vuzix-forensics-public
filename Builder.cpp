#include "Builder.h"
#include <sstream>
#include <print>

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

std::string Builder::build_graph(const Timeline& timeline) {
	std::stringstream out_ss{};
	out_ss << HEADER;

	const std::map<std::optional<Package>, std::set<Proc>> pkg_procs{sort_into_packages(timeline)};
	const std::map<std::optional<UID>, std::set<Proc>> uid_procs{sort_into_uid(timeline)};
	//std::vector<const Proc*> procs{};
	for (const std::pair<std::optional<Package>, std::set<Proc>>& pkg : pkg_procs) {
		if (pkg.first.has_value()) {
			std::println(out_ss, "\tsubgraph \"cluster_{}\" {{", pkg.first.value());
		}
		for (const Proc& proc : pkg.second) {
			//procs.emplace_back(&proc);
			std::optional<std::string> node{get_node_name_until_pid(proc)};
			if (!node) continue;

			if (pkg.first.has_value()) std::print(out_ss, "\t");
			std::println(out_ss, "\t\"{}\"", node.value());
		}
		
		if (pkg.first.has_value()) {
			std::println(out_ss, "\t\tlabel=\"{}\"", pkg.first.value());
			std::println(out_ss, "\t}}");
		}
	}

	for (const std::unique_ptr<Event>& event : timeline.events) {
		std::optional<std::string> parent{get_node_name(event->parent)};
		std::optional<std::string> child{get_node_name(event->child)};
		if (!parent || !child) continue;
		std::println(out_ss, "\t\"{}\" -> \"{}\" [label=\"{}\"]",
			parent.value(),
			child.value(),
			event->get_relation()
		);
	}

	std::println(out_ss, "\tsubgraph uids {{");
	std::println(out_ss, "\t\tedge [dir=none]");
	for (const std::pair<std::optional<UID>, std::set<Proc>>& uid : uid_procs) {
		if (uid.second.size() == 1 || !uid.first.has_value()) continue;
		for (const Proc& proc : uid.second) {
			std::optional<std::string> node{get_node_name_until_pid(proc)};
			if (!node) continue;

			std::println(out_ss, "\t\t\"{}\" -> \"{}\" [label=\"UID belongs to\"]",
				uid.first.value(), node.value()
			);
		}
	}
	std::println(out_ss, "\t}}");
	
	out_ss << FOOTER;
	return out_ss.str();
}

std::optional<std::string> Builder::get_node_name(const Proc& proc) noexcept {
	if (proc.name.has_value()) {
		return proc.name.value();
	} else if (proc.ids.has_value()) {
		if (proc.ids.value().pid.has_value()) {
			return std::to_string(proc.ids.value().pid.value());
		} else {
			return std::to_string(proc.ids.value().uid);
		}
	} else if (proc.package.has_value()) {
		return "cluster_"+proc.package.value();
	} else {
		return std::nullopt;
	}
}

std::optional<std::string> Builder::get_node_name_until_pid(const Proc& proc) noexcept {
	if (proc.name.has_value()) {
		return proc.name.value();
	} else if (proc.ids.has_value() && proc.ids.value().pid.has_value()) {
		return std::to_string(proc.ids.value().pid.value());
	} else {
		return std::nullopt;
	}
}

