#include "TimelineGenerator.h"

Timeline TimelineGenerator::generate_timeline(const std::vector<std::shared_ptr<Log>>& logs) {
	Timeline timeline{};

	for (const std::shared_ptr<Log>& log : logs) {
		if (auto event{log->generate_event()}) {
			timeline.events.emplace_back(event.value());
		}
	}

	return timeline;
}

