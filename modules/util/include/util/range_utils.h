#pragma once

#include <ranges>

namespace phenyl::util {
    template <std::forward_iterator I1, std::sentinel_for<I1> S1, std::forward_iterator I2, std::sentinel_for<I2> S2, typename Comp = std::ranges::less>
    class RangeIntersection {
    private:
        using T = std::remove_cvref_t<decltype(*std::declval<I1>())>;

        I1 first1;
        S1 last1;
        I2 first2;
        S2 last2;
        Comp comp;

        struct Sentinel {

        };

        struct Iterator {
        private:
            I1 it1;
            S1 s1;
            I2 it2;
            S2 s2;
            Comp comp;

            void advanceToNext () {
                while (it1 != s1 && it2 != s2) {
                    if (comp(*it1, *it2)) {
                        ++it1;
                    } else if (comp(*it2, *it1)) {
                        ++it2;
                    } else {
                        break;
                    }
                }
            }
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using reference = const value_type&;

            Iterator () = default;
            Iterator (I1 it1, S1 s1, I2 it2, S2 s2, Comp comp = {}) : it1{std::move(it1)}, s1{std::move(s1)}, it2{std::move(it2)}, s2{std::move(s2)}, comp{std::move(comp)} {

            }

            reference operator* () const noexcept {
                return *it1;
            }

            Iterator& operator++ () {
                ++it1;
                ++it2;
                advanceToNext();
                return *this;
            }

            Iterator operator++ (int) {
                auto copy = *this;
                ++*this;
                return copy;
            }

            bool operator== (const Iterator& other) const {
                return it1 == other.it1 && it2 == other.it2;
            }

            bool operator== (const Sentinel&) const {
                return it1 == s2 || it2 == s2;
            }
        };
    public:
        RangeIntersection (I1 first1, S1 last1, I2 first2, S2 last2, Comp comp = {}) : first1{std::move(first1)}, last1{std::move(last1)}, first2{std::move(first2)}, last2{std::move(last2)}, comp{std::move(comp)} {}

        template <std::ranges::forward_range R1, std::ranges::forward_range R2>
        RangeIntersection (R1&& range1, R2&& range2, Comp comp = {}) : RangeIntersection{range1.begin(), range1.end(), range2.begin(), range2.end(), std::move(comp)} {}

        Iterator begin () const {
            return cbegin();
        }

        Iterator cbegin () const {
            return Iterator{first1, last1, first2, last2, comp};
        }

        Sentinel end () const {
            return cend();
        }

        Sentinel cend () const {
            return Sentinel{};
        }
    };

    template <std::input_iterator I1, std::sentinel_for<I1> S1, std::input_iterator I2, std::sentinel_for<I2> S2, typename Comp = std::ranges::less>
    RangeIntersection (I1 first1, S1 last1, I2 first2, S2 last2, Comp comp = {}) -> RangeIntersection<I1, S1, I2, S2, Comp>;

    template <std::ranges::forward_range R1, std::ranges::forward_range R2, typename Comp = std::ranges::less>
    RangeIntersection (R1&& range1, R2&& range2, Comp comp = {}) -> RangeIntersection<std::ranges::iterator_t<R1>, std::ranges::sentinel_t<R1>, std::ranges::iterator_t<R2>, std::ranges::sentinel_t<R2>, Comp>;
}