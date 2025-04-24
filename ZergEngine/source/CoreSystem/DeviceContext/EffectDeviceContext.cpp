#include <ZergEngine\CoreSystem\DeviceContext\EffectDeviceContext.h>
#include <ZergEngine\CoreSystem\Effect\DummyEffect.h>

using namespace ze;

DummyEffect g_de;

EffectDeviceContext::EffectDeviceContext() noexcept
	: m_pDeviceContext(nullptr)
	, m_pCurrentEffect(&g_de)
{
}

EffectDeviceContext::EffectDeviceContext(ID3D11DeviceContext* pDeviceContext) noexcept
	: m_pDeviceContext(pDeviceContext)
	, m_pCurrentEffect(&g_de)
{
}

void EffectDeviceContext::AttachDeviceContext(ID3D11DeviceContext* pDeviceContext) noexcept
{
	m_pDeviceContext = pDeviceContext;
}

void EffectDeviceContext::Apply(IEffect* pEffect) noexcept
{
	assert(pEffect != nullptr);
	assert(m_pDeviceContext != nullptr);

	if (m_pCurrentEffect != pEffect)
	{
		m_pCurrentEffect->KickedFromDeviceContext();
		m_pCurrentEffect = pEffect;
	}

	m_pCurrentEffect->ApplyImpl(m_pDeviceContext);
}

void EffectDeviceContext::ClearState() noexcept
{
	m_pCurrentEffect->KickedFromDeviceContext();

	m_pDeviceContext->ClearState();
	m_pCurrentEffect = &g_de;
}
