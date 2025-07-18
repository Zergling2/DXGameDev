#pragma once

#include "..\framework.h"

class IHTVItem abstract
{
public:
	IHTVItem() = default;
	virtual ~IHTVItem() = default;

	virtual void OnSelect() {}
};
