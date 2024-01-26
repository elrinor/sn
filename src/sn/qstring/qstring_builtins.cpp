#include "qstring_builtins.h"

#include "qstring_exceptions.h"

namespace sn::builtins {

//
// bool.
//

bool try_to_qstring(const bool &src, QString *dst, tn::detail::explicit_type_tag) noexcept {
    *dst = src ? QStringLiteral("true") : QStringLiteral("false");
    return true;
}

bool try_from_qstring(QStringView src, bool *dst, tn::detail::explicit_type_tag) noexcept {
    if (src == QStringLiteral("true") || src == QStringLiteral("1")) {
        *dst = true;
        return true;
    } else if (src == QStringLiteral("false") || src == QStringLiteral("0")) {
        *dst = false;
        return true;
    } else {
        return false;
    }
}

void to_qstring(const bool &src, QString *dst, tn::detail::explicit_type_tag) {
    (void) try_to_qstring(src, dst); // Always succeeds.
}

void from_qstring(QStringView src, bool *dst, tn::detail::explicit_type_tag) {
    if (!try_from_qstring(src, dst))
        throw_from_string_error<bool>(src);
}

} // namespace sn::builtins
