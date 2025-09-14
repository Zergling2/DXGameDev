#pragma once

#include "ATVItemInterface.h"

namespace ze
{
	class Mesh;
}

class ATVItemMesh : public IATVItem
{
public:
	ATVItemMesh() = default;
	virtual ~ATVItemMesh() = default;

	virtual ATVItemType GetType() const override { return ATVItemType::Mesh; }
	virtual void OnSelect() override;
public:
	std::shared_ptr<ze::Mesh> m_spMesh;
};
