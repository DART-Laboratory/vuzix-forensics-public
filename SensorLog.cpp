#include "Log.h"
#include <regex>

std::optional<std::shared_ptr<SensorRegisterLog>> SensorRegisterLog::read_log(const std::string& l) {
	SensorRegisterLog log{};

	const std::regex r{
		"(\\d{2}:\\d{2}:\\d{2}) (\\+|-) (0x.{8}) pid= ?(\\d{4}) uid= ?(\\d+) package=([^ ]+).*"
	};
	std::smatch m{};
	std::regex_match(l, m, r);
	if (m.empty()) return std::nullopt;

	std::stringstream ss{m[1].str()};
	ss >> std::chrono::parse("%T", log.time);

	log.register_or_not = (m[2] == '+');

	ss = std::stringstream{};
	ss << std::hex << m[3].str();
	ss >> log.sensor_id;

	log.pid = std::stoi(m[4].str());
	log.uid = std::stoi(m[5].str());
	log.package = m[6].str();

	return std::make_shared<SensorRegisterLog>(log);
}

std::optional<Event> SensorRegisterLog::generate_event() {
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

std::optional<std::shared_ptr<SensorInfoLog>> SensorInfoLog::read_log(const std::string& l) {
	SensorInfoLog log{};

	const std::regex r{
		"(0x.{8})\\) (.+?(?= \\|)).*type: (.+?(?= \\|)).*"
	};
	std::smatch m{};
	std::regex_match(l, m, r);
	if (m.empty()) return std::nullopt;

	std::stringstream ss{};
	ss << std::hex << m[1].str();
	ss >> log.sensor_id;

	log.component_name = m[2].str();
	log.type = m[3].str();

	return std::make_shared<SensorInfoLog>(log);
}

