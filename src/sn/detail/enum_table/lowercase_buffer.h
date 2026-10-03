#pragma once

#include "overwrite_buffer.h"

namespace sn::detail {

template<class Traits>
class lowercase_buffer {
    using value_type = typename Traits::string_type::value_type;
    using string_view_type = typename Traits::string_view_type;
public:
    explicit lowercase_buffer(string_view_type s) : _buffer(Traits::to_lower_size(s)) {
        _result = Traits::to_lower(s, static_cast<value_type *>(_buffer.data()));
    }

    operator string_view_type() const {
        return _result;
    }

    string_view_type string_view() const {
        return _result;
    }

private:
    overwrite_buffer<SN_MAX_SMALL_BUFFER_SIZE> _buffer;
    string_view_type _result;
};

} // namespace sn::detail
