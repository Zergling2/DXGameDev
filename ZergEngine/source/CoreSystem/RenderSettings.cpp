#include <ZergEngine\CoreSystem\RenderSettings.h>

using namespace ze;

RenderSettings* RenderSettings::s_pInstance = nullptr;

RenderSettings::RenderSettings()
	: m_skyboxCubeMap()
{
}

RenderSettings::~RenderSettings()
{
}

void RenderSettings::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new RenderSettings();
}

void RenderSettings::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void RenderSettings::Init()
{
}

void RenderSettings::UnInit()
{
	m_skyboxCubeMap.Reset();
}
