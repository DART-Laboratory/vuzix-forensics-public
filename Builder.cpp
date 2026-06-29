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

	const std::map<std::optional<Package>, std::vector<Proc>> package_procs{sort_into_packages(timeline)};
	std::vector<const Proc*> procs{};
	for (const std::pair<std::optional<Package>, std::vector<Proc>>& package : package_procs) {
		if (package.first.has_value()) {
			std::println(out_ss, "\tsubgraph \"cluster_{}\" {{", package.first.value());
		}
		for (const Proc& proc : package.second) {
			procs.emplace_back(&proc);
			if (package.first.has_value()) std::print(out_ss, "\t");
			std::println(out_ss, "\t\"{}\"", proc.name.value_or(NO_NAME));
		}
		
		if (package.first.has_value()) {
			std::println(out_ss, "\t\tlabel=\"{}\"", package.first.value());
			std::println(out_ss, "\t}}");
		}
		//std::println(out_ss, "\t\"{}\" -> \"{}\" [label=\"{}\"];", *rel.lhs, *rel.rhs, rel.relation);
	}

	for (const std::unique_ptr<Event>& event : timeline.events) {
		std::println(out_ss, "\"{}\" -> \"{}\"",
			event->parent.name.value_or(NO_NAME),
			event->child.name.value_or(NO_NAME)
		);
	}
	
	out_ss << FOOTER;
	return out_ss.str();
}

std::map<std::optional<Package>, std::vector<Proc>> Builder::sort_into_packages(const Timeline& timeline) {
	std::map<std::optional<Package>, std::vector<Proc>> package_procs{};
	for (const std::unique_ptr<Event>& event : timeline.events) {
		package_procs[event->parent.package].emplace_back(event->parent);
		package_procs[event->child.package].emplace_back(event->child);
	}

	return package_procs;
}

