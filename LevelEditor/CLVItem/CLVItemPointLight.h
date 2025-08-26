#pragma once

#include "CLVItemInterface.h"

class CLVItemPointLight : public ICLVItem
{
public:
	CLVItemPointLight(ze::PointLight* pLight)
		: m_pLight(pLight)
	{
	}
	virtual ~CLVItemPointLight() = default;

	virtual void OnSelect() override;
	ze::PointLight* GetPointLight() const { return m_pLight; }
private:
	ze::PointLight* m_pLight;
};
