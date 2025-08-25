#pragma once

#include "ATVItemInterface.h"

class ATVItemFolder : public IATVItem
{
public:
	ATVItemFolder()
		: IATVItem(ATV_ITEM_TYPE::FOLDER)
	{
	}
	virtual ~ATVItemFolder() = default;

	virtual void OnSelect() override;
};
