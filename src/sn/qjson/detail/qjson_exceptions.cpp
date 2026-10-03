#include "qjson_exceptions.h"

#include <QJsonValue>

#include "sn/core/exception.h"

namespace sn::detail {

void throw_from_qjson_error(std::string_view type_name, const QJsonValue &value) {
    //throw sn::exception("Cannot deserialize json value '{}' as '{}'", value.toString())
    throw 1;
}

} // namespace sn::detail
