#pragma once

#include "Log.h"

class Reader {
public:
	Reader() = delete;

	static std::vector<std::shared_ptr<Log>> read_bugreport(const std::vector<std::string>& lines);
};

