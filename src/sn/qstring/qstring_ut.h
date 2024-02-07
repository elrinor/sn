#pragma once

#include <ostream>

#include <QtCore/QString>

#include "sn/detail/test/test_suite.h"

#include "qstring.h"

namespace sn::detail {
template<>
struct test_stringifier<QString> {
    QString operator()(std::string_view s) const {
        return QString::fromUtf8(s);
    }
};

template<class T>
struct test_serializer<T, QString> {
    template<class... Tags>
    [[nodiscard]] bool try_to(const T &src, QString *dst, Tags... tags) noexcept {
        return sn::try_to_qstring(src, dst, tags...);
    }

    template<class... Tags>
    [[nodiscard]] QString to(const T &src, Tags... tags) {
        return sn::to_qstring(src, tags...);
    }

    template<class... Tags>
    [[nodiscard]] bool try_from(QStringView src, T *dst, Tags... tags) noexcept {
        return sn::try_from_qstring(src, dst, tags...);
    }

    template<class... Tags>
    [[nodiscard]] T from(QStringView src, Tags... tags) {
        return sn::from_qstring<T>(src, tags...);
    }
};
} // namespace sn::detail

// GTest integration. The functions are static and won't escape TU, but will be found via ADL.
// We need both functions b/c operator<< is used in test case comments, and PrintTo is used for printing invalid values.
static std::ostream &operator<<(std::ostream &os, const QString &s) {
    return os << s.toUtf8().toStdString();
}
static void PrintTo(const QString& s, std::ostream* os) {
    *os << s.toUtf8().toStdString();
}
