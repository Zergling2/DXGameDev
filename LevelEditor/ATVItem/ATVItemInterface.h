#pragma once

#include "..\framework.h"

class IATVItem abstract
{
public:
	IATVItem() = default;
	virtual ~IATVItem() = default;

	virtual void OnSelect() = 0;
};
