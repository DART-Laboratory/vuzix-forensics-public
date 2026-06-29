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
	std::vector<const Proc*> procs{};
	for (const std::pair<std::optional<Package>, std::set<Proc>>& pkg : pkg_procs) {
		if (pkg.first.has_value()) {
			std::println(out_ss, "\tsubgraph \"cluster_{}\" {{", pkg.first.value());
		}
		for (const Proc& proc : pkg.second) {
			procs.emplace_back(&proc);
			if (pkg.first.has_value()) std::print(out_ss, "\t");
			std::println(out_ss, "\t\"{}\"", proc.name.value_or(NO_NAME));
		}
		
		if (pkg.first.has_value()) {
			std::println(out_ss, "\t\tlabel=\"{}\"", pkg.first.value());
			std::println(out_ss, "\t}}");
		}
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

