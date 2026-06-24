#pragma once

#include "Reader.h"

struct Proc {
	std::optional<std::string> name;
	std::optional<uint16_t> pid;
	std::optional<uint64_t> uid;
};

template<>
struct std::formatter<Proc> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Proc& p, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "(NAME {} | PID {} | UID {})",
			p.name.value_or("n/a"),
			(p.pid.has_value() ? std::to_string(p.pid.value()) : "n/a"),
			(p.uid.has_value() ? std::to_string(p.uid.value()) : "n/a")
		);
    }
};

struct ProcRelation {
	Proc lhs;
	enum class Relation {
		Started
	} relation;
	Proc rhs;
};

template<>
struct std::formatter<ProcRelation> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const ProcRelation& r, std::format_context& ctx) const {
		std::string relation{};
		switch (r.relation) {
			default:
				relation = "Started";
				break;
		}
        return std::format_to(ctx.out(), "{} '{}' -> {}", r.lhs, relation, r.rhs);
    }
};

class Analyzer {
public:
	Analyzer() = delete;

	static std::vector<ProcRelation> analyze_logcat(const std::vector<std::shared_ptr<LogcatLog>>& logs);
	static std::vector<ProcRelation> analyze_bugreport(const std::vector<std::shared_ptr<Log>>& logs);

	static uint64_t get_uid(const std::string& str) noexcept;
};

