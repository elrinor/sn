#pragma once

#include <cstddef>
#include <type_traits>
#include <memory>

namespace sn {

// TODO(elric): #cpp20 AppleClang 14 doesn't have std::make_unique_for_overwrite<T[]>, drop this once we upgrade.
template<class Array>
auto make_unique_for_overwrite(std::size_t size) {
    static_assert(std::is_unbounded_array_v<Array>, "The specified template argument must be an unbounded array type");
    return std::unique_ptr<Array>(new std::remove_extent_t<Array>[size]);
}

} // namespace sn
