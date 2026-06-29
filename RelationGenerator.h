#pragma once

#include "Reader.h"
#include <format>
#include <concepts>
#include <variant>

template <typename T1, typename T2> requires std::equality_comparable_with<T1, T2>
static int optional_value_compare(const std::optional<T1>& lhs, const std::optional<T2>& rhs) {
	bool lhs_has_value{lhs.has_value()};
	bool rhs_has_value{rhs.has_value()};
	if (!lhs_has_value) return 0;
	if (!rhs_has_value) return 1;
	if (*lhs == *rhs) return 2;
	else return 3;
}

struct Proc {
	std::optional<std::string> name;
	std::optional<uint16_t> pid;
	std::optional<uint64_t> uid;

	bool has_similarity(Proc& proc) {
		int name_comp{optional_value_compare(name, proc.name)};
		int pid_comp{optional_value_compare(pid, proc.pid)};
		int uid_comp{optional_value_compare(uid, proc.uid)};

		if (name_comp == 2) return true;
		else if (name_comp == 3) return false;
		
		if (pid_comp == 2) return true;
		if (uid_comp == 2) return true;

		return false;
	}
	bool set_equal_if_similar(Proc& proc) {
		if (!has_similarity(proc)) return false;

		if (proc.name.has_value()) name = proc.name;
		if (proc.pid.has_value()) pid = proc.pid;
		if (proc.uid.has_value()) uid = proc.uid;

		if (name.has_value()) proc.name = name;
		if (pid.has_value()) proc.pid = pid;
		if (uid.has_value()) proc.uid = uid;

		return true;
	}
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
	std::shared_ptr<Proc> lhs;
	enum class Relation {
		Started,
		NeededToStart,
		Hosted,
	} relation;
	std::shared_ptr<Proc> rhs;
};

template<>
struct std::formatter<ProcRelation::Relation> {
    constexpr std::format_parse_context::const_iterator parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

	std::format_context::iterator format(const ProcRelation::Relation& r, std::format_context& ctx) const {
		switch (r) {
			case ProcRelation::Relation::Started:
				return std::format_to(ctx.out(), "Started");
				break;
			case ProcRelation::Relation::NeededToStart:
				return std::format_to(ctx.out(), "Needed to start");
				break;
			case ProcRelation::Relation::Hosted:
				return std::format_to(ctx.out(), "Hosted");
				break;
		}
    }
};

template<>
struct std::formatter<ProcRelation> {
    constexpr std::format_parse_context::const_iterator parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

	std::format_context::iterator format(const ProcRelation& r, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{} '{}' -> {}", *r.lhs, r.relation, *r.rhs);
    }
};

class RelationGenerator {
public:
	RelationGenerator() = delete;

	static std::vector<ProcRelation> generate_logcat_relations(const std::vector<std::shared_ptr<LogcatLog>>& logs);
	static std::vector<ProcRelation> generate_bugreport_relations(const std::vector<std::shared_ptr<Log>>& logs);

	static uint64_t get_uid(const std::string& str) noexcept;
	static std::variant<std::pair<std::string, std::string>, std::string> split_activity_string(const std::string& str) noexcept;
};

