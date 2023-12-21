#include "qstring_builtins.h"

#include "qstring_exceptions.h"

namespace sn::builtins {

//
// bool.
//

bool try_to_qstring(const bool &src, QString *dst) noexcept {
    *dst = src ? QStringLiteral("true") : QStringLiteral("false");
    return true;
}

bool try_from_qstring(QStringView src, bool *dst) noexcept {
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

void to_qstring(const bool &src, QString *dst) {
    (void) try_to_qstring(src, dst); // Always succeeds.
}

void from_qstring(QStringView src, bool *dst) {
    if (!try_from_qstring(src, dst))
        throw_from_string_error<bool>(src);
}

} // namespace sn::builtins
