#pragma once

#include "CLVItemInterface.h"

namespace ze
{
	class Transform;
}

class CLVItemTransform : public ICLVItem
{
public:
	CLVItemTransform(ze::Transform* pTransform)
		: m_pTransform(pTransform)
	{
	}
	virtual ~CLVItemTransform() = default;

	virtual void OnSelect() override;
	ze::Transform* GetTransform() const { return m_pTransform; }
private:
	ze::Transform* m_pTransform;
};
