#include <string>
#include <vector>

struct ProcRelation {
	std::string lhs, rhs;

	enum class Relation {
		Start,
		Kill
	} relation;
};

std::vector<ProcRelation> build_relations(const std::vector<Log>& logs) {
}


