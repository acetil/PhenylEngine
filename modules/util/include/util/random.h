#pragma once

#include <concepts>
#include <cstdint>

#include <random>

namespace phenyl::util {
    template <typename T>
    concept RandomRangeType = requires (const T& t, float f) {
        { t * f } -> std::convertible_to<T>;
        { t - t } -> std::convertible_to<T>;
        { t + t } -> std::convertible_to<T>;
        requires !std::integral<T>;
        requires !std::floating_point<T>;
    };

    class Random {
    private:
        static Random* INSTANCE;

        std::mt19937 random;

        static Random* GetInstance () {
            if (!INSTANCE) {
                INSTANCE = new Random(std::random_device{});
            }

            return INSTANCE;
        }

        explicit Random (std::random_device rd) : random{rd()} {}

        void seed (std::uint32_t seedVal) {
            random.seed(seedVal);
        }

        template <std::integral T>
        T rand () {
            return std::uniform_int_distribution<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max())(random);
        }

        template <std::integral T>
        T rand (T min, T max) {
            return std::uniform_int_distribution<T>(min, max)(random);
        }

        template <std::floating_point T>
        T rand () {
            return std::uniform_real_distribution<T>(0, 1)(random);
        }

        template <std::floating_point T>
        T rand (T min, T max) {
            return std::uniform_real_distribution<T>(min, max)(random);
        }

        template <RandomRangeType T>
        T rand (const T& min, const T& max) {
            return (max - min) * rand<float>() + min;
        }
    public:
        static void Seed (std::uint32_t seed) {
            GetInstance()->seed(seed);
        }

        static void Cleanup () {
            delete INSTANCE;
        }

        template <typename T>
        static T Rand () {
            return GetInstance()->rand<T>();
        }

        template <typename T>
        static T Rand (T min, T max) {
            return GetInstance()->rand(min, max);
        }
    };
}