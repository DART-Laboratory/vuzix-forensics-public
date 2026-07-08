#pragma once

#include <optional>
#include <memory>
#include <chrono>
#include <map>
#include <iostream>
#include "GraphComponent.h"

using UID = uint64_t;
using PID = uint16_t;
using Package = std::string;

struct Node {
	virtual std::optional<std::string> get_node_name() const noexcept = 0;
	virtual std::vector<GraphComponent> get_graph_components() const { return {}; }
	auto operator<=>(const Node& node) const noexcept = default;
};

struct Proc : public Node {
	std::optional<Package> package;
	std::optional<std::string> name;

	struct IDs {
		UID uid;
		std::optional<PID> pid;

		auto operator<=>(const IDs&) const noexcept = default;
	};
	std::optional<IDs> ids;

	Proc(
		const std::optional<Package>& pkg,
		const std::optional<std::string>& name,
		const std::optional<IDs>& ids
	) : package{pkg}, name{name}, ids{ids} { }

	auto operator<=>(const Proc&) const noexcept = default;
	std::optional<std::string> get_node_name() const noexcept override;
	std::optional<std::string> get_node_name_until_pid() const noexcept;
	std::vector<GraphComponent> get_graph_components() const override;
	
	static inline const std::array<std::string, 4> proc_types{
		"pre-top-activity",
		"top-activity",
		"service",
		"content provider"
	};
};

struct Sensor : public Node {
	uint32_t id;
	std::string name;

	Sensor(const uint32_t& id) : id{id} { }

	auto operator<=>(const Sensor&) const noexcept = default;
	std::optional<std::string> get_node_name() const noexcept override;
	std::vector<GraphComponent> get_graph_components() const override;
};

struct JavaClass : public Node {
	std::string name;

	JavaClass(const std::string& name) : name{name} { }

	auto operator<=>(const JavaClass&) const noexcept = default;
	std::optional<std::string> get_node_name() const noexcept override;
	std::vector<GraphComponent> get_graph_components() const override;
};

struct Broadcast : public Node {
	std::optional<Package> package;
	std::string name;

	Broadcast(const std::optional<Package>& pkg, const std::string& name)
		: package{pkg}, name{name} { }

	auto operator<=>(const Broadcast&) const noexcept = default;
	std::optional<std::string> get_node_name() const noexcept override;
	std::vector<GraphComponent> get_graph_components() const override;
};

struct Event {
	std::chrono::month_day date;
	std::chrono::milliseconds time;

	std::shared_ptr<Node> parent;
	std::shared_ptr<Node> child;

	enum class Relation {
		Started, StartedForBroadcast, RegisteredSensor
	} relation;
	inline static const std::map<Relation, std::string> relation_to_str{
		{Relation::Started, "Started"},
		{Relation::StartedForBroadcast, "Started for broadcast"},
		{Relation::RegisteredSensor, "Registered sensor"}
	};

	Event() = default;
	Event(
		const std::chrono::month_day& date,
		const std::chrono::milliseconds& time,
		const std::shared_ptr<Node>& parent,
		Relation relation, const std::shared_ptr<Node>& child
	) : date{date}, time{time}, parent{parent}, relation{relation}, child{child} { }
	
	GraphComponent get_graph_component() const;
};

