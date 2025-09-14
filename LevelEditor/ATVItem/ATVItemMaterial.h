#pragma once

#include "ATVItemInterface.h"

namespace ze
{
	class Material;
}

class ATVItemMaterial : public IATVItem
{
public:
	ATVItemMaterial(std::shared_ptr<ze::Material> spMaterial)
		: m_spMaterial(std::move(spMaterial))
	{
	}
	virtual ~ATVItemMaterial() = default;

	virtual ATVItemType GetType() const override { return ATVItemType::Material; }
	virtual void OnSelect() override;

	std::shared_ptr<ze::Material> GetMaterial() const { return m_spMaterial; }
	ze::Material* GetMaterialPtr() const { return m_spMaterial.get(); }
private:
	std::shared_ptr<ze::Material> m_spMaterial;
};
