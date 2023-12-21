#pragma once

#include <array>
#include <memory>

#include "sn/workaround/make_unique_for_overwrite.h"

namespace sn::detail {

template<class T, std::size_t small_size>
class small_buffer {
public:
    explicit small_buffer(std::size_t size) {
        if (size > _small.size())
            _big = sn::detail::std_make_unique_for_overwrite<T[]>(size);
    }

    [[nodiscard]] T *data() {
        return _big ? _big.get() : _small.data();
    }

    [[nodiscard]] const T *data() const {
        return const_cast<small_buffer *>(this)->data();
    }

private:
    std::array<T, small_size> _small; // Intentionally left uninitialized.
    std::unique_ptr<T[]> _big;
};

} // namespace sn::detail
