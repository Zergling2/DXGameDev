#pragma once
#include "CLVItemInterface.h"

class CLVItemSpotLight : public ICLVItem
{
public:
	CLVItemSpotLight(ze::SpotLight* pLight)
		: m_pLight(pLight)
	{
	}
	virtual ~CLVItemSpotLight() = default;

	virtual void OnSelect() override;
	ze::SpotLight* GetSpotLight() const { return m_pLight; }
private:
	ze::SpotLight* m_pLight;
};
