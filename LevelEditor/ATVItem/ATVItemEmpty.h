#pragma once

#include "ATVItemInterface.h"

class ATVItemEmpty : public IATVItem
{
public:
	ATVItemEmpty() = default;
	virtual ~ATVItemEmpty() = default;

	virtual ATVItemType GetType() const override { return ATVItemType::Empty; }
	virtual void OnSelect() override;
};
