#include <ZergEngine\CoreSystem\Resource\TerrainData.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>

using namespace ze;

constexpr uint32_t CELLS_PER_TERRAIN_PATCH = 64;

TerrainData::TerrainData()
	: m_heightmapScale(1.0f, 1.0f, 1.0f)
	, m_uvScale(32.0f)
	, m_spDiffusemap{}
	, m_spNormalmap{}
	, m_spBlendmap()
	, m_heightmapWidth(0)
	, m_heightmapHeight(0)
	, m_patchCtrlPtCountRow(0)
	, m_patchCtrlPtCountCol(0)
	, m_patchCtrlPtIndexCount(0)
	, m_heightData()
	, m_cpHeightmapSRV(nullptr)
	, m_cpPatchCtrlPtBuffer(nullptr)
	, m_cpPatchCtrlPtIndexBuffer(nullptr)
{
}

/*
bool TerrainData::SetHeightmap(const std::shared_ptr<Texture2D> heightmap)
{
	if (m_spHeightmap == nullptr)
		return false;

	if (m_spHeightmap == heightmap)
		return true;

	// 텍스쳐가 높이맵으로 쓰일 수 있는지 검증
	D3D11_TEXTURE2D_DESC descHeightmap;
	heightmap->GetTexDesc(&descHeightmap);

	if (descHeightmap.Format != DXGI_FORMAT_R16_FLOAT ||
		descHeightmap.BindFlags != D3D11_BIND_SHADER_RESOURCE ||
		descHeightmap.Usage != D3D11_USAGE_IMMUTABLE)		// 정적 용도로 생성된 텍스쳐만 지원
		return false;

	// 가로 세로가 각각 64의 배수 + 1인지 체크
	if (descHeightmap.Width < 65 || (descHeightmap.Width & 0b111111) != 1 ||
		descHeightmap.Height < 65 || (descHeightmap.Height & 0b111111) != 1)
		return false;

	// 높이맵으로 사용 가능하면 텍스쳐 및 디스크립터 저장
	m_spHeightmap = heightmap;
	m_descHeightmap = descHeightmap;

	m_patchCtrlPtCountRow = (m_descHeightmap.Height - 1) / CELLS_PER_TERRAIN_PATCH + 1;
	m_patchCtrlPtCountCol = (m_descHeightmap.Width - 1) / CELLS_PER_TERRAIN_PATCH + 1;
	m_patchCtrlPtIndexCount = (m_patchCtrlPtCountRow - 1) * (m_patchCtrlPtCountCol - 1) * 4;	// 패치당 인덱스 4개 (사각 패치)

	// 높이맵 데이터를 시스템 메모리에 생성
	D3D11_TEXTURE2D_DESC descSysMemHeightmap = descHeightmap;
	descSysMemHeightmap.Usage = D3D11_USAGE_STAGING;
	descSysMemHeightmap.BindFlags = 0;
	descSysMemHeightmap.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	ComPtr<ID3D11Texture2D> cpSysMemHeightmap;
	HRESULT hr = GraphicDevice::GetInstance().GetDeviceComInterface()->CreateTexture2D(&descSysMemHeightmap, nullptr, cpSysMemHeightmap.GetAddressOf());
	if (FAILED(hr))
		return false;

	// 스테이징 텍스처로 높이맵 복사
	GraphicDevice::GetInstance().GetImmContextComInterface()->CopyResource(cpSysMemHeightmap.Get(), m_spHeightmap->GetTex2DComInterface());
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = GraphicDevice::GetInstance().GetImmContextComInterface()->Map(cpSysMemHeightmap.Get(), D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ,
		0, &mapped);
	if (FAILED(hr))
		return false;

	switch (descHeightmap.Format)
	{
	case DXGI_FORMAT_R16_FLOAT:
		{
			// 텍스처 데이터를 읽기 (16비트 부동소수점 -> 32비트 부동소수점으로 변환)
			const size_t itemCount = descHeightmap.Width * descHeightmap.Height;
			m_heightData.clear();
			m_heightData.resize(itemCount);
			const PackedVector::HALF* pHalf = reinterpret_cast<PackedVector::HALF*>(mapped.pData);
			PackedVector::XMConvertHalfToFloatStream(m_heightData.data(), sizeof(float), pHalf, sizeof(PackedVector::HALF), itemCount);
			for (size_t i = 0; i < itemCount; ++i)
				m_heightData[i] *= m_heightmapScale.y;
		}
		break;
	case DXGI_FORMAT_R16_UNORM:
		break;
	case DXGI_FORMAT_R16_UINT:
		break;
	default:
		break;
	}

	GraphicDevice::GetInstance().GetImmContextComInterface()->Unmap(cpSysMemHeightmap.Get(), D3D11CalcSubresource(0, 0, 0));

	// 물리 콜라이더 생성
	// ...
}
*/