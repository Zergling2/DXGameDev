#include "TerrainDataEditor.h"

void TerrainDataEditor::SetTerrainToModify(ze::Terrain* pTerrain)
{
	if (pTerrain == m_pTerrain)
		return;

	ID3D11Device* pDevice = ze::GraphicDevice::GetInstance()->GetDeviceComInterface();
	ID3D11DeviceContext* pDeviceContext = ze::GraphicDevice::GetInstance()->GetImmediateContextComInterface();

	HRESULT hr;
	D3D11_TEXTURE2D_DESC heightMapDesc;
	D3D11_TEXTURE2D_DESC normalMapDesc;
	D3D11_TEXTURE2D_DESC blendMapDesc;
	D3D11_BUFFER_DESC patchCtrlPtBufferDesc;

	ID3D11Texture2D* pHeightMap = pTerrain->GetHeightMap().GetTex2DComInterface();
	ID3D11Texture2D* pNormalMap = pTerrain->GetNormalMap().GetTex2DComInterface();
	ID3D11Texture2D* pBlendMap = pTerrain->GetBlendMap().GetTex2DComInterface();
	ID3D11Buffer* pPatchCtrlPtBuffer = pTerrain->GetPatchControlPointBufferComInterface();

	pHeightMap->GetDesc(&heightMapDesc);
	pNormalMap->GetDesc(&normalMapDesc);
	pBlendMap->GetDesc(&blendMapDesc);
	pPatchCtrlPtBuffer->GetDesc(&patchCtrlPtBufferDesc);

	heightMapDesc.Usage = D3D11_USAGE_STAGING;
	heightMapDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	heightMapDesc.BindFlags = 0;
	ComPtr<ID3D11Texture2D> cpStagingHeightMap;
	hr = pDevice->CreateTexture2D(&heightMapDesc, nullptr, cpStagingHeightMap.GetAddressOf());
	if (FAILED(hr))
		;

	normalMapDesc.Usage = D3D11_USAGE_STAGING;
	normalMapDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	normalMapDesc.BindFlags = 0;
	ComPtr<ID3D11Texture2D> cpStagingNormalMap;
	hr = pDevice->CreateTexture2D(&normalMapDesc, nullptr, cpStagingNormalMap.GetAddressOf());
	if (FAILED(hr))
		;

	blendMapDesc.Usage = D3D11_USAGE_STAGING;
	blendMapDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	blendMapDesc.BindFlags = 0;
	ComPtr<ID3D11Texture2D> cpStagingBlendMap;
	hr = pDevice->CreateTexture2D(&blendMapDesc, nullptr, cpStagingBlendMap.GetAddressOf());
	if (FAILED(hr))
		;

	patchCtrlPtBufferDesc.Usage = D3D11_USAGE_STAGING;
	patchCtrlPtBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	patchCtrlPtBufferDesc.BindFlags = 0;
	ComPtr<ID3D11Buffer> cpStagingPatchCtrlPtBuffer;
	hr = pDevice->CreateBuffer(&patchCtrlPtBufferDesc, nullptr, cpStagingPatchCtrlPtBuffer.GetAddressOf());
	if (FAILED(hr))
		;

	pDeviceContext->CopyResource(cpStagingHeightMap.Get(), pHeightMap);
	pDeviceContext->CopyResource(cpStagingNormalMap.Get(), pNormalMap);
	pDeviceContext->CopyResource(cpStagingBlendMap.Get(), pBlendMap);
	pDeviceContext->CopyResource(cpStagingPatchCtrlPtBuffer.Get(), pPatchCtrlPtBuffer);

	m_cpStagingHeightMap = std::move(cpStagingHeightMap);
	m_cpStagingNormalMap = std::move(cpStagingNormalMap);
	m_cpStagingBlendMap = std::move(cpStagingBlendMap);
	m_cpStagingPatchCtrlPtBuffer = std::move(cpStagingPatchCtrlPtBuffer);
}
