#include <ranges>
#include <algorithm>
#include "Cleaner.h"

void Cleaner::clean_relations(Timeline& timeline, const CleanerOptions& options) {
	/*std::vector<std::shared_ptr<Proc>> procs{};
	procs.reserve(relations.size()*2);
	for (const ProcRelation& rel : relations) {
		procs.emplace_back(rel.lhs);
		procs.emplace_back(rel.rhs);
	}

	for (ProcRelation& rel : relations) {
		for (std::shared_ptr<Proc>& proc : procs) {
			if (rel.lhs != proc && rel.lhs->set_equal_if_similar(*proc)) {
				rel.lhs = proc;
			} else if (rel.rhs != proc && rel.rhs->set_equal_if_similar(*proc)) {
				rel.rhs = proc;
			}
		}
	}

	return relations;*/

	/*
	auto sort_into_pkg{timeline.sort_into_packages()};
	if (options.merge_procs_starting_with_dot) {
		for (const std::pair<std::optional<Package>, std::set<Proc*>>& pkg : sort_into_pkg) {
			for (Proc* proc : pkg.second) {
				if (!proc->package.has_value() || !proc->name.has_value()) continue;
				if (!proc->name.value().starts_with('.')) continue;

				std::string combined_name{proc->package.value()+proc->name.value()};
				if (std::ranges::find_if(pkg.second, [&combined_name](const Proc* proc) {
					if (!proc->name.has_value()) return false;
					return proc->name.value() == combined_name;
				}) != pkg.second.end()) {
					proc->name = combined_name;
				}
			}
		}
	}
	*/
}

