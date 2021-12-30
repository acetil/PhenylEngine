#ifndef DATA_FUNCS_H
#define DATA_FUNCS_H
namespace util {
    class DataValue;
    template <typename T>
    bool fromdata (const DataValue& dataVal, T& val);
    template <typename T>
    DataValue todata (T& val);
}
#endif //DATA_FUNCS_H
