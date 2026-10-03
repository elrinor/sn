#pragma once

#include <string>
#include <string_view>

#include <QtCore/QString> // NOLINT: not a C system header.

#include "sn/qstring/qstring.h"

namespace sn::detail {

struct qstring_ops {
    template<class T, class... Tags>
    [[nodiscard]] bool try_to(const T &src, std::string *dst, Tags... tags) const noexcept {
        QString qdst;
        bool result = sn::try_to_qstring(src, &qdst, tags...);
        *dst = qdst.toUtf8().toStdString();
        return result;
    }

    template<class T, class... Tags>
    void to(const T &src, std::string *dst, Tags... tags) const {
        QString qdst;
        sn::to_qstring(src, &qdst, tags...);
        *dst = qdst.toUtf8().toStdString();
    }

    template<class T, class... Tags>
    [[nodiscard]] bool try_from(std::string_view src, T *dst, Tags... tags) const noexcept {
        return sn::try_from_qstring(QString::fromUtf8(src), dst, tags...);
    }

    template<class T, class... Tags>
    void from(std::string_view src, T *dst, Tags... tags) const {
        sn::from_qstring(QString::fromUtf8(src), dst, tags...);
    }
};

} // namespace sn::detail
