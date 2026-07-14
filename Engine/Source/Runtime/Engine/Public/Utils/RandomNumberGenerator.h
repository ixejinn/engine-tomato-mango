#ifndef MANGO_RANDOMNUMBERGENERATOR_H
#define MANGO_RANDOMNUMBERGENERATOR_H

#include <random>

namespace tomato
{
    class RandomNumberGenerator
    {
    public:
        /// [min, max]
        template<std::integral T = int>
        static T GetUniformIntDistribution(T min, T max)
        {
            std::uniform_int_distribution<T> dis(min, max);
            return dis(gen_);
        }

        /// [min, max)
        template<std::floating_point T = float>
        static T GetUniformRealDistribution(T min, T max)
        {
            std::uniform_real_distribution<T> dis(min, max);
            return dis(gen_);
        }

    private:
        inline static thread_local std::random_device rd_;
        inline static thread_local std::mt19937 gen_{rd_()};
    };
}

#endif //MANGO_RANDOMNUMBERGENERATOR_H
