#pragma once

#include <type_traits>

namespace sn::detail {

template<class T>
struct member_object_pointer_field_type;

template<class Object, class Member>
struct member_object_pointer_field_type<Member(Object::*)> : std::type_identity<Member> {};

template<class T>
using member_object_pointer_field_type_t = typename member_object_pointer_field_type<T>::type;


template<class T>
struct member_function_pointer_result_type;

template<class Result, class Object, class... Args>
struct member_function_pointer_result_type<Result (Object::*)(Args...)> : std::type_identity<Result> {};

template<class Result, class Object, class... Args>
struct member_function_pointer_result_type<Result (Object::*)(Args...) const> : std::type_identity<Result> {};

template<class T>
using member_function_pointer_result_type_t = typename member_function_pointer_result_type<T>::type;

} // namespace sn::detail
