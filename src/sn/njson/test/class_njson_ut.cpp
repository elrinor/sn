#include <limits>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h> // NOLINT: not a C system header.
#include <nlohmann/json.hpp>

#include "sn/core/exception.h"
#include "sn/core/globals.h"
#include "sn/core/tag.h"
#include "sn/core/type_name.h"
#include "sn/detail/format/format.h"
#include "sn/njson/class_njson.h"
#include "sn/njson/enum_njson.h"
#include "sn/reflection/class_reflection.h"
#include "sn/reflection/enum_reflection.h"

namespace classnjsonns {

struct short_names : sn::tags::tag {};

enum class kind {
    SMALL,
    LARGE,
};
using enum kind;

SN_DEFINE_ENUM_REFLECTION(kind, ({
    {SMALL, "small"},
    {LARGE, "large"},
}))
SN_DEFINE_ENUM_NJSON_FUNCTIONS(kind, sn::case_sensitive)

SN_DEFINE_ENUM_REFLECTION(kind, ({
    {SMALL, "s"},
    {LARGE, "l"},
}), short_names)
SN_DEFINE_ENUM_NJSON_FUNCTIONS(kind, sn::case_sensitive, short_names)

struct point {
    int x = 0;
    int y = 0;
    friend bool operator==(const point &, const point &) = default;
};

SN_DEFINE_CLASS_REFLECTION(point, (
    (&point::x, "x"),
    (&point::y, "y")
))
SN_DEFINE_CLASS_NJSON_FUNCTIONS(point)

SN_DEFINE_CLASS_REFLECTION(point, (
    (&point::x, "X"),
    (&point::y, "Y")
), short_names)
SN_DEFINE_CLASS_NJSON_FUNCTIONS(point, short_names)

struct shape {
    std::string name;
    kind size = SMALL;
    kind short_size = SMALL;
    std::vector<point> points;
    std::optional<int> layer;
    std::map<std::string, double> properties;
    point origin;
    friend bool operator==(const shape &, const shape &) = default;
};

SN_DEFINE_CLASS_REFLECTION(shape, (
    (&shape::name, "name"),
    (&shape::size, "size"),
    (&shape::short_size, "short_size", short_names()),
    (&shape::points, "points"),
    (&shape::layer, "layer"),
    (&shape::properties, "properties"),
    (&shape::origin, "origin")
))
SN_DEFINE_INLINE_CLASS_NJSON_FUNCTIONS(shape)

class counter {
public:
    [[nodiscard]] int count() const {
        return _count;
    }

    void set_count(int count) {
        _count = count;
    }

    friend bool operator==(const counter &, const counter &) = default;

private:
    int _count = 0;
};

SN_DEFINE_CLASS_REFLECTION(counter, (
    (&counter::count, &counter::set_count, "count")
))
SN_DEFINE_STATIC_CLASS_NJSON_FUNCTIONS(counter)

struct node {
    int value = 0;
    std::vector<node> children;
    friend bool operator==(const node &, const node &) = default;
};

SN_DEFINE_CLASS_REFLECTION(node, (
    (&node::value, "value"),
    (&node::children, "children")
))
SN_DEFINE_CLASS_NJSON_FUNCTIONS(node)

struct empty {
    friend bool operator==(const empty &, const empty &) = default;
};

SN_DEFINE_CLASS_REFLECTION(empty, ())
SN_DEFINE_CLASS_NJSON_FUNCTIONS(empty)

struct measurement {
    double value = 0.0;
};

SN_DEFINE_CLASS_REFLECTION(measurement, (
    (&measurement::value, "value")
))
SN_DEFINE_CLASS_NJSON_FUNCTIONS(measurement)

struct reflected_only {
    int value = 0;
};

SN_DEFINE_CLASS_REFLECTION(reflected_only, (
    (&reflected_only::value, "value")
))

} // namespace classnjsonns

using namespace classnjsonns; // NOLINT

