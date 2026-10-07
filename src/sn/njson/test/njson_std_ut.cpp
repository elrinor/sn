#include <array>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h> // NOLINT: not a C system header.
#include <nlohmann/json.hpp>

#include "sn/core/exception.h"
#include "sn/core/tag.h"
#include "sn/core/type_name.h"
#include "sn/detail/format/format.h"
#include "sn/njson/njson.h"

template<class T, class... Tags>
static void check_supported() {
    static_assert(sn::concepts::to_njsonable<T, Tags...>);
    static_assert(sn::concepts::try_to_njsonable<T, Tags...>);
    static_assert(sn::concepts::from_njsonable<T, Tags...>);
    static_assert(sn::concepts::try_from_njsonable<T, Tags...>);
}

template<class T, class... Tags>
static void check_unsupported() {
    static_assert(!sn::concepts::to_njsonable<T, Tags...>);
    static_assert(!sn::concepts::try_to_njsonable<T, Tags...>);
    static_assert(!sn::concepts::from_njsonable<T, Tags...>);
    static_assert(!sn::concepts::try_from_njsonable<T, Tags...>);
}

template<class T>
static void expect_valid_fromto(const nlohmann::json &json, const T &value) {
    nlohmann::json json_dst;
    EXPECT_TRUE(sn::try_to_njson(value, &json_dst));
    EXPECT_EQ(json_dst, json);
    EXPECT_EQ(sn::to_njson(value), json);

    T value_dst = T();
    EXPECT_TRUE(sn::try_from_njson(json, &value_dst)) << "with json = " << json.dump();
    EXPECT_EQ(value_dst, value) << "with json = " << json.dump();
    EXPECT_EQ(sn::from_njson<T>(json), value) << "with json = " << json.dump();
}

template<class T>
static void expect_throwing_from(const nlohmann::json &src, std::string_view message = {}) {
    T dst = T();
    EXPECT_FALSE(sn::try_from_njson(src, &dst)) << "with src = " << src.dump() << " and T = " << sn::type_name<T>();
    try {
        sn::from_njson(src, &dst);
        ADD_FAILURE() << "Expected an exception with src = " << src.dump() << " and T = " << sn::type_name<T>();
    } catch (const sn::exception &e) {
        if (!message.empty())
            EXPECT_EQ(std::string_view(e.what()), message);
    }
}

template<class T>
static void expect_throwing_to(const T &src, std::string_view message) {
    nlohmann::json dst;
    EXPECT_FALSE(sn::try_to_njson(src, &dst)) << "with T = " << sn::type_name<T>();
    try {
        sn::to_njson(src, &dst);
        ADD_FAILURE() << "Expected an exception with T = " << sn::type_name<T>();
    } catch (const sn::exception &e) {
        EXPECT_EQ(std::string_view(e.what()), message);
    }
}

static std::string from_int_message(std::string_view value) {
    return sn::detail::format("Cannot deserialize json value '{}' as '{}'", value, sn::type_name<int>());
}

static std::string infinity_message() {
    return sn::detail::format("Cannot serialize 'inf' of type '{}' to json", sn::type_name<double>());
}

namespace stdnjsonns {
struct some_tag : sn::tags::tag {};

// Type that only supports json functions with some_tag.
struct tagged {
    int value = 0;
    friend bool operator==(const tagged &, const tagged &) = default;
};

SN_DECLARE_NJSON_FUNCTIONS(tagged, some_tag)

bool try_to_njson(const tagged &src, nlohmann::json *dst, some_tag) noexcept {
    *dst = src.value;
    return true;
}

void to_njson(const tagged &src, nlohmann::json *dst, some_tag) {
    *dst = src.value;
}

bool try_from_njson(const nlohmann::json &src, tagged *dst, some_tag) noexcept {
    return sn::try_from_njson(src, &dst->value);
}

void from_njson(const nlohmann::json &src, tagged *dst, some_tag) {
    sn::from_njson(src, &dst->value);
}
} // namespace stdnjsonns

