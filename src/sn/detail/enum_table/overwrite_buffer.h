#pragma once

#include <array>
#include <memory>

namespace sn::detail {

/**
 * @internal
 *
 * Small buffer for uninitialized chars.
 *
 * Constructor takes buffer size. If it's less than `small_size`, then stack storage is used. Otherwise, memory is
 * dynamically allocated.
 *
 * @tparam small_size                   Stack storage size.
 */
template<std::size_t small_size>
class overwrite_buffer {
public:
    explicit overwrite_buffer(std::size_t size) {
        if (size > small_size)
            _big = std::make_unique_for_overwrite<char[]>(size);
    }

    [[nodiscard]] char *data() {
        return _big ? _big.get() : _small.data();
    }

private:
    std::array<char, small_size> _small; // Intentionally left uninitialized.
    std::unique_ptr<char[]> _big;
};

} // namespace sn::detail
