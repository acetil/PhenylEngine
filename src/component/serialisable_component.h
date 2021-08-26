#include <string>
#ifndef SERIALISABLE_COMPONENT_H
#define SERIALISABLE_COMPONENT_H
#ifndef DATA_H
namespace util {
    class DataValue;
}
#endif
namespace component {
    template <typename T>
    class SerialisableComponent {
    private:
        SerialisableComponent() = default;
    public:
        util::DataValue _serialise () {
            return static_cast<T&>(*this).serialise();
        };
        void _deserialise (const util::DataValue& val) {
            static_cast<T&>(*this).deserialise(val);
        };
        static const std::string& _getName () {
            return T::getName();
        }
        friend T;
    };
}

#endif
