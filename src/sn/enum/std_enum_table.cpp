#include "std_enum_table.h"

#include <cassert>
#include <array>
#include <memory>

#include "sn/workaround/make_unique_for_overwrite.h"

namespace sn::detail {

static constexpr std::size_t default_small_buffer_size = 128;

template<class Char, std::size_t small_size>
struct small_buffer {
    std::array<Char, small_size> small;
    std::unique_ptr<Char[]> big;

    [[nodiscard]] Char *allocate(std::size_t size) {
        if (size <= small.size()) {
            return small.data();
        } else {
            big = sn::detail::std_make_unique_for_overwrite<Char[]>(size);
            return big.get();
        }
    }

    [[nodiscard]] const Char *data() const {
        return big ? big.get() : small.data();
    }
};

template<class Char>
static inline Char to_lower_ascii(Char c) {
    return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
}

template<class Char>
static inline std::basic_string_view<Char> to_lower_ascii(std::basic_string_view<Char> src, Char *buffer) {
    const Char *src_ptr = src.data();
    const Char *src_end = src.data() + src.size();
    Char *dst_ptr = buffer;

    while (src_ptr < src_end)
        *dst_ptr++ = to_lower_ascii(*src_ptr++);

    return {buffer, src.size()};
}

template<class Char, class Map>
static inline bool do_try_to_string(std::uint64_t src, std::basic_string_view<Char> *dst, const Map &map) noexcept {
    auto pos = map.find(src);
    if (pos == map.end())
        return false;
    *dst = pos->second;
    return true;
}

template<case_sensitivity mode, class Char, class Map>
static inline bool do_try_from_string(std::basic_string_view<Char> src, std::uint64_t *dst, const Map &map) noexcept {
    auto run = [&] (std::basic_string_view<Char> src, std::uint64_t *dst) {
        auto pos = map.find(src);
        if (pos == map.end())
            return false;
        *dst = pos->second;
        return true;
    };

    if constexpr (mode == case_insensitive) {
        small_buffer<Char, default_small_buffer_size> buffer;
        return run(to_lower_ascii(src, buffer.allocate(src.size())), dst);
    } else {
        return run(src, dst);
    }
}

bool universal_std_enum_table::try_to_string(std::uint64_t src, std::string *dst) const noexcept {
    std::string_view tmp;
    if (do_try_to_string(src, &tmp, _string_by_enum)) {
        *dst = tmp;
        return true;
    }
    return false;
}

bool universal_std_enum_table::try_from_string(std::string_view src, std::uint64_t *dst) const noexcept {
    assert(_mode == case_sensitive);
    return do_try_from_string<case_sensitive>(src, dst, _enum_by_string);
}

bool universal_std_enum_table::try_from_string_ci(std::string_view src, std::uint64_t *dst) const noexcept {
    assert(_mode == case_insensitive);
    return do_try_from_string<case_insensitive>(src, dst, _enum_by_string);
}

void universal_std_enum_table::insert(std::uint64_t value, std::string_view name) {
    if (!_string_by_enum.contains(value))
        _string_by_enum.emplace(value, name);

    if (_mode == case_sensitive) {
        assert(!_enum_by_string.contains(name));
        _enum_by_string.emplace(name, value);
    } else {
        std::string lower_name(name.size(), '\0');
        to_lower_ascii(name, lower_name.data());
        assert(!_enum_by_string.contains(lower_name));
        _enum_by_string.emplace(std::move(lower_name), value);
    }
}

} // namespace sn::detail
