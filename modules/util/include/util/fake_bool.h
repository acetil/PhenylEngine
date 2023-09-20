#pragma once

#include <vector>

namespace phenyl::util {
    struct FakeBool {
        bool val;
        FakeBool () : val{false} {}
        FakeBool (bool _val) : val(_val) {}
        FakeBool& operator= (bool _val) noexcept {
            val = _val;
            return *this;
        }

        operator bool () {
            return val;
        }
    };

    using BoolVector = std::vector<FakeBool>;
}