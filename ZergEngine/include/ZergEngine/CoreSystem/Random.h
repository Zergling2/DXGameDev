#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>
#include <chrono>

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
            std::uniform_int_distribution<T> dist(min, max);
            return dist(m_engine);
        }

        template<typename T>
        T Next()
        {
            std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
            return dist(m_engine);
        }
    private:
        std::mt19937 m_engine;
	};
}
