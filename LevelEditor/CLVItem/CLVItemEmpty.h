#pragma once

#include "CLVItemInterface.h"

class CLVItemEmpty : public ICLVItem
{
public:
	CLVItemEmpty() = default;
	virtual ~CLVItemEmpty() = default;

	virtual void OnSelect() override;
};
