#pragma once

namespace phenyl::util {
    template <class T, int N>
    class SmoothQueue {
        T m_total{};
        T m_values[N] = {};
        int m_pos = 0;
    public:
        T pushPop (T val) {
            T old = m_values[m_pos];
            m_total -= old;
            m_total += val;
            m_values[m_pos] = val;
            m_pos = (m_pos + 1) % N;
            return old;
        }

        decltype(m_total / N) getSmoothed () {
            return m_total / N;
        }
    };
}
