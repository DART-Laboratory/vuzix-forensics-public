#pragma once

#include "TimelineGenerator.h"

struct CleanerOptions {
	bool map_sensor_ids{true};
	bool extract_package_names{true};
};

class Cleaner {
public:
	Cleaner() = delete;

	static void clean_relations(Timeline& timeline, const std::vector<std::shared_ptr<Log>>& logs, const CleanerOptions& options);

private:
	static void map_sensor_ids(const std::vector<std::shared_ptr<Node>*>& nodes, const std::vector<std::shared_ptr<Log>>& logs);
	static void extract_package_names(const std::vector<std::shared_ptr<Node>*>& nodes);
	static void prefix_packages_on_names(const std::vector<std::shared_ptr<Node>*>& nodes);
	static void relate_similar_nodes(std::vector<std::shared_ptr<Node>*>& nodes);
};

