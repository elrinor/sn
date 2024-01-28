#pragma once

#include <array>
#include <memory>

#include "sn/detail/workaround/make_unique_for_overwrite.h"

namespace sn::detail {

template<class T, std::size_t small_size>
class small_buffer {
public:
    explicit small_buffer(std::size_t size) {
        if (size > small_size)
            _big = sn::detail::std_make_unique_for_overwrite<char[]>(size * sizeof(T));
    }

    [[nodiscard]] T *data() {
        return reinterpret_cast<T *>(_big ? _big.get() : _small.data());
    }

    [[nodiscard]] const T *data() const {
        return const_cast<small_buffer *>(this)->data();
    }

private:
    alignas(T) std::array<char, small_size * sizeof(T)> _small; // Intentionally left uninitialized.
    std::unique_ptr<char[]> _big;
};

} // namespace sn::detail
