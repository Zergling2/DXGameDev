#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class GameObject;
	class TerrainData;

	class Terrain : public IComponent
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::TERRAIN;
		static constexpr bool IsCreatable() { return true; }

		Terrain();
		~Terrain() = default;
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		std::shared_ptr<TerrainData> m_spTerrainData;
	};
}
