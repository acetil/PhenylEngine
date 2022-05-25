#pragma once

namespace util {
    class DataValue;
    template <typename T>
    bool fromdata (const DataValue& dataVal, T& val);
    template <typename T>
    DataValue todata (const T& val);

    DataValue todata (const DataValue& val);
    bool fromdata (const DataValue& dataVal, DataValue& val);
}

