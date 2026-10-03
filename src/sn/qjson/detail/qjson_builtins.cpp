#include "qjson_builtins.h"

namespace sn::detail::builtins {

#if 0
bool try_to_qbytearray(const QJsonValue &src, QByteArray *dst) {
    switch (src.type()) {
    case QJsonValue::Null:
        *dst = "null";
        break;
    case QJsonValue::Bool:
        *dst = src.toBool() ? "true" : "false";
        break;
    case QJsonValue::Double:
    {
        double d = value.toDouble();
        if (qIsFinite(d))
        {
            *outTarget = QByteArray::number(value.toDouble(), 'g', 17);
        }
        else
        {
            *outTarget = "null"; //< +INF || -INF || NaN (see RFC4627#section2.4)
        }
        break;
    }
    case QJsonValue::String:
    {
        // We convert a string via QJsonDocument because escaping the special characters is not
        // that easy.
        QJsonArray array;
        array.push_back(value);
        QByteArray result = QJsonDocument(array).toJson(QJsonDocument::Compact);
        *outTarget = result.mid(1, result.size() - 2);
        break;
    }
    case QJsonValue::Array:
        *outTarget = QJsonDocument(value.toArray()).toJson(format);
        break;
    case QJsonValue::Object:
        *outTarget = QJsonDocument(value.toObject()).toJson(format);
        break;
    case QJsonValue::Undefined:
    default:
        outTarget->clear();
    }
}

void to_qbytearray(const QJsonValue &src, QByteArray *dst) {

}

bool try_from_qbytearray(QByteArrayView src, QJsonValue *dst) {

}

void from_qbytearray(QByteArrayView src, QJsonValue *dst) {

}
#endif

} // namespace sn::detail::builtins
