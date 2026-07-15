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
	std::optional<Package> package;
	std::optional<std::string> name;

	Node(const std::optional<Package>& pkg, const std::optional<std::string>& name)
		: package{pkg}, name{name} { }

	auto operator<=>(const Node& node) const noexcept = default;

	virtual std::optional<std::string> get_node_name() const noexcept;
	virtual std::vector<GraphComponent> get_graph_components() const;

	void extract_package_from_name(const Package& package) noexcept;
	void prefix_package_on_name() noexcept;

	virtual bool similar(const std::shared_ptr<Node>& node) const;
	virtual void relate(const std::shared_ptr<Node>& node);

	enum class Shape {
		Ellipse, Hexagon, Octagon, Box, Parallelogram
	};

	inline static const std::map<Shape, std::string> shape_to_str{
		{Shape::Ellipse,	"ellipse"},
		{Shape::Box,		"box"},
		{Shape::Hexagon,	"hexagon"},
		{Shape::Octagon,	"octagon"},
		{Shape::Parallelogram,	"parallelogram"}
	};
	
	inline static const std::string dummy_package_node_prefix{"DUMMY_NODE_"};

	static std::string package_cluster(const std::optional<Package>& package, const std::string& cluster_contents);

	virtual constexpr Shape get_shape() const noexcept = 0;
	virtual constexpr uint32_t get_color() const noexcept = 0;

protected:
	GraphComponent get_node_def_graph_component(const std::string& node_name) const;
};

struct Proc : public Node {
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
	) : Node{pkg, name}, ids{ids} { }

	std::optional<std::string> get_node_name() const noexcept override;
	std::optional<std::string> get_node_name_until_pid() const noexcept;
	std::vector<GraphComponent> get_graph_components() const override;

	bool similar(const std::shared_ptr<Node>& node) const override;
	void relate(const std::shared_ptr<Node>& node) override;
	
	constexpr Shape get_shape() const noexcept override {
		return Node::Shape::Ellipse;
	}
	constexpr uint32_t get_color() const noexcept override {
		return 0xd6eaf8;
	}

	auto operator<=>(const Proc&) const noexcept = default;
};

struct Service : public Node {
	Service(const std::optional<Package>& pkg, const std::string& name)
		: Node{pkg, name} { }

	constexpr Shape get_shape() const noexcept override {
		return Node::Shape::Octagon;
	}
	constexpr uint32_t get_color() const noexcept override {
		return 0xabebc6;
	}

	auto operator<=>(const Service&) const noexcept = default;
};

struct Sensor : public Node {
	uint32_t id;

	Sensor(const uint32_t& id) : Node{std::nullopt, std::nullopt}, id{id} { }

	constexpr Shape get_shape() const noexcept override {
		return Node::Shape::Box;
	}
	constexpr uint32_t get_color() const noexcept override {
		return 0xfdebd0;
	}

	bool similar(const std::shared_ptr<Node>& node) const override;

	auto operator<=>(const Sensor&) const noexcept = default;
};

struct Component : public Node {
	Component(const std::string& name) : Node {std::nullopt, name} { }

	constexpr Shape get_shape() const noexcept override {
		return Node::Shape::Box;
	}
	constexpr uint32_t get_color() const noexcept override {
		return 0xf4d4ed;
	}

	auto operator<=>(const Component&) const noexcept = default;
};

struct Activity : public Node {
	Activity(const std::optional<Package>& pkg, const std::string& name)
		: Node{pkg, name} { }

	constexpr Shape get_shape() const noexcept override {
		return Node::Shape::Ellipse;
	}
	constexpr uint32_t get_color() const noexcept override {
		return 0xd5f5e3;
	}

	auto operator<=>(const Activity&) const noexcept = default;
};

struct JavaClass : public Node {
	JavaClass(const std::string& name) : Node{std::nullopt, name} { }

	constexpr Shape get_shape() const noexcept override {
		return Node::Shape::Octagon;
	}
	constexpr uint32_t get_color() const noexcept override {
		return 0xe8daef;
	}

	auto operator<=>(const JavaClass&) const noexcept = default;
};

struct BroadcastReceiver : public Node {
	BroadcastReceiver(const std::optional<Package>& pkg, const std::string& name)
		: Node{pkg, name} { }

	constexpr Shape get_shape() const noexcept override {
		return Node::Shape::Hexagon;
	}
	constexpr uint32_t get_color() const noexcept override {
		return 0xfadbd8;
	}

	auto operator<=>(const BroadcastReceiver&) const noexcept = default;
};

struct Event {
	std::chrono::month_day date;
	std::chrono::milliseconds time;

	std::shared_ptr<Node> parent;
	std::shared_ptr<Node> child;

	enum class Relation {
		Started, StartedForBroadcast, RegisteredSensor, StartedForService, ConnectedTo
	} relation;
	inline static const std::map<Relation, std::string> relation_to_str{
		{Relation::Started, "Started"},
		{Relation::StartedForBroadcast, "Started for broadcast"},
		{Relation::RegisteredSensor, "Registered sensor"},
		{Relation::StartedForService, "Started for service"},
		{Relation::ConnectedTo, "Connected to"}
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

