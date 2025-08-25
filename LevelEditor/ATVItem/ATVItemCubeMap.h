#pragma once

#include "ATVItemInterface.h"

class ATVItemCubeMap : public IATVItem
{
public:
	ATVItemCubeMap()
		: IATVItem(ATV_ITEM_TYPE::CUBEMAP)
	{
	}
	virtual ~ATVItemCubeMap() = default;
};
