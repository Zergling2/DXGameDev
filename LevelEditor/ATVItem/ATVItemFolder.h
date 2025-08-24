#pragma once

#include "ATVItemInterface.h"

class ATVItemFolder : public IATVItem
{
public:
	ATVItemFolder() = default;
	virtual ~ATVItemFolder() = default;

	virtual void OnSelect() override;
};
