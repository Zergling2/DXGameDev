#pragma once

#include "ATVItemInterface.h"

class ATVItemEmpty : public IATVItem
{
public:
	ATVItemEmpty()
		: IATVItem(ATV_ITEM_TYPE::EMPTY)
	{
	}
	virtual ~ATVItemEmpty() = default;

	virtual void OnSelect() override;
};
