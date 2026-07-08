#pragma once

#include "TimelineGenerator.h"

struct CleanerOptions {
	bool merge_procs_starting_with_dot{false};
};

class Cleaner {
public:
	Cleaner() = delete;

	static void clean_relations(Timeline& timeline, const std::vector<std::shared_ptr<Log>>& logs, const CleanerOptions& options);

private:
	static void set_procs_equal(std::shared_ptr<Proc>& proc1, std::shared_ptr<Proc>& proc2);
};

