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

DataArray::DataArray (std::vector<DataValue>& _values) {
    values = _values;
}
DataArray::DataArray (std::vector<DataValue>&& _values) {
    values = std::move(_values);
}

std::size_t DataArray::size () {
    return values.size();
}

DataValue& DataArray::operator[] (std::size_t index) {
    return values[index];
}

bool DataArray::empty () {
    return values.empty();
}

DataArray::iterator DataArray::begin () {
    return values.begin();
}
DataArray::iterator DataArray::end () {
    return values.end();
}

DataArray::const_iterator DataArray::cbegin () {
    return values.cbegin();
}

DataArray::const_iterator DataArray::cend () {
    return values.cend();
}

void DataArray::clear () {
    values.clear();
}

DataArray::iterator DataArray::erase (const_iterator pos) {
    return values.erase(pos);
}

void DataArray::pop_back () {
    values.pop_back();
}