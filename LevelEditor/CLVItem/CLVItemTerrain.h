#pragma once

#include "CLVItemInterface.h"

namespace ze
{
	class Terrain;
}

class CLVItemTerrain : public ICLVItem
{
public:
	CLVItemTerrain(ze::Terrain* pTerrain)
		: m_pTerrain(pTerrain)
	{
	}
	virtual ~CLVItemTerrain() = default;

	virtual void OnSelect() override;
	ze::Terrain* GetTerrain() const { return m_pTerrain; }
private:
	ze::Terrain* m_pTerrain;
};
