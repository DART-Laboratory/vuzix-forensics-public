#include "Log.h"
#include <regex>

std::optional<std::shared_ptr<SensorDumpLog>> SensorDumpLog::read_log(const std::string& l) {
	SensorDumpLog log{};

	const std::regex r{
		"(\\d{2}:\\d{2}:\\d{2}) (\\+|-) (0x\\d{8}) pid= ?(\\d{4}) uid= ?(\\d{5}) package=(.*)"
	};
	std::smatch m{};
	std::regex_match(l, m, r);
	if (m.empty()) return std::nullopt;

	std::stringstream ss{m[1].str()};
	ss >> std::chrono::parse("%T", log.time);

	log.register_or_not = (m[2] == '+');

	ss.clear();
	ss << std::hex << m[3].str();
	ss >> log.sensor_id;

	log.pid = std::stoi(m[4].str());
	log.uid = std::stoi(m[5].str());
	log.package = m[6].str();

	return std::make_shared<SensorDumpLog>(log);
}

std::optional<Event> SensorDumpLog::generate_event() {
	if (!register_or_not) return std::nullopt;
	return Event{
		std::chrono::month_day{},
		time,
		std::make_shared<Proc>(
			std::nullopt,
			package,
			Proc::IDs{
				uid,
				pid
			}
		),
		Event::Relation::RegisteredSensor,
		std::make_shared<Sensor>(
			sensor_id
		)
	};
}

