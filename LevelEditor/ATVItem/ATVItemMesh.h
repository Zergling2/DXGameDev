#pragma once

#include "ATVItemInterface.h"

class ATVItemMesh : public IATVItem
{
public:
	ATVItemMesh()
		: IATVItem(ATV_ITEM_TYPE::MESH)
	{
	}
	virtual ~ATVItemMesh() = default;

	virtual void OnSelect() override;
public:
	std::shared_ptr<ze::Mesh> m_spMesh;
};