TEST(njson_std, concepts) {
    check_supported<std::vector<int>>();
    check_supported<std::vector<bool>>();
    check_supported<std::vector<std::string>>();
    check_supported<std::vector<std::vector<int>>>();
    check_supported<std::array<double, 3>>();
    check_supported<std::map<std::string, int>>();
    check_supported<std::map<int, std::vector<std::string>>>();
    check_supported<std::unordered_map<std::string, double>>();
    check_supported<std::unordered_map<long long, bool>>();
    check_supported<std::optional<int>>();
    check_supported<std::optional<std::vector<int>>>();
    check_supported<std::vector<std::optional<std::string>>>();
    check_supported<std::map<std::string, std::vector<std::optional<std::array<int, 2>>>>>();
    check_supported<nlohmann::json::array_t>();
    check_supported<nlohmann::json::object_t>();

    // Element types are checked.
    check_unsupported<std::vector<char>>();
    check_unsupported<std::vector<void *>>();
    check_unsupported<std::vector<std::set<int>>>();
    check_unsupported<std::vector<std::vector<char>>>();
    check_unsupported<std::array<char, 3>>();
    check_unsupported<std::optional<char>>();
    check_unsupported<std::optional<std::vector<char>>>();
    check_unsupported<std::map<std::string, char>>();
    check_unsupported<std::unordered_map<std::string, void *>>();

    // Map keys need to be convertible to/from string.
    check_unsupported<std::map<char, int>>();
    check_unsupported<std::map<void *, int>>();
    check_unsupported<std::map<std::vector<int>, int>>();
    check_unsupported<std::map<std::optional<int>, int>>();

    // Some containers are just not supported.
    check_unsupported<std::set<int>>();

    // Tags are passed to elements.
    check_unsupported<std::vector<stdnjsonns::tagged>>();
    check_supported<std::vector<stdnjsonns::tagged>, stdnjsonns::some_tag>();
    check_supported<std::array<stdnjsonns::tagged, 2>, stdnjsonns::some_tag>();
    check_supported<std::map<int, stdnjsonns::tagged>, stdnjsonns::some_tag>();
    check_supported<std::unordered_map<std::string, stdnjsonns::tagged>, stdnjsonns::some_tag>();
    check_supported<std::optional<stdnjsonns::tagged>, stdnjsonns::some_tag>();
    check_unsupported<std::vector<int>, stdnjsonns::some_tag>();
}

TEST(njson_std, vector) {
    expect_valid_fromto(nlohmann::json::array(), std::vector<int>());
    expect_valid_fromto(nlohmann::json::array({1, 2, 3}), std::vector<int>({1, 2, 3}));
    expect_valid_fromto(nlohmann::json::array({true, false}), std::vector<bool>({true, false}));
    expect_valid_fromto(nlohmann::json::array({"a", "b"}), std::vector<std::string>({"a", "b"}));
    expect_valid_fromto(nlohmann::json::parse("[[1], [], [2, 3]]"), std::vector<std::vector<int>>({{1}, {}, {2, 3}}));

    // Previous content is replaced.
    std::vector<int> dst = {4, 5, 6, 7};
    sn::from_njson(nlohmann::json::array({1}), &dst);
    EXPECT_EQ(dst, std::vector<int>({1}));

    expect_throwing_from<std::vector<int>>(nlohmann::json(nullptr));
    expect_throwing_from<std::vector<int>>(nlohmann::json(1));
    expect_throwing_from<std::vector<int>>(nlohmann::json::object());
    expect_throwing_from<std::vector<int>>(nlohmann::json::parse(R"({"0": 1})"));
    expect_throwing_from<std::vector<int>>(nlohmann::json::parse(R"([1, "x"])"), "Cannot deserialize array element 1: " + from_int_message("\"x\""));
    expect_throwing_from<std::vector<std::vector<int>>>(
        nlohmann::json::parse(R"([[1], [2, 2.5]])"),
        "Cannot deserialize array element 1: Cannot deserialize array element 1: " + from_int_message("2.5"));

    expect_throwing_to(std::vector<double>({1.0, std::numeric_limits<double>::infinity()}), "Cannot serialize array element 1: " + infinity_message());
}

TEST(njson_std, array) {
    expect_valid_fromto(nlohmann::json::array(), std::array<int, 0>());
    expect_valid_fromto(nlohmann::json::array({1, 2, 3}), std::array<int, 3>({1, 2, 3}));
    expect_valid_fromto(nlohmann::json::parse("[[1, 2], [3, 4]]"), std::array<std::array<int, 2>, 2>({{{1, 2}, {3, 4}}}));

    // Size should match.
    expect_throwing_from<std::array<int, 3>>(nlohmann::json::array({1, 2}));
    expect_throwing_from<std::array<int, 3>>(nlohmann::json::array({1, 2, 3, 4}));
    expect_throwing_from<std::array<int, 0>>(nlohmann::json::array({1}));

    expect_throwing_from<std::array<int, 3>>(nlohmann::json::object());
    expect_throwing_from<std::array<int, 3>>(nlohmann::json::parse(R"([1, 2, null])"), "Cannot deserialize array element 2: " + from_int_message("null"));

    expect_throwing_to(std::array<double, 1>({std::numeric_limits<double>::infinity()}), "Cannot serialize array element 0: " + infinity_message());
}

