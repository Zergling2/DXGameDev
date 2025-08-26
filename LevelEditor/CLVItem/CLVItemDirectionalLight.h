#pragma once

#include "CLVItemInterface.h"

class CLVItemDirectionalLight : public ICLVItem
{
public:
	CLVItemDirectionalLight(ze::DirectionalLight* pLight)
		: m_pLight(pLight)
	{
	}
	virtual ~CLVItemDirectionalLight() = default;

	virtual void OnSelect() override;
	ze::DirectionalLight* GetDirectionalLight() const { return m_pLight; }
private:
	ze::DirectionalLight* m_pLight;
};
