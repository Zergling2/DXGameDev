#include <ZergEngine\CoreSystem\Effect\BillboardEffect.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

constexpr size_t PS_INDEX_NO_LIGHTING = 0;


void BillboardEffect::Init()
{
	ID3D11Device* pDevice = GraphicDevice::GetInstance()->GetDeviceComInterface();



	m_cbPerBillboard.Init(pDevice);
}

void BillboardEffect::Release()
{
	m_cbPerBillboard.Release();
}
