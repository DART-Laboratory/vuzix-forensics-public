#include <ranges>
#include <algorithm>
#include "Cleaner.h"

std::vector<ProcRelation> Cleaner::clean_relations(std::vector<ProcRelation> relations) {
	std::vector<std::shared_ptr<Proc>> procs{};
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

	return relations;
}

