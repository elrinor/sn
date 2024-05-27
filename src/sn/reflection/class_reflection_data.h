#pragma once

#include <tuple>
#include <string_view>
#include <type_traits>

#include "sn/core/tags.h"

namespace sn {

/**
 * Structure describing a reflected field.
 */
template<class T, class Getter, class Setter, sn::concepts::tag... Tags>
struct field_reflection {
    [[no_unique_address]] std::type_identity<T> type;
    std::string_view name;
    std::initializer_list<std::string_view> alternative_names;
    [[no_unique_address]] Getter getter;
    [[no_unique_address]] Setter setter;
    [[no_unique_address]] std::tuple<Tags...> tags;

    explicit constexpr field_reflection(std::string_view name, Tags... tags): name(name), tags(tags...) {}
};

/**
 * Structure describing a reflected class.
 */
template<class... Fields>
struct class_reflection {
    std::tuple<Fields...> fields;

    explicit constexpr class_reflection(Fields... fields) : fields(fields...) {}
};

} // namespace sn
