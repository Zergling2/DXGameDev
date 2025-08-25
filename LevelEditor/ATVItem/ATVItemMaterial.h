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
	
	ze::Material& GetMaterial() { return m_material; }
	const ze::Material& GetMaterial() const { return m_material; }
private:
	ze::Material m_material;
};
