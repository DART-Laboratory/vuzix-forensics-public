#include "Cleaner.h"
#include "Node.h"

void Cleaner::clean_relations(Timeline& timeline, const std::vector<std::shared_ptr<Log>>& logs, const CleanerOptions& options) {
	std::vector<std::shared_ptr<Node>> nodes{};
	nodes.reserve(timeline.events.size()*2);
	for (const Event& event : timeline.events) {
		nodes.emplace_back(event.child);
		nodes.emplace_back(event.parent);
	}

	map_sensor_ids(nodes, logs);
	std::println(std::cout, "Cleaner: Map sensor ids passed");
	extract_package_names(nodes);
	std::println(std::cout, "Cleaner: Extract package names passed");
	prefix_packages_on_names(nodes);
	std::println(std::cout, "Cleaner: prefix packages on names passed");
	relate_similar_nodes(nodes);
	std::println(std::cout, "Cleaner: Relate similar nodes passed");
}

void Cleaner::map_sensor_ids(const std::vector<std::shared_ptr<Node>>& nodes, const std::vector<std::shared_ptr<Log>>& logs) {
	std::map<uint32_t, std::string> id_to_name{};
	for (const std::shared_ptr<Log>& log : logs) {
		if (auto sensor_info{std::dynamic_pointer_cast<SensorInfoLog>(log)}) {
			id_to_name.insert(std::make_pair(sensor_info->sensor_id, sensor_info->type));
		}
	}

	for (const std::shared_ptr<Node>& node : nodes) {
		if (auto sensor_node{std::dynamic_pointer_cast<Sensor>(node)}) {
			if (auto mapping{id_to_name.find(sensor_node->id)}; mapping != id_to_name.end()) {
				sensor_node->name = mapping->second;
			} else {
				throw std::runtime_error{"Sensor id does not map to any name."};
			}
		}
	}
}

void Cleaner::extract_package_names(const std::vector<std::shared_ptr<Node>>& nodes) {
	std::set<Package> packages{};
	for (const std::shared_ptr<Node>& node : nodes) {
		if (auto pkg{node->package}) {
			packages.emplace(pkg.value());
		}
	}

	for (const std::shared_ptr<Node>& node : nodes) {
		for (const Package& package : packages) {
			node->extract_package_from_name(package);
		}
	}
}

void Cleaner::prefix_packages_on_names(const std::vector<std::shared_ptr<Node>>& nodes) {
	for (const std::shared_ptr<Node>& node : nodes) {
		node->prefix_package_on_name();
	}
}

void Cleaner::relate_similar_nodes(std::vector<std::shared_ptr<Node>>& nodes) {
	for (size_t i{0}; i < nodes.size(); i++) {
		for (size_t j{i+1}; j < nodes.size(); j++) {
			if (nodes[i]->similar(nodes[j])) {
				nodes[i]->relate(nodes[j]);
				nodes[i] = nodes[j];
			}
		}
	}
}

