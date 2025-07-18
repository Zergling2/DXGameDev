#pragma once

#include "CLVItemInterface.h"

class CLVItemMeshRenderer : public ICLVItem
{
public:
	CLVItemMeshRenderer(ze::MeshRenderer* pMeshRenderer)
		: m_pMeshRenderer(pMeshRenderer)
	{
	}
	virtual ~CLVItemMeshRenderer() = default;

	virtual void OnSelect() override;
private:
	ze::MeshRenderer* m_pMeshRenderer;
};