TEST(njson_std, map) {
    expect_valid_fromto(nlohmann::json::object(), std::map<std::string, int>());
    expect_valid_fromto(nlohmann::json::parse(R"({"a": 1, "b": 2})"), std::map<std::string, int>({{"a", 1}, {"b", 2}}));
    expect_valid_fromto(nlohmann::json::parse(R"({"": 1})"), std::map<std::string, int>({{"", 1}}));
    expect_valid_fromto(nlohmann::json::parse(R"({"-1": "a", "10": "b"})"), std::map<int, std::string>({{-1, "a"}, {10, "b"}}));
    expect_valid_fromto(nlohmann::json::parse(R"({"a": {"b": [1]}})"), std::map<std::string, std::map<std::string, std::vector<int>>>({{"a", {{"b", {1}}}}}));
    expect_valid_fromto(nlohmann::json::parse(R"({"true": 1})"), std::map<bool, int>({{true, 1}}));

    // Previous content is replaced.
    std::map<std::string, int> dst = {{"x", 1}};
    sn::from_njson(nlohmann::json::parse(R"({"y": 2})"), &dst);
    EXPECT_EQ(dst, (std::map<std::string, int>({{"y", 2}})));

    expect_throwing_from<std::map<std::string, int>>(nlohmann::json(nullptr));
    expect_throwing_from<std::map<std::string, int>>(nlohmann::json::array());
    expect_throwing_from<std::map<std::string, int>>(nlohmann::json::parse(R"([["a", 1]])"));
    expect_throwing_from<std::map<std::string, int>>(nlohmann::json::parse(R"({"a": 1, "b": "2"})"), "Cannot deserialize object member 'b': " + from_int_message("\"2\""));

    // Keys go through sn::from_string, so the message for keys comes from there.
    expect_throwing_from<std::map<int, int>>(nlohmann::json::parse(R"({"x": 1})"));
    expect_throwing_from<std::map<int, int>>(nlohmann::json::parse(R"({" 1": 1})"));
    try {
        (void) sn::from_njson<std::map<int, int>>(nlohmann::json::parse(R"({"x": 1})"));
    } catch (const sn::exception &e) {
        EXPECT_TRUE(std::string_view(e.what()).starts_with("Cannot deserialize object member 'x': ")) << e.what();
    }

    // Different strings that map to the same key.
    expect_throwing_from<std::map<int, int>>(nlohmann::json::parse(R"({"1": 1, "01": 2})"), "Cannot deserialize object member '1': another member maps to the same key");

    expect_throwing_to(std::map<std::string, double>({{"a", 1.0}, {"b", std::numeric_limits<double>::infinity()}}), "Cannot serialize object member 'b': " + infinity_message());
}

TEST(njson_std, unordered_map) {
    expect_valid_fromto(nlohmann::json::object(), std::unordered_map<std::string, int>());
    expect_valid_fromto(nlohmann::json::parse(R"({"a": 1, "b": 2})"), std::unordered_map<std::string, int>({{"a", 1}, {"b", 2}}));
    expect_valid_fromto(nlohmann::json::parse(R"({"1": [true]})"), std::unordered_map<unsigned, std::vector<bool>>({{1, {true}}}));

    expect_throwing_from<std::unordered_map<std::string, int>>(nlohmann::json::array());
    expect_throwing_from<std::unordered_map<std::string, int>>(nlohmann::json::parse(R"({"a": null})"), "Cannot deserialize object member 'a': " + from_int_message("null"));
    expect_throwing_from<std::unordered_map<int, int>>(nlohmann::json::parse(R"({"1": 1, "01": 2})"));
}

TEST(njson_std, optional) {
    expect_valid_fromto(nlohmann::json(nullptr), std::optional<int>());
    expect_valid_fromto(nlohmann::json(1), std::optional<int>(1));
    expect_valid_fromto(nlohmann::json("a"), std::optional<std::string>("a"));
    expect_valid_fromto(nlohmann::json::array({1, 2}), std::optional<std::vector<int>>({1, 2}));
    expect_valid_fromto(nlohmann::json::parse("[1, null, 3]"), std::vector<std::optional<int>>({1, std::nullopt, 3}));
    expect_valid_fromto(nlohmann::json::parse(R"({"a": null})"), std::map<std::string, std::optional<int>>({{"a", std::nullopt}}));

    // Null resets the optional.
    std::optional<int> dst = 1;
    sn::from_njson(nlohmann::json(nullptr), &dst);
    EXPECT_EQ(dst, std::nullopt);

    expect_throwing_from<std::optional<int>>(nlohmann::json("1"), from_int_message("\"1\""));
    expect_throwing_from<std::optional<int>>(nlohmann::json::array());

    expect_throwing_to(std::optional<double>(std::numeric_limits<double>::infinity()), infinity_message());
}

TEST(njson_std, tags) {
    using stdnjsonns::tagged;
    stdnjsonns::some_tag tag;

    EXPECT_EQ(sn::to_njson(std::vector<tagged>({{1}, {2}}), tag), nlohmann::json::array({1, 2}));
    EXPECT_EQ(sn::from_njson<std::vector<tagged>>(nlohmann::json::array({1, 2}), tag), std::vector<tagged>({{1}, {2}}));
    EXPECT_EQ(sn::to_njson(std::array<tagged, 1>({{{1}}}), tag), nlohmann::json::array({1}));
    EXPECT_EQ(sn::to_njson(std::map<int, tagged>({{1, {2}}}), tag), nlohmann::json::parse(R"({"1": 2})"));
    EXPECT_EQ((sn::from_njson<std::map<int, tagged>>(nlohmann::json::parse(R"({"1": 2})"), tag)), (std::map<int, tagged>({{1, {2}}})));
    EXPECT_EQ(sn::to_njson(std::optional<tagged>(tagged{3}), tag), nlohmann::json(3));
    EXPECT_EQ(sn::from_njson<std::optional<tagged>>(nlohmann::json(3), tag), std::optional<tagged>(tagged{3}));
}
