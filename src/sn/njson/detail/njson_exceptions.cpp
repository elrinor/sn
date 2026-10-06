#include "njson_exceptions.h"

#include <string>

#include <nlohmann/json.hpp>

#include "sn/core/exception.h"

namespace sn::detail {

static std::string to_short_json_string(const nlohmann::json &value) {
    // Escape everything that's not ASCII, so that we can cut the string anywhere. Invalid UTF-8 is replaced, we
    // don't want to throw a different exception from here.
    std::string result = value.dump(-1, ' ', true, nlohmann::json::error_handler_t::replace);

    constexpr std::size_t max_size = 64;
    if (result.size() > max_size) {
        result.resize(max_size - 3);
        result += "...";
    }
    return result;
}

void throw_to_njson_error(std::string_view type_name, std::string_view value) {
    throw sn::exception("Cannot serialize '{}' of type '{}' to json", value, type_name);
}

void throw_from_njson_error(std::string_view type_name, const nlohmann::json &value) {
    throw sn::exception("Cannot deserialize json value '{}' as '{}'", to_short_json_string(value), type_name);
}

void throw_element_to_njson_error(std::size_t index, std::string_view reason) {
    throw sn::exception("Cannot serialize array element {}: {}", index, reason);
}

void throw_element_from_njson_error(std::size_t index, std::string_view reason) {
    throw sn::exception("Cannot deserialize array element {}: {}", index, reason);
}

void throw_member_to_njson_error(std::string_view key, std::string_view reason) {
    throw sn::exception("Cannot serialize object member '{}': {}", key, reason);
}

void throw_member_from_njson_error(std::string_view key, std::string_view reason) {
    throw sn::exception("Cannot deserialize object member '{}': {}", key, reason);
}

void throw_field_to_njson_error(std::string_view type_name, std::string_view field_name, std::string_view reason) {
    throw sn::exception("Cannot serialize field '{}' of '{}': {}", field_name, type_name, reason);
}

void throw_field_from_njson_error(std::string_view type_name, std::string_view field_name, std::string_view reason) {
    throw sn::exception("Cannot deserialize field '{}' of '{}': {}", field_name, type_name, reason);
}

void throw_missing_field_from_njson_error(std::string_view type_name, std::string_view field_name) {
    throw_field_from_njson_error(type_name, field_name, "json object doesn't have it");
}

} // namespace sn::detail
