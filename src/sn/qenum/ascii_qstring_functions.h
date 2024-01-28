#pragma once

#include <QtCore/QChar>
#include <QtCore/QStringView>

#include "sn/detail/ascii/ascii_functions.h"

namespace sn::detail {

[[nodiscard]] constexpr QChar to_lower_ascii(QChar c) noexcept {
    return QChar(to_lower_ascii(c.unicode()));
}

inline QStringView to_lower_ascii(QStringView src, QChar *buffer) noexcept {
    const QChar *src_ptr = src.data();
    const QChar *src_end = src.data() + src.size();
    QChar *dst_ptr = buffer;

    while (src_ptr < src_end)
        *dst_ptr++ = to_lower_ascii(*src_ptr++);

    return {buffer, src.size()};
}

} // namespace sn::detail
