#pragma once

#include "framework.h"

template<typename _T>
void SafeReleaseComPtr(_T& comptr)
{
	if (comptr != nullptr)
	{
		comptr->Release();
		comptr = nullptr;
	}
}
