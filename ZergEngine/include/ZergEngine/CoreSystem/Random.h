#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Random
	{
    public:
        Random()
            : m_engine()
        {
            SetSeed(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        }
        explicit Random(uint32_t seed)
        {
            SetSeed(seed);
        }

        void SetSeed(uint32_t seed) { m_engine.seed(seed); }
        
        template<typename T>
        T Next(T min, T max)
        {
            static_assert(std::is_integral<T>::value, "Next<T>() function only supports integer type!");
            std::uniform_int_distribution<T> dist(min, max);
            return dist(m_engine);
        }

        template<typename T>
        T Next()
        {
            static_assert(std::is_integral<T>::value, "Next<T>() function only supports integer type!");
            std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
            return dist(m_engine);
        }
    private:
        std::mt19937 m_engine;
	};
}
