#pragma once

#include <initializer_list>
#include <string_view>
#include <type_traits>
#include <utility>
#include <tuple>

#include "sn/core/preprocessor.h"
#include "sn/reflection/detail/reflection_type_traits.h"

namespace sn {

template<class T, class Getter, class Setter, class... Tags>
struct field_reflection {
    [[no_unique_address]] std::type_identity<T> type;
    std::string_view name;
    std::initializer_list<std::string_view> alternative_names;
    [[no_unique_address]] Getter getter;
    [[no_unique_address]] Setter setter;
    [[no_unique_address]] std::tuple<Tags...> tags;

    explicit constexpr field_reflection(std::string_view name, Tags... tags): name(name), tags(tags...) {}
};

template<class... Fields>
struct class_reflection {
    std::tuple<Fields...> fields;

    explicit constexpr class_reflection(Fields... fields) : fields(fields...) {}
};

} // namespace sn

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
struct getter_wrapper {
    template<class Object>
    decltype(auto) operator()(const Object &object) const {
        return (object.*getter)();
    }
};

template<auto setter>
struct setter_wrapper {
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
    return field_reflection<field_type, getter_wrapper<getter>, setter_wrapper<setter>, Tags...>(name, tags...);
}

template<class... Fields>
consteval auto make_class_reflection(Fields... fields) {
    return class_reflection<Fields...>(fields...);
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

// TODO(elric): #cpp23 the magic below with _class_reflection_container isn't needed in c++23, can just create a static
//              constexpr variable inside the function.

#define SN_DEFINE_CLASS_REFLECTION(T, CLASS_REFLECTION, ... /* TAGS */)                                                 \
    template<class...>                                                                                                  \
    struct _class_reflection_container;                                                                                 \
                                                                                                                        \
    template<>                                                                                                          \
    struct _class_reflection_container<T __VA_OPT__(,) __VA_ARGS__> {                                                   \
        static constexpr auto value = sn::detail::make_class_reflection SN_PP_TUPLE_TRANSFORM(_SN_DEFINE_CLASS_REFLECTION_I, CLASS_REFLECTION); \
    };                                                                                                                  \
                                                                                                                        \
    [[nodiscard]] constexpr const auto &reflect_class(std::type_identity<T> __VA_OPT__(,) __VA_ARGS__) noexcept {       \
        return _class_reflection_container<T __VA_OPT__(,) __VA_ARGS__>::value;                                         \
    }

#define _SN_DEFINE_CLASS_REFLECTION_I(FIELD_REFLECTION)                                                                 \
    _SN_DEFINE_CLASS_REFLECTION_II FIELD_REFLECTION

#define _SN_DEFINE_CLASS_REFLECTION_II(ARG0, ARG1, ...)                                                                 \
    sn::detail::make_member_reflection<ARG0, sn::detail::member_ptr_or_nullptr(ARG1)>(ARG1 __VA_OPT__(,) __VA_ARGS__)












