#include "Cleaner.h"

void Cleaner::clean_relations(Timeline& timeline, const std::vector<std::shared_ptr<Log>>& logs, const CleanerOptions& options) {
	std::map<uint32_t, std::string> id_to_name{};
	for (const std::shared_ptr<Log>& log : logs) {
		if (auto sensor_info{std::dynamic_pointer_cast<SensorInfoLog>(log)}) {
			id_to_name.insert(std::make_pair(sensor_info->sensor_id, sensor_info->type));
		}
	}

	std::vector<std::shared_ptr<Node>> nodes{};
	nodes.reserve(timeline.events.size()*2);
	for (const Event& event : timeline.events) {
		nodes.emplace_back(event.child);
		nodes.emplace_back(event.parent);
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

