#pragma once

#include "CLVItemInterface.h"

namespace ze
{
	class MeshRenderer;
}

class CLVItemMeshRenderer : public ICLVItem
{
public:
	CLVItemMeshRenderer(ze::MeshRenderer* pMeshRenderer)
		: m_pMeshRenderer(pMeshRenderer)
	{
	}
	virtual ~CLVItemMeshRenderer() = default;

	virtual void OnSelect() override;
	ze::MeshRenderer* GetMeshRenderer() const { return m_pMeshRenderer; }
private:
	ze::MeshRenderer* m_pMeshRenderer;
};
