#pragma once

#include "..\framework.h"

class IALVItem abstract
{
public:
	IALVItem() = default;
	virtual ~IALVItem() = default;

	virtual void OnSelect() = 0;
};
