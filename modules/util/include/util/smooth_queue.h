#pragma once

namespace phenyl::util {
    template <class T, int N>
    class SmoothQueue {
        T totalVal{};
        T values[N] = {};
        int pos = 0;
    public:
        T pushPop (T val) {
            T old = values[pos];
            totalVal -= old;
            totalVal += val;
            values[pos] = val;
            pos = (pos + 1) % N;
            return old;
        }

        decltype(totalVal / N) getSmoothed () {
            return totalVal / N;
        }
    };
}
