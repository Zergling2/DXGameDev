#pragma once

#include "ATVItemInterface.h"

class ATVItemFolder : public IATVItem
{
public:
	ATVItemFolder() = default;
	virtual ~ATVItemFolder() = default;

	virtual ATVItemType GetType() const override { return ATVItemType::Folder; }
	virtual void OnSelect() override;
};
