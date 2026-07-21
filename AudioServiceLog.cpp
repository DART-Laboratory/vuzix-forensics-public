#include "Log.h"
#include <regex>
#include <print>

std::optional<std::shared_ptr<AudioServiceLog>> AudioServiceLog::read_log(const std::string& l) {
	AudioServiceLog log{};

	const std::regex r{
		"(\\d{2}-\\d{2}) (\\d{2}:\\d{2}:\\d{2}:\\d{3}) rec ((update)|(stop)|(start)).*uid:(\\d+).*src:(.*) .*pack:(.*)"
	};
	std::smatch m{};
	std::regex_match(l, m, r);
	if (m.empty()) return std::nullopt;

	std::istringstream ss{m[1].str()};
	ss >> std::chrono::parse("%m-%d", log.date);
	ss = std::istringstream{m[2].str()};
	ss >> std::chrono::parse("%T", log.time);

	log.uid = std::stoi(m[7].str());
	log.source = m[8].str();
	log.package = m[9].str();

	if (m[3] == "update") {
		log.event = Event::Relation::AudioRecUpdated;
	} else if (m[3] == "stop") {
		log.event = Event::Relation::AudioRecStoped;
	} else if (m[3] == "start") {
		log.event = Event::Relation::AudioRecStarted;
	} else {
		throw std::runtime_error{"Unsupported audio record event"};
	}

	return std::make_shared<AudioServiceLog>(log);
}

std::optional<Event> AudioServiceLog::generate_event() {
	return Event{
		date, time,
		std::make_shared<Proc>(
			package, std::nullopt,
			//Proc::IDs{uid, std::nullopt}
			std::nullopt
		),
		event,
		std::make_shared<Component>(source)
	};
}

