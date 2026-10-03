#pragma once

#include <QtCore/QJsonValue>

namespace sn::detail::builtins {

//
// QJsonValue to QByteArray support.
//

bool try_to_qbytearray(const QJsonValue &src, QByteArray *dst);

void to_qbytearray(const QJsonValue &src, QByteArray *dst);

bool try_from_qbytearray(QByteArrayView src, QJsonValue *dst);

void from_qbytearray(QByteArrayView src, QJsonValue *dst);


//
// Support for bool.
//

bool try_to_qjson(bool src, QJsonValue *dst) noexcept {
    *dst = QJsonValue(src);
    return true;
}

void to_qjson(bool src, QJsonValue *dst) {
    (void) try_to_qjson(src, dst);
}

bool try_from_qjson(const QJsonValue &src, bool *dst) noexcept {
    if (src.type() != QJsonValue::Bool)
        return false;

    *dst = src.toBool();
    return true;
}

void from_qjson(const QJsonValue &src, bool *dst) {
    if (!try_from_qjson(src, dst))
        throw 1;
}


//
//
//

} // namespace sn::detail::builtins

