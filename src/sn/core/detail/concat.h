#pragma once

#include <string>
#include <string_view>

namespace sn::detail {

/**
 * Concatenates the passed strings, e.g. to build an error message. Note that `std::string + std::string_view` only
 * works starting with C++26.
 *
 * @param args                          Strings to concatenate, anything that converts to `std::string_view`.
 * @return                              Concatenated string.
 */
template<class... Args>
std::string concat(const Args &... args) {
    std::string result;
    result.reserve((std::string_view(args).size() + ... + 0));
    (result += ... += args);
    return result;
}

} // namespace sn::detail
