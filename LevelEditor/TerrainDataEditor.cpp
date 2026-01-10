#include "TerrainDataEditor.h"
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>
#include <ZergEngine\CoreSystem\Helper.h>

bool TerrainDataEditor::GetModifyingTerrainSize(uint32_t* pWidth, uint32_t* pHeight)
{
	if (m_pTerrainToModify == nullptr)
		return false;

	D3D11_TEXTURE2D_DESC heightMapDesc;
	m_pTerrainToModify->GetHeightMap().GetTexture2DDesc(&heightMapDesc);

	*pWidth = static_cast<uint32_t>(heightMapDesc.Width);
	*pHeight = static_cast<uint32_t>(heightMapDesc.Height);

	return true;
}

void TerrainDataEditor::SetTerrainToModify(ze::Terrain* pTerrain)
{
	if (pTerrain == m_pTerrainToModify)
		return;

	m_pTerrainToModify = pTerrain;

	ID3D11Device* pDevice = ze::GraphicDevice::GetInstance()->GetDevice();
	ID3D11DeviceContext* pImmediateContext = ze::GraphicDevice::GetInstance()->GetImmediateContext();

	ID3D11Texture2D* pHeightMap = m_pTerrainToModify->GetHeightMap().GetTexture2D();
	D3D11_TEXTURE2D_DESC heightMapDesc;
	pHeightMap->GetDesc(&heightMapDesc);

	ID3D11Texture2D* pNormalMap = m_pTerrainToModify->GetNormalMap().GetTexture2D();
	D3D11_TEXTURE2D_DESC normalMapDesc;
	pNormalMap->GetDesc(&normalMapDesc);

	ID3D11Texture2D* pBlendMap = m_pTerrainToModify->GetBlendMap().GetTexture2D();
	D3D11_TEXTURE2D_DESC blendMapDesc;
	pBlendMap->GetDesc(&blendMapDesc);

	assert(heightMapDesc.Width == normalMapDesc.Width && heightMapDesc.Height == normalMapDesc.Height);

	HRESULT hr;

	// 높이맵 스테이징 리소스 생성 및 복사
	heightMapDesc.Usage = D3D11_USAGE_STAGING;
	heightMapDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	heightMapDesc.BindFlags = 0;
	ComPtr<ID3D11Texture2D> cpStagingHeightMap;
	hr = pDevice->CreateTexture2D(&heightMapDesc, nullptr, cpStagingHeightMap.GetAddressOf());
	if (FAILED(hr))
		ze::Debug::ForceCrashWithHRESULTMessageBox(L"Failed to create staging terrain data. ID3D11Device::CreateTexture2D()", hr);
	pImmediateContext->CopyResource(cpStagingHeightMap.Get(), pHeightMap);


	// 노말맵 스테이징 리소스 생성 및 복사
	normalMapDesc.Usage = D3D11_USAGE_STAGING;
	normalMapDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	normalMapDesc.BindFlags = 0;
	ComPtr<ID3D11Texture2D> cpStagingNormalMap;
	hr = pDevice->CreateTexture2D(&normalMapDesc, nullptr, cpStagingNormalMap.GetAddressOf());
	if (FAILED(hr))
		ze::Debug::ForceCrashWithHRESULTMessageBox(L"Failed to create staging terrain data. ID3D11Device::CreateTexture2D()", hr);
	pImmediateContext->CopyResource(cpStagingNormalMap.Get(), pNormalMap);


	// 블렌드맵 스테이징 리소스 생성 및 복사
	blendMapDesc.Usage = D3D11_USAGE_STAGING;
	blendMapDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	blendMapDesc.BindFlags = 0;
	ComPtr<ID3D11Texture2D> cpStagingBlendMap;
	hr = pDevice->CreateTexture2D(&blendMapDesc, nullptr, cpStagingBlendMap.GetAddressOf());
	if (FAILED(hr))
		ze::Debug::ForceCrashWithHRESULTMessageBox(L"Failed to create staging terrain data. ID3D11Device::CreateTexture2D()", hr);
	pImmediateContext->CopyResource(cpStagingBlendMap.Get(), pBlendMap);

	/*
	// 패치 제어점 정점 버퍼 스테이징 리소스 생성 및 복사(는 불필요. heightMap 데이터 읽어온것으로 충분)
	ID3D11Buffer* pPatchCtrlPtBuffer = m_pTerrainToModify->GetPatchControlPointVertexBuffer();
	D3D11_BUFFER_DESC patchCtrlPtBufferDesc;
	pPatchCtrlPtBuffer->GetDesc(&patchCtrlPtBufferDesc);

	patchCtrlPtBufferDesc.Usage = D3D11_USAGE_STAGING;
	patchCtrlPtBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	patchCtrlPtBufferDesc.BindFlags = 0;
	ComPtr<ID3D11Buffer> cpStagingPatchCtrlPtBuffer;
	hr = pDevice->CreateBuffer(&patchCtrlPtBufferDesc, nullptr, cpStagingPatchCtrlPtBuffer.GetAddressOf());
	if (FAILED(hr))
		ze::Debug::ForceCrashWithHRESULTMessageBox(L"Failed to create staging terrain data. ID3D11Device::CreateBuffer()", hr);
	pImmediateContext->CopyResource(cpStagingPatchCtrlPtBuffer.Get(), pPatchCtrlPtBuffer);
	*/


	
	// 지형 데이터 수정을 위해 읽어온 데이터를 D3D11 인터페이스 밖으로 빼내야 함.
	// 복사된 스테이징 텍스쳐를 읽어서 배열에 복사

	// 지형 높이맵 데이터 저장 (GPU -> CPU)
	{
		m_heights.resize(static_cast<size_t>(heightMapDesc.Width) * static_cast<size_t>(heightMapDesc.Height));

		D3D11_MAPPED_SUBRESOURCE mapped;
		hr = pImmediateContext->Map(cpStagingHeightMap.Get(), D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &mapped);
		if (FAILED(hr))
			ze::Debug::ForceCrashWithHRESULTMessageBox(L"Failed to read staging terrain data. ID3D11DeviceContext::Map()", hr);

		ze::D3D11Mapped2DSubresourceReader<uint16_t> heightMap(mapped);

		ze::IndexConverter2DTo1D ic(heightMapDesc.Width);
		for (UINT i = heightMapDesc.Height; i < heightMapDesc.Height; ++i)
			for (UINT j = heightMapDesc.Width; j < heightMapDesc.Width; ++j)
				m_heights[ic.CalcIndex(i, j)] = heightMap.GetTexel(i, j);

		// 다 읽었으면 해제
		pImmediateContext->Unmap(cpStagingHeightMap.Get(), D3D11CalcSubresource(0, 0, 0));
	}
	
	// 지형 노말맵 데이터 저장 (GPU -> CPU)
	{
		m_normals.resize(static_cast<size_t>(normalMapDesc.Width) * static_cast<size_t>(normalMapDesc.Height));

		D3D11_MAPPED_SUBRESOURCE mapped;
		hr = pImmediateContext->Map(cpStagingNormalMap.Get(), D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &mapped);
		if (FAILED(hr))
			ze::Debug::ForceCrashWithHRESULTMessageBox(L"Failed to read staging terrain data. ID3D11DeviceContext::Map()", hr);

		ze::D3D11Mapped2DSubresourceReader<ze::TerrainNormalMapFormat> normalMap(mapped);

		ze::IndexConverter2DTo1D ic(normalMapDesc.Width);
		for (UINT i = normalMapDesc.Height; i < normalMapDesc.Height; ++i)
			for (UINT j = normalMapDesc.Width; j < normalMapDesc.Width; ++j)
				m_normals[ic.CalcIndex(i, j)] = normalMap.GetTexel(i, j);

		// 다 읽었으면 해제
		pImmediateContext->Unmap(cpStagingNormalMap.Get(), D3D11CalcSubresource(0, 0, 0));
	}

	// 블렌드맵 데이터 저장 (GPU -> CPU)
	{
		m_blends.resize(static_cast<size_t>(blendMapDesc.Width) * static_cast<size_t>(blendMapDesc.Height));

		D3D11_MAPPED_SUBRESOURCE mapped;
		hr = pImmediateContext->Map(cpStagingBlendMap.Get(), D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &mapped);
		if (FAILED(hr))
			ze::Debug::ForceCrashWithHRESULTMessageBox(L"Failed to read staging terrain data. ID3D11DeviceContext::Map()", hr);

		ze::D3D11Mapped2DSubresourceReader<ze::TerrainBlendMapFormat> blendMap(mapped);

		ze::IndexConverter2DTo1D ic(blendMapDesc.Width);
		for (UINT i = blendMapDesc.Height; i < blendMapDesc.Height; ++i)
			for (UINT j = blendMapDesc.Width; j < blendMapDesc.Width; ++j)
				m_blends[ic.CalcIndex(i, j)] = blendMap.GetTexel(i, j);

		// 다 읽었으면 해제
		pImmediateContext->Unmap(cpStagingBlendMap.Get(), D3D11CalcSubresource(0, 0, 0));
	}
}
