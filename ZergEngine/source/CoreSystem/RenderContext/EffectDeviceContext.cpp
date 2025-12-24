#include <ZergEngine\CoreSystem\RenderContext\EffectDeviceContext.h>
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

void EffectDeviceContext::Bind(IEffect* pEffect) noexcept
{
	assert(m_pDeviceContext != nullptr);

	IEffect* pOldEffect = m_pCurrentEffect;
	m_pCurrentEffect = pEffect;

	if (pOldEffect != pEffect)
		pOldEffect->OnUnbindFromDeviceContext();

	m_pCurrentEffect->ApplyImpl(m_pDeviceContext);
}

void EffectDeviceContext::ClearState() noexcept
{
	m_pCurrentEffect->OnUnbindFromDeviceContext();

	m_pDeviceContext->ClearState();
	m_pCurrentEffect = &g_de;
}
