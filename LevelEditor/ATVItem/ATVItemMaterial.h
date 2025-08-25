#pragma once

#include "ATVItemInterface.h"

class ATVItemMaterial : public IATVItem
{
public:
	ATVItemMaterial()
		: IATVItem(ATV_ITEM_TYPE::MATERIAL)
	{
	}
	virtual ~ATVItemMaterial() = default;

	virtual void OnSelect() override;
public:
	std::shared_ptr<ze::Material> m_spMaterial;
};
