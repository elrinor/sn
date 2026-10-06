#pragma once

#include <string_view>

#include "sn/detail/ascii/ascii_functions.h"

#include "overwrite_buffer.h"

namespace sn::detail {

class lowercase_buffer {
public:
    explicit lowercase_buffer(std::string_view s) : _buffer(s.size()) {
        _result = to_lower_ascii(s, _buffer.data());
    }

    operator std::string_view() const {
        return _result;
    }

private:
    overwrite_buffer<SN_MAX_SMALL_BUFFER_SIZE> _buffer;
    std::string_view _result;
};

} // namespace sn::detail
