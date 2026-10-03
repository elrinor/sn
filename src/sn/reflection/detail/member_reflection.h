#pragma once

#include <utility>
#include <type_traits>

#include "sn/reflection/class_reflection_data.h"

#include "reflection_type_traits.h"

namespace sn::detail {

template<auto field>
struct field_getter {
    template<class Object>
    decltype(auto) operator()(const Object &object) const {
        return object.*field;
    }
};

template<auto field>
struct field_setter {
    template<class Object, class T>
    void operator()(Object &object, T &&value) const { // NOLINT: intentional non-const reference.
        object.*field = std::forward<T>(value);
    }
};

template<auto getter>
struct wrapped_getter {
    template<class Object>
    decltype(auto) operator()(const Object &object) const {
        return (object.*getter)();
    }
};

template<auto setter>
struct wrapped_setter {
    template<class Object, class T>
    void operator()(Object &object, T &&value) const { // NOLINT: intentional non-const reference.
        (object.*setter)(std::forward<T>(value));
    }
};

template<auto field, class... Tags>
    requires std::is_member_object_pointer_v<decltype(field)>
consteval auto make_field_reflection(std::string_view name, Tags... tags) {
    using field_type = member_object_pointer_field_type_t<decltype(field)>;
    return field_reflection<field_type, field_getter<field>, field_setter<field>, Tags...>(name, tags...);
}

template<auto getter, auto setter, class... Tags>
    requires std::is_member_function_pointer_v<decltype(getter)> && std::is_member_function_pointer_v<decltype(setter)>
consteval auto make_getter_setter_reflection(std::string_view name, Tags... tags) {
    using field_type = std::remove_cvref_t<member_function_pointer_result_type_t<decltype(getter)>>;
    return field_reflection<field_type, wrapped_getter<getter>, wrapped_setter<setter>, Tags...>(name, tags...);
}

template<class T>
consteval auto member_ptr_or_nullptr(const T &arg) {
    if constexpr (std::is_member_pointer_v<T>) {
        return arg;
    } else {
        return nullptr;
    }
}

template<auto targ0, auto targ1, class Arg1, class... Args>
consteval auto make_member_reflection(Arg1 arg1, Args... args) {
    if constexpr (std::is_member_object_pointer_v<decltype(targ0)>) {
        // Args in the form: (&Class::field, name, tags...).
        return make_field_reflection<targ0>(arg1, args...);
    } else {
        // Args in the form: (&Class::getter, &Class::setter, name, tags...).
        return make_getter_setter_reflection<targ0, targ1>(args...);
    }
}

} // namespace sn::detail
