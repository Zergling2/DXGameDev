#pragma once

#include "ATVItemInterface.h"

namespace ze
{
	class StaticMesh;
}

class ATVItemStaticMesh : public IATVItem
{
public:
	ATVItemStaticMesh() = default;
	virtual ~ATVItemStaticMesh() = default;

	virtual ATVItemType GetType() const override { return ATVItemType::StaticMesh; }
	virtual void OnSelect() override;
public:
	std::shared_ptr<ze::StaticMesh> m_spMesh;
};
