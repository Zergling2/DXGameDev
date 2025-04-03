#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class TerrainData;

	class Terrain : public IComponent
	{
	public:
		Terrain();
		~Terrain() = default;
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		std::shared_ptr<TerrainData> m_spTerrainData;
	};
}
