#pragma once

#include <string>
#include "util/data.h"

/*#ifndef DATA_H
namespace util {
    class DataValue;
}
#endif*/
namespace component {
    template <typename T>
    class SerialisableComponent {
    private:
        //SerialisableComponent() = default;
    public:
        util::DataValue _serialise () const {
            return static_cast<const T&>(*this).serialise();
        };
        void _deserialise (const util::DataValue& val) {
            static_cast<T&>(*this).deserialise(val);
        };
        static constexpr std::string_view const& _getName () {
            return T::getName();
        }
        friend T;
    };
}
