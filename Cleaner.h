#pragma once

#include "RelationGenerator.h"

class Cleaner {
public:
	Cleaner() = delete;

	static std::vector<ProcRelation> clean_relations(std::vector<ProcRelation> relations);

private:
	static void set_procs_equal(std::shared_ptr<Proc>& proc1, std::shared_ptr<Proc>& proc2);
};

