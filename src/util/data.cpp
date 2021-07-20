#include "data.h"

using namespace util;

DataObject::DataObject () = default;

DataObject::DataObject (DataValue& val) {
    DataObject obj;
    if (val.getValue(obj)) {
        values = std::move(obj.values);
    }
}

Map<std::string, DataValue>::iterator DataObject::begin () {
    return values.begin();
}

Map<std::string, DataValue>::iterator DataObject::end () {
    return values.end();
}

Map<std::string, DataValue>::const_iterator DataObject::cbegin () {
    return values.cbegin();
}

Map<std::string, DataValue>::const_iterator DataObject::cend () {
    return values.cend();
}