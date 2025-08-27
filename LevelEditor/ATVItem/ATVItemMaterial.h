#pragma once

#include "ATVItemInterface.h"

class ATVItemMaterial : public IATVItem
{
public:
	ATVItemMaterial(std::shared_ptr<ze::Material> spMaterial)
		: IATVItem(ATV_ITEM_TYPE::MATERIAL)
		, m_spMaterial(std::move(spMaterial))
	{
	}
	virtual ~ATVItemMaterial() = default;

	virtual void OnSelect() override;
	std::shared_ptr<ze::Material> GetMaterial() const { return m_spMaterial; }
	ze::Material* GetMaterialPtr() const { return m_spMaterial.get(); }
private:
	std::shared_ptr<ze::Material> m_spMaterial;
};