template<class T, class... Tags>
static void expect_valid_fromto(const nlohmann::json &json, const T &value, Tags... tags) {
    nlohmann::json json_dst;
    EXPECT_TRUE(sn::try_to_njson(value, &json_dst, tags...));
    EXPECT_EQ(json_dst, json);
    EXPECT_EQ(sn::to_njson(value, tags...), json);

    T value_dst = T();
    EXPECT_TRUE(sn::try_from_njson(json, &value_dst, tags...)) << "with json = " << json.dump();
    EXPECT_TRUE(value_dst == value) << "with json = " << json.dump();
    EXPECT_TRUE(sn::from_njson<T>(json, tags...) == value) << "with json = " << json.dump();
}

template<class T>
static void expect_throwing_from(const nlohmann::json &src, std::string_view message) {
    T dst = T();
    EXPECT_FALSE(sn::try_from_njson(src, &dst)) << "with src = " << src.dump();
    try {
        sn::from_njson(src, &dst);
        ADD_FAILURE() << "Expected an exception with src = " << src.dump();
    } catch (const sn::exception &e) {
        EXPECT_EQ(std::string_view(e.what()), message);
    }
}

static shape make_shape() {
    shape result;
    result.name = "triangle";
    result.size = LARGE;
    result.short_size = LARGE;
    result.points = {{0, 0}, {1, 0}, {0, 1}};
    result.layer = 2;
    result.properties = {{"weight", 0.5}};
    result.origin = {10, 20};
    return result;
}

static std::string missing_message(std::string_view type_name, std::string_view field) {
    return sn::detail::format("Cannot deserialize field '{}' of '{}': json object doesn't have it", field, type_name);
}

TEST(njson_class, concepts) {
    static_assert(sn::concepts::to_njsonable<point>);
    static_assert(sn::concepts::from_njsonable<point>);
    static_assert(sn::concepts::to_njsonable<point, short_names>);
    static_assert(sn::concepts::from_njsonable<point, short_names>);
    static_assert(sn::concepts::to_njsonable<shape>);
    static_assert(!sn::concepts::to_njsonable<shape, short_names>);
    static_assert(sn::concepts::to_njsonable<std::vector<shape>>);
    static_assert(sn::concepts::to_njsonable<counter>);
    static_assert(sn::concepts::to_njsonable<node>);
    static_assert(sn::concepts::from_njsonable<node>);
    static_assert(sn::concepts::to_njsonable<empty>);

    // Reflection alone is not enough.
    static_assert(!sn::concepts::to_njsonable<reflected_only>);
    static_assert(!sn::concepts::from_njsonable<reflected_only>);
}

TEST(njson_class, basic) {
    expect_valid_fromto(nlohmann::json::parse(R"({"x": 1, "y": -2})"), point{1, -2});
    expect_valid_fromto(nlohmann::json::object(), empty());

    // nlohmann::json sorts keys.
    EXPECT_EQ(sn::to_njson(point{1, 2}).dump(), R"({"x":1,"y":2})");
}

TEST(njson_class, nested) {
    expect_valid_fromto(nlohmann::json::parse(R"({
        "name": "triangle",
        "size": "large",
        "short_size": "l",
        "points": [{"x": 0, "y": 0}, {"x": 1, "y": 0}, {"x": 0, "y": 1}],
        "layer": 2,
        "properties": {"weight": 0.5},
        "origin": {"x": 10, "y": 20}
    })"), make_shape());

    expect_valid_fromto(nlohmann::json::parse(R"({"value": 1, "children": [{"value": 2, "children": [{"value": 3, "children": []}]}]})"),
                        node{1, {node{2, {node{3, {}}}}}});
}

TEST(njson_class, getters_setters) {
    counter value;
    value.set_count(5);
    expect_valid_fromto(nlohmann::json::parse(R"({"count": 5})"), value);
}

TEST(njson_class, tags) {
    // Class-level tags select the reflection.
    expect_valid_fromto(nlohmann::json::parse(R"({"X": 1, "Y": 2})"), point{1, 2}, short_names());
    expect_throwing_from<point>(nlohmann::json::parse(R"({"X": 1, "Y": 2})"), missing_message(sn::type_name<point>(), "x"));

    // Field-level tags are used for the field.
    nlohmann::json json = sn::to_njson(make_shape());
    EXPECT_EQ(json["size"], nlohmann::json("large"));
    EXPECT_EQ(json["short_size"], nlohmann::json("l"));
}

