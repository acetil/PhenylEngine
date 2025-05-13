#pragma once

namespace phenyl::util {
class DataValue;
// template <typename T>
// bool phenyl_from_data (const DataValue& dataVal, T& val);
// template <typename T>
// DataValue phenyl_to_data (const T& val);

DataValue phenyl_to_data (const DataValue& val);
bool phenyl_from_data (const DataValue& dataVal, DataValue& val);
} // namespace phenyl::util
