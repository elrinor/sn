#include <map>
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
#include "sn/njson/enum_njson.h"
#include "sn/reflection/enum_reflection.h"
#include "sn/string/enum_string.h"

namespace enumnjsonns {

struct short_names : sn::tags::tag {};

enum class color {
    RED = 1,
    GREEN = 2,
    BLUE = 3,
    UNNAMED = 42,
};
using enum color;

SN_DEFINE_ENUM_REFLECTION(color, ({
    {RED, "red"},
    {GREEN, "green"},
    {BLUE, "Blue"},
}))
SN_DEFINE_ENUM_NJSON_FUNCTIONS(color, sn::case_sensitive)

SN_DEFINE_ENUM_REFLECTION(color, ({
    {RED, "r"},
    {GREEN, "g"},
    {BLUE, "b"},
}), short_names)
SN_DEFINE_ENUM_NJSON_FUNCTIONS(color, sn::case_sensitive, short_names)

enum class shape {
    CIRCLE,
    SQUARE,
};
using enum shape;

SN_DEFINE_ENUM_REFLECTION(shape, ({
    {CIRCLE, "Circle"},
    {SQUARE, "Square"},
}))
SN_DEFINE_INLINE_ENUM_NJSON_FUNCTIONS(shape, sn::case_insensitive)

// Enum with both string and json functions in the same namespace.
enum class direction {
    LEFT,
    RIGHT,
};
using enum direction;

SN_DEFINE_ENUM_REFLECTION(direction, ({
    {LEFT, "left"},
    {RIGHT, "right"},
}))
SN_DEFINE_ENUM_STRING_FUNCTIONS(direction, sn::case_sensitive)
SN_DEFINE_STATIC_ENUM_NJSON_FUNCTIONS(direction, sn::case_sensitive)

enum class byte_sized : unsigned char {
    BYTE_VALUE = 1,
    BYTE_UNNAMED = 200,
};
using enum byte_sized;

SN_DEFINE_ENUM_REFLECTION(byte_sized, ({
    {BYTE_VALUE, "value"},
}))
SN_DEFINE_ENUM_NJSON_FUNCTIONS(byte_sized, sn::case_sensitive)

} // namespace enumnjsonns

using namespace enumnjsonns; // NOLINT

template<class T, class... Tags>
static void expect_valid_fromto(const nlohmann::json &json, T value, Tags... tags) {
    nlohmann::json json_dst;
    EXPECT_TRUE(sn::try_to_njson(value, &json_dst, tags...));
    EXPECT_EQ(json_dst, json);
    EXPECT_EQ(sn::to_njson(value, tags...), json);

    T value_dst = T();
    EXPECT_TRUE(sn::try_from_njson(json, &value_dst, tags...)) << "with json = " << json.dump();
    EXPECT_EQ(value_dst, value) << "with json = " << json.dump();
    EXPECT_EQ(sn::from_njson<T>(json, tags...), value) << "with json = " << json.dump();
}

template<class T, class... Tags>
static void expect_throwing_from(const nlohmann::json &src, Tags... tags) {
    T dst = T();
    EXPECT_FALSE(sn::try_from_njson(src, &dst, tags...)) << "with src = " << src.dump();

    std::string expected = sn::detail::format("Cannot deserialize json value '{}' as '{}'", src.dump(), sn::type_name<T>());
    try {
        sn::from_njson(src, &dst, tags...);
        ADD_FAILURE() << "Expected an exception with src = " << src.dump();
    } catch (const sn::exception &e) {
        EXPECT_EQ(std::string_view(e.what()), expected);
    }
}

template<class T>
static void expect_throwing_to(T src, std::string_view value) {
    nlohmann::json dst;
    EXPECT_FALSE(sn::try_to_njson(src, &dst));

    std::string expected = sn::detail::format("Cannot serialize '{}' of type '{}' to json", value, sn::type_name<T>());
    try {
        sn::to_njson(src, &dst);
        ADD_FAILURE() << "Expected an exception";
    } catch (const sn::exception &e) {
        EXPECT_EQ(std::string_view(e.what()), expected);
    }
}

TEST(njson_enum, basic) {
    expect_valid_fromto(nlohmann::json("red"), RED);
    expect_valid_fromto(nlohmann::json("green"), GREEN);
    expect_valid_fromto(nlohmann::json("Blue"), BLUE);

    expect_throwing_from<color>(nlohmann::json("RED"));
    expect_throwing_from<color>(nlohmann::json("blue"));
    expect_throwing_from<color>(nlohmann::json(" red"));
    expect_throwing_from<color>(nlohmann::json(""));
    expect_throwing_from<color>(nlohmann::json("1"));
    expect_throwing_from<color>(nlohmann::json(1));
    expect_throwing_from<color>(nlohmann::json(nullptr));
    expect_throwing_from<color>(nlohmann::json::array({"red"}));

    expect_throwing_to(UNNAMED, "42");
}

TEST(njson_enum, tags) {
    static_assert(sn::concepts::to_njsonable<color>);
    static_assert(sn::concepts::to_njsonable<color, short_names>);
    static_assert(!sn::concepts::to_njsonable<shape, short_names>);

    expect_valid_fromto(nlohmann::json("r"), RED, short_names());
    expect_valid_fromto(nlohmann::json("b"), BLUE, short_names());
    expect_throwing_from<color>(nlohmann::json("red"), short_names());
}

TEST(njson_enum, case_insensitive) {
    expect_valid_fromto(nlohmann::json("Circle"), CIRCLE);
    expect_valid_fromto(nlohmann::json("Square"), SQUARE);

    EXPECT_EQ(sn::from_njson<shape>(nlohmann::json("circle")), CIRCLE);
    EXPECT_EQ(sn::from_njson<shape>(nlohmann::json("SQUARE")), SQUARE);
    expect_throwing_from<shape>(nlohmann::json("triangle"));
}

TEST(njson_enum, with_string_functions) {
    expect_valid_fromto(nlohmann::json("left"), LEFT);
    EXPECT_EQ(sn::to_string(RIGHT), "right");
    EXPECT_EQ(sn::from_string<direction>("right"), RIGHT);
}

TEST(njson_enum, small_underlying_type) {
    expect_valid_fromto(nlohmann::json("value"), BYTE_VALUE);
    expect_throwing_to(BYTE_UNNAMED, "200");
}

TEST(njson_enum, containers) {
    expect_valid_fromto(nlohmann::json::array({"red", "Blue"}), std::vector<color>({RED, BLUE}));
    expect_valid_fromto(nlohmann::json::array({"r", "b"}), std::vector<color>({RED, BLUE}), short_names());

    // Enum map keys need string functions.
    static_assert(!sn::concepts::to_njsonable<std::map<color, int>>);
    using direction_map = std::map<direction, int>;
    expect_valid_fromto(nlohmann::json::parse(R"({"left": 1, "right": 2})"), direction_map({{LEFT, 1}, {RIGHT, 2}}));
    EXPECT_ANY_THROW((void) sn::from_njson<direction_map>(nlohmann::json::parse(R"({"up": 1})")));
}
