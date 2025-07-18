#pragma once

#include "HTVItemInterface.h"

// 컴포넌트 리스트 뷰 및 컴포넌트 인스펙터 뷰 정리 객체
class HTVItemEmpty : public IHTVItem
{
public:
	HTVItemEmpty() = default;
	virtual ~HTVItemEmpty() = default;

	virtual void OnSelect();
};
