#pragma once

#include <array>
#include <memory>

#include "sn/detail/workaround/make_unique_for_overwrite.h"

namespace sn::detail {

/**
 * @internal
 *
 * Small buffer for uninitialized data.
 *
 * Constructor takes buffer size. If it's less than `small_size`, then stack storage is used. Otherwise, memory is
 * dynamically allocated.
 *
 * @tparam small_size                   Stack storage size.
 * @tparam small_align                  Stack storage alignment.
 */
template<std::size_t small_size, std::size_t small_align = alignof(void *)>
class overwrite_buffer {
public:
    explicit overwrite_buffer(std::size_t size) {
        if (size > small_size)
            _big = sn::detail::std_make_unique_for_overwrite<char[]>(size);
    }

    [[nodiscard]] void *data() {
        return _big ? _big.get() : _small.data();
    }

    [[nodiscard]] const void *data() const {
        return const_cast<overwrite_buffer *>(this)->data();
    }

private:
    alignas(small_align) std::array<char, small_size> _small; // Intentionally left uninitialized.
    std::unique_ptr<char[]> _big;
};

} // namespace sn::detail
