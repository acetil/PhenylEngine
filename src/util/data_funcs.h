#ifndef DATA_FUNCS_H
#define DATA_FUNCS_H
namespace util {
    template <typename L>
    class DataValueImpl;
    template <typename T, typename L>
    bool fromdata (DataValueImpl<L>& dataVal, T& val);
    template <typename T, typename L>
    DataValueImpl<L> todata (T& val);
}
#endif //DATA_FUNCS_H