TEST(njson_class, missing_fields) {
    expect_throwing_from<point>(nlohmann::json::parse(R"({"x": 1})"), missing_message(sn::type_name<point>(), "y"));
    expect_throwing_from<point>(nlohmann::json::object(), missing_message(sn::type_name<point>(), "x"));

    // Optional fields can be missing.
    nlohmann::json json = sn::to_njson(make_shape());
    json.erase("layer");
    shape dst = make_shape();
    EXPECT_TRUE(dst.layer);
    sn::from_njson(json, &dst);
    EXPECT_EQ(dst.layer, std::nullopt);

    dst = make_shape();
    EXPECT_TRUE(sn::try_from_njson(json, &dst));
    EXPECT_EQ(dst.layer, std::nullopt);

    // Or null, and that's what we write for std::nullopt.
    json["layer"] = nullptr;
    dst = make_shape();
    sn::from_njson(json, &dst);
    EXPECT_EQ(dst.layer, std::nullopt);
    EXPECT_EQ(sn::to_njson(dst), json);

    // But non-optional fields can't be null.
    json["name"] = nullptr;
    EXPECT_FALSE(sn::try_from_njson(json, &dst));
}

TEST(njson_class, unknown_members) {
    expect_valid_fromto(nlohmann::json::parse(R"({"x": 1, "y": 2})"), point{1, 2});
    EXPECT_EQ(sn::from_njson<point>(nlohmann::json::parse(R"({"x": 1, "y": 2, "z": 3})")), (point{1, 2}));
    EXPECT_EQ(sn::from_njson<point>(nlohmann::json::parse(R"({"X": 3, "x": 1, "y": 2})")), (point{1, 2}));
}

TEST(njson_class, errors) {
    std::string point_name(sn::type_name<point>());
    std::string shape_name(sn::type_name<shape>());
    std::string node_name(sn::type_name<node>());

    expect_throwing_from<point>(nlohmann::json::array({1, 2}), sn::detail::format("Cannot deserialize json value '[1,2]' as '{}'", point_name));
    expect_throwing_from<point>(nlohmann::json(nullptr), sn::detail::format("Cannot deserialize json value 'null' as '{}'", point_name));

    expect_throwing_from<point>(
        nlohmann::json::parse(R"({"x": 1, "y": "2"})"),
        sn::detail::format("Cannot deserialize field 'y' of '{}': Cannot deserialize json value '\"2\"' as '{}'", point_name, sn::type_name<int>()));

    nlohmann::json json = sn::to_njson(make_shape());
    json["points"][1]["x"] = 1.5;
    expect_throwing_from<shape>(
        json,
        sn::detail::format("Cannot deserialize field 'points' of '{}': Cannot deserialize array element 1: "
                           "Cannot deserialize field 'x' of '{}': Cannot deserialize json value '1.5' as '{}'",
                           shape_name, point_name, sn::type_name<int>()));

    json = sn::to_njson(make_shape());
    json["origin"].erase("y");
    expect_throwing_from<shape>(json, sn::detail::format("Cannot deserialize field 'origin' of '{}': {}", shape_name, missing_message(point_name, "y")));

    json = sn::to_njson(make_shape());
    json["size"] = "huge";
    expect_throwing_from<shape>(
        json,
        sn::detail::format("Cannot deserialize field 'size' of '{}': Cannot deserialize json value '\"huge\"' as '{}'", shape_name, sn::type_name<kind>()));

    expect_throwing_from<node>(
        nlohmann::json::parse(R"({"value": 1, "children": [{"value": 2}]})"),
        sn::detail::format("Cannot deserialize field 'children' of '{}': Cannot deserialize array element 0: {}", node_name, missing_message(node_name, "children")));

    // Errors when serializing.
    measurement value;
    value.value = std::numeric_limits<double>::infinity();
    nlohmann::json dst;
    EXPECT_FALSE(sn::try_to_njson(value, &dst));
    try {
        sn::to_njson(value, &dst);
        ADD_FAILURE() << "Expected an exception";
    } catch (const sn::exception &e) {
        EXPECT_EQ(std::string_view(e.what()),
                  sn::detail::format("Cannot serialize field 'value' of '{}': Cannot serialize 'inf' of type '{}' to json", sn::type_name<measurement>(), sn::type_name<double>()));
    }
}
