#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\Helper.h>

using namespace ze;

constexpr float DEFAULT_TILING_SCALE = 64.0f;

Terrain::Terrain(GameObject& owner)
	: IComponent(owner, TerrainManager::GetInstance()->AssignUniqueId())
	, m_cellSize(0.0f)
	, m_heightScale(0.0f)
	, m_tilingScale(DEFAULT_TILING_SCALE)
	, m_patchCtrlPtIndexCount(0)
	, m_heightMap()
	, m_normalMap()
	, m_diffuseMapLayer()
	, m_normalMapLayer()
	, m_blendMap()
	, m_cpPatchCtrlPtBuffer(nullptr)
	, m_cpPatchCtrlPtIndexBuffer(nullptr)
	, m_heightData()
{
}

float Terrain::GetMaxHeight() const
{
	return m_heightScale * static_cast<float>((std::numeric_limits<uint16_t>::max)());
}

IComponentManager* Terrain::GetComponentManager() const
{
	return TerrainManager::GetInstance();
}

bool Terrain::SetHeightMap(Texture2D heightMap, float cellSize, float heightScale)
{
	ID3D11Texture2D* pTex2D = heightMap.GetTexture2D();
	if (!pTex2D)
		return false;

	if (cellSize < 0.1f || cellSize > 1.0f)
		return false;

	if (heightScale < 0.001f || heightScale > 1.0f)
		return false;

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// ¿äÃ»µÈ ÅØ½ºÃÄ°¡ ³ôÀÌ¸ÊÀ¸·Î »ç¿ëµÉ ¼ö ÀÖ´ÂÁö °Ë»ç
	D3D11_TEXTURE2D_DESC $heightMapDesc;
	pTex2D->GetDesc(&$heightMapDesc);

	const D3D11_TEXTURE2D_DESC& heightMapDesc = $heightMapDesc;
	// ÅØ½ºÃÄ Æ÷¸ËÀÌ DXGI_FORMAT_R16_UNORM Å¸ÀÔÀÎÁö È®ÀÎ
	if (heightMapDesc.Format != DXGI_FORMAT_R16_UNORM)	// => UNORM Å¸ÀÔ (Á¤¼ö¸¦ ÀúÀåÇÏÁö¸¸ ¼ÎÀÌ´õ¿¡¼­ Sample ½Ã 0.0 ~ 1.0À¸·Î Á¤±ÔÈ­ Á¢±Ù °¡´ÉÇÑ Æ÷¸Ë)
		return false;

	// D3D11_BIND_SHADER_RESOURCE ÇÃ·¡±×·Î »ý¼ºµÇ¾ú´ÂÁö È®ÀÎ
	if (!(heightMapDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE))
		return false;

	// Usage È®ÀÎ
	if (heightMapDesc.Usage != D3D11_USAGE_DEFAULT)
		return false;

	if (heightMapDesc.CPUAccessFlags != 0)
		return false;

	if (heightMapDesc.SampleDesc.Count != 1)
		return false;

	// ³ôÀÌ¸Ê ³Êºñ¿Í ³ôÀÌ°¡ 64ÀÇ ¹è¼ö + 1ÀÎÁö È®ÀÎ
	if (heightMapDesc.Width < CELLS_PER_TERRAIN_PATCH + 1 || heightMapDesc.Width % CELLS_PER_TERRAIN_PATCH != 1 ||
		heightMapDesc.Height < CELLS_PER_TERRAIN_PATCH + 1 || heightMapDesc.Height % CELLS_PER_TERRAIN_PATCH != 1)
		return false;
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬




	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	uint32_t patchCtrlPtCountRow = (heightMapDesc.Height - 1) / CELLS_PER_TERRAIN_PATCH + 1;
	uint32_t patchCtrlPtCountCol = (heightMapDesc.Width - 1) / CELLS_PER_TERRAIN_PATCH + 1;
	uint32_t patchCtrlPtIndexCount = (patchCtrlPtCountRow - 1) * (patchCtrlPtCountCol - 1) * 4;	// ÆÐÄ¡´ç ÀÎµ¦½º 4°³ (»ç°¢ ÆÐÄ¡)
	std::vector<float> heightData(heightMapDesc.Width * heightMapDesc.Height);

	const float terrainSizeAlongX = static_cast<float>((patchCtrlPtCountCol - 1) * CELLS_PER_TERRAIN_PATCH) * cellSize;
	const float terrainSizeAlongZ = static_cast<float>((patchCtrlPtCountRow - 1) * CELLS_PER_TERRAIN_PATCH) * cellSize;
	const float maxHeight = heightScale * static_cast<float>((std::numeric_limits<uint16_t>::max)());

	ID3D11Device* pDevice = GraphicDevice::GetInstance()->GetDevice();
	ID3D11DeviceContext* pImmediateContext = GraphicDevice::GetInstance()->GetImmediateContext();
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// Step 1. ÁöÇü Á¦¾îÁ¡ ¹öÆÛ Àç»ý¼º & ³ôÀÌ µ¥ÀÌÅÍ ½Ã½ºÅÛ ¸Þ¸ð¸®¿¡ ÀúÀå
	IndexConverter2DTo1D aic;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initialData;
	HRESULT hr = S_OK;
	std::vector<VFTerrainPatchControlPoint> patchCtrlPts(patchCtrlPtCountRow * patchCtrlPtCountCol);
	ComPtr<ID3D11Texture2D> cpStagingHeightMapTexture2D;

	D3D11_TEXTURE2D_DESC heightMapForCPUReadDesc = heightMapDesc;
	heightMapForCPUReadDesc.Usage = D3D11_USAGE_STAGING;	// CPU¿¡¼­ ÀÐ±â À§ÇØ¼­ STAGING ÅØ½ºÃÄ »ý¼º
	heightMapForCPUReadDesc.BindFlags = 0;
	heightMapForCPUReadDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	hr = pDevice->CreateTexture2D(&heightMapForCPUReadDesc, nullptr, cpStagingHeightMapTexture2D.GetAddressOf());
	if (FAILED(hr))
		return false;

	// ³ôÀÌ¸Ê ÅØ½ºÃÄ µ¥ÀÌÅÍ¸¦ ÀÐ±â À§ÇØ ½ºÅ×ÀÌÂ¡ ÅØ½ºÃÄ·Î º¹»ç
	pImmediateContext->CopyResource(cpStagingHeightMapTexture2D.Get(), heightMap.GetTexture2D());

	// º¹»çµÈ ½ºÅ×ÀÌÂ¡ ÅØ½ºÃÄ¸¦ ÀÐ±â
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = pImmediateContext->Map(cpStagingHeightMapTexture2D.Get(), D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &mapped);
	if (FAILED(hr))
		return false;

	const float tuc = (1.0f / static_cast<float>(heightMapDesc.Width)) / 2.0f;	// ÅØ½ºÃÄ ÁÂÇ¥ ¿ÀÂ÷ º¸Á¤¿ë (ÅØ¼¿ÀÇ Áß¾Ó °¡¸®Å°±â À§ÇØ¼­)
	const float tvc = (1.0f / static_cast<float>(heightMapDesc.Height)) / 2.0f;	// ÅØ½ºÃÄ ÁÂÇ¥ ¿ÀÂ÷ º¸Á¤¿ë (ÅØ¼¿ÀÇ Áß¾Ó °¡¸®Å°±â À§ÇØ¼­)
	const float terrainHalfSizeX = static_cast<float>(heightMapDesc.Width - 1) * cellSize / 2.0f;
	const float terrainHalfSizeZ = static_cast<float>(heightMapDesc.Height - 1) * cellSize / 2.0f;
	D3D11Mapped2DSubresourceReader<uint16_t> heightMapData(mapped);

	aic.SetColumnSize(heightMapDesc.Width);
	for (uint32_t i = 0; i < patchCtrlPtCountRow; ++i)
	{
		for (uint32_t j = 0; j < patchCtrlPtCountCol; ++j)
		{
			VFTerrainPatchControlPoint& v = patchCtrlPts[i * patchCtrlPtCountCol + j];
			const uint16_t data = heightMapData.GetTexel(i * CELLS_PER_TERRAIN_PATCH, j * CELLS_PER_TERRAIN_PATCH);
			v.m_position.x = static_cast<float>(j * CELLS_PER_TERRAIN_PATCH) * cellSize - terrainHalfSizeX;
			v.m_position.y = static_cast<float>(data) / static_cast<float>((std::numeric_limits<uint16_t>::max)()) * maxHeight;
			v.m_position.z = -static_cast<float>(i * CELLS_PER_TERRAIN_PATCH) * cellSize + terrainHalfSizeZ;

			v.m_texCoord.x = static_cast<float>(j * CELLS_PER_TERRAIN_PATCH) / static_cast<float>(heightMapDesc.Width) + tuc;
			v.m_texCoord.y = static_cast<float>(i * CELLS_PER_TERRAIN_PATCH) / static_cast<float>(heightMapDesc.Height) + tvc;

			// i¿Í j°¡ °¡Àå ¾Æ·¡, °¡Àå ¿À¸¥ÂÊ ¿§Áö À§Ä¡¸¦ °¡¸®Å°´Â °æ¿ì¿¡´Â ¹Ù¿îµå °è»êÀ» ÇÏ¸é ¾ÈµÈ´Ù. (Index out of range)
			if (i < patchCtrlPtCountRow - 1 && j < patchCtrlPtCountCol - 1)
			{
				// ¹Ù¿îµå °è»ê (65 x 65 °Ë»ö) ¹× ³ôÀÌ µ¥ÀÌÅÍ ½Ã½ºÅÛ ¸Þ¸ð¸®¿¡ ÀúÀå
				const uint32_t ke = i * CELLS_PER_TERRAIN_PATCH + 65;
				const uint32_t le = j * CELLS_PER_TERRAIN_PATCH + 65;
				uint16_t min = (std::numeric_limits<uint16_t>::max)();
				uint16_t max = (std::numeric_limits<uint16_t>::min)();
				
				for (uint32_t k = i * CELLS_PER_TERRAIN_PATCH; k < ke; ++k)
				{
					for (uint32_t l = j * CELLS_PER_TERRAIN_PATCH; l < le; ++l)
					{
						const uint16_t data = heightMapData.GetTexel(k, l);

						// 1. ½Ã½ºÅÛ ¸Þ¸ð¸®¿¡ ³ôÀÌ µ¥ÀÌÅÍ ÀúÀå
						const float normalizedHeight = static_cast<float>(data) / static_cast<float>((std::numeric_limits<uint16_t>::max)());
						const float realHeight = normalizedHeight * maxHeight;
						heightData[aic.CalcIndex(k, l)] = realHeight;

						// 2. ¹Ù¿îµå °è»ê
						if (data < min)
							min = data;
						if (data > max)
							max = data;
					}
				}

				v.m_boundsY.x = static_cast<float>(min) / static_cast<float>((std::numeric_limits<uint16_t>::max)()) * maxHeight;
				v.m_boundsY.y = static_cast<float>(max) / static_cast<float>((std::numeric_limits<uint16_t>::max)()) * maxHeight;
				if (v.m_boundsY.y - v.m_boundsY.x < 1.0f)	// ÆÐÄ¡°¡ Æò¸é¿¡ °¡±î¿ï °æ¿ì ÇÁ·¯½ºÅÒ ÄÃ¸µÀÌ ¾î·Á¿ï ¼ö ÀÖÀ¸¹Ç·Î ¹Ù¿îµù º¼·ýÀÇ µÎ²²¸¦ Áõ°¡½ÃÅ²´Ù.
				{
					v.m_boundsY.x -= 0.5f;
					v.m_boundsY.y += 0.5f;
				}
			}
			else
			{
				v.m_boundsY = XMFLOAT2(0.0f, 0.0f);
			}
		}
	}

	for (size_t i = 0; i < heightData.size(); ++i)
	{
		if (heightData[i] < 0.0f)
			*reinterpret_cast<int*>(0) = 0;
	}

	// ´Ù ÀÐ¾úÀ¸¸é ÇØÁ¦
	pImmediateContext->Unmap(cpStagingHeightMapTexture2D.Get(), D3D11CalcSubresource(0, 0, 0));

	// ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(VFTerrainPatchControlPoint) * patchCtrlPts.size());
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = static_cast<UINT>(sizeof(VFTerrainPatchControlPoint));

	// ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = patchCtrlPts.data();
	initialData.SysMemPitch = 0;	// ÅØ½ºÃÄ ¸®¼Ò½º¿¡¼­¸¸ À¯È¿ÇÑ °ª
	initialData.SysMemSlicePitch = 0;

	ComPtr<ID3D11Buffer> cpPatchCtrlPtBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &initialData, cpPatchCtrlPtBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// Step 2. ÁöÇü Á¦¾îÁ¡ ÀÎµ¦½º ¹öÆÛ »ý¼º
	std::vector<uint32_t> indexBuffer(patchCtrlPtIndexCount);

	uint32_t k = 0;
	// ÆÐÄ¡ °³¼ö¸¸Å­ ¹Ýº¹
	const uint32_t patchCountRow = patchCtrlPtCountRow - 1;
	const uint32_t patchCountCol = patchCtrlPtCountCol - 1;
	aic.SetColumnSize(patchCtrlPtCountCol);
	for (uint32_t i = 0; i < patchCountRow; ++i)
	{
		for (uint32_t j = 0; j < patchCountCol; ++j)
		{
			// [0]  [1]
			//
			// [2]  [3]
			indexBuffer[k] = static_cast<uint32_t>(aic.CalcIndex(i, j));
			indexBuffer[k + 1] = static_cast<uint32_t>(aic.CalcIndex(i, j + 1));
			indexBuffer[k + 2] = static_cast<uint32_t>(aic.CalcIndex(i + 1, j));
			indexBuffer[k + 3] = static_cast<uint32_t>(aic.CalcIndex(i + 1, j + 1));
			k += 4;
		}
	}

	// ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * patchCtrlPtIndexCount);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;	// ÀÎµ¦½º ¹öÆÛ´Â º¯°æµÉ ÀÏÀÌ ¾øÀ½
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = indexBuffer.data();
	initialData.SysMemPitch = 0;
	initialData.SysMemSlicePitch = 0;

	ComPtr<ID3D11Buffer> cpPatchCtrlPtIndexBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &initialData, cpPatchCtrlPtIndexBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// Step 3. ³ë¸» ¸Ê »ý¼º (¿ÜÀû ¼ø¼­ ÁÖÀÇÇÏ¸ç °è»ê)
	aic.SetColumnSize(heightMapDesc.Width);
	const float* pHeightData = heightData.data();
	XMFLOAT3A tangent;
	XMFLOAT3A bitangent;
	std::vector<TerrainNormalMapFormat> terrainNormals(heightMapDesc.Width * heightMapDesc.Height);	// R16G16B16A16_FLOAT
	// 1. LEFT TOP ¡æ x ¡é
	tangent.x = cellSize;
	tangent.y = pHeightData[aic.CalcIndex(0, 1)] - pHeightData[aic.CalcIndex(0, 0)];
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.y = pHeightData[aic.CalcIndex(1, 0)] - pHeightData[aic.CalcIndex(0, 0)];
	bitangent.z = -cellSize;
	XMStoreHalf4(&terrainNormals[aic.CalcIndex(0, 0)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	// 2. RIGHT TOP ¡é x ¡ç
	tangent.x = -cellSize;
	tangent.y = pHeightData[aic.CalcIndex(0, heightMapDesc.Width - 2)] - pHeightData[aic.CalcIndex(0, heightMapDesc.Width - 1)];
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.y = pHeightData[aic.CalcIndex(1, heightMapDesc.Width - 1)] - pHeightData[aic.CalcIndex(0, heightMapDesc.Width - 1)];
	bitangent.z = -cellSize;
	XMStoreHalf4(&terrainNormals[aic.CalcIndex(0, heightMapDesc.Width - 1)], XMVector3Cross(XMLoadFloat3A(&bitangent), XMLoadFloat3A(&tangent)));
	// 3. LEFT BOTTOM ¡è x ¡æ
	tangent.x = cellSize;
	tangent.y = pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, 1)] - pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, 0)];
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.y = pHeightData[aic.CalcIndex(heightMapDesc.Height - 2, 0)] - pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, 0)];
	bitangent.z = cellSize;
	XMStoreHalf4(&terrainNormals[aic.CalcIndex(heightMapDesc.Height - 1, 0)], XMVector3Cross(XMLoadFloat3A(&bitangent), XMLoadFloat3A(&tangent)));
	// 4. RIGHT BOTTOM ¡ç x ¡è
	tangent.x = -cellSize;
	tangent.y = pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, heightMapDesc.Width - 2)] - pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, heightMapDesc.Width - 1)];
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.y = pHeightData[aic.CalcIndex(heightMapDesc.Height - 2, heightMapDesc.Width - 1)] - pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, heightMapDesc.Width - 1)];
	bitangent.z = cellSize;
	XMStoreHalf4(&terrainNormals[aic.CalcIndex(heightMapDesc.Height - 1, heightMapDesc.Width - 1)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	// 5. LEFT EDGE ¡æ x ¡é2
	tangent.x = cellSize;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = cellSize * -2.0f;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Height - 1); ++i)
	{
		tangent.y = pHeightData[aic.CalcIndex(i, 1)] - pHeightData[aic.CalcIndex(i, 0)];
		bitangent.y = pHeightData[aic.CalcIndex(i + 1, 0)] - pHeightData[aic.CalcIndex(i - 1, 0)];
		XMStoreHalf4(&terrainNormals[aic.CalcIndex(i, 0)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	}
	// 6. TOP EDGE ¡æ2 x ¡é
	tangent.x = cellSize * 2.0f;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = -cellSize;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Width - 1); ++i)
	{
		tangent.y = pHeightData[aic.CalcIndex(0, i + 1)] - pHeightData[aic.CalcIndex(0, i - 1)];
		bitangent.y = pHeightData[aic.CalcIndex(1, i)] - pHeightData[aic.CalcIndex(0, i)];
		XMStoreHalf4(&terrainNormals[aic.CalcIndex(0, i)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	}
	// 7. RIGHT EDGE ¡ç x ¡è2
	tangent.x = -cellSize;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = cellSize * 2.0f;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Height - 1); ++i)
	{
		tangent.y = pHeightData[aic.CalcIndex(i, heightMapDesc.Width - 2)] - pHeightData[aic.CalcIndex(i, heightMapDesc.Width - 1)];
		bitangent.y = pHeightData[aic.CalcIndex(i - 1, heightMapDesc.Width - 1)] - pHeightData[aic.CalcIndex(i + 1, heightMapDesc.Width - 1)];
		XMStoreHalf4(&terrainNormals[aic.CalcIndex(i, heightMapDesc.Width - 1)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	}
	// 8. BOTTOM EDGE ¡ç2 x ¡è
	tangent.x = cellSize * -2.0f;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = cellSize;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Width - 1); ++i)
	{
		tangent.y = pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, i - 1)] - pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, i + 1)];
		bitangent.y = pHeightData[aic.CalcIndex(heightMapDesc.Height - 2, i)] - pHeightData[aic.CalcIndex(heightMapDesc.Height - 1, i)];
		XMStoreHalf4(&terrainNormals[aic.CalcIndex(heightMapDesc.Height - 1, i)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	}
	// 9. ELSE ¡æ2 x ¡é2
	tangent.x = cellSize * 2.0f;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = cellSize * -2.0f;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Height - 1); ++i)
	{
		for (uint32_t j = 1; j < static_cast<uint32_t>(heightMapDesc.Width - 1); ++j)
		{
			tangent.y = pHeightData[aic.CalcIndex(i, j + 1)] - pHeightData[aic.CalcIndex(i, j - 1)];
			bitangent.y = pHeightData[aic.CalcIndex(i + 1, j)] - pHeightData[aic.CalcIndex(i - 1, j)];
			XMStoreHalf4(&terrainNormals[aic.CalcIndex(i, j)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
		}
	}

	// ¸¶Áö¸·À¸·Î ³ë¸»À» ¸ðµÎ Á¤±ÔÈ­
	for (size_t i = 0; i < terrainNormals.size(); ++i)
	{
		// XMStoreHalf4(&terrainNormals[i], XMVector3NormalizeEst(XMLoadHalf4(&terrainNormals[i])));
		XMStoreHalf4(&terrainNormals[i], XMVector3Normalize(XMLoadHalf4(&terrainNormals[i])));
	}

	D3D11_TEXTURE2D_DESC normalMapDesc;
	normalMapDesc.Width = heightMapDesc.Width;
	normalMapDesc.Height = heightMapDesc.Height;
	normalMapDesc.MipLevels = 1;
	normalMapDesc.ArraySize = 1;
	normalMapDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;	// ÁöÇü ³ë¸»¸Ê Æ÷¸Ë (Sampling °¡´ÉÇÑ Æ÷¸Ë (R32G32B32_FLOAT´Â Æ÷ÀÎÆ® »ùÇÃ¸µ¸¸ °¡´É))
	normalMapDesc.SampleDesc.Count = 1;
	normalMapDesc.SampleDesc.Quality = 0;
	normalMapDesc.Usage = D3D11_USAGE_DEFAULT;
	normalMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	normalMapDesc.CPUAccessFlags = 0;
	normalMapDesc.MiscFlags = 0;

	// ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = terrainNormals.data();
	initialData.SysMemPitch = heightMapDesc.Width * sizeof(TerrainNormalMapFormat);
	initialData.SysMemSlicePitch = 0;

	ComPtr<ID3D11Texture2D> cpNormalMapTex2D;
	hr = pDevice->CreateTexture2D(&normalMapDesc, &initialData, cpNormalMapTex2D.GetAddressOf());
	if (FAILED(hr))
		return false;

	ComPtr<ID3D11ShaderResourceView> cpNormalMapSRV;
	hr = pDevice->CreateShaderResourceView(cpNormalMapTex2D.Get(), nullptr, cpNormalMapSRV.GetAddressOf());
	if (FAILED(hr))
		return false;

	Texture2D normalMap(cpNormalMapTex2D, cpNormalMapSRV);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// Step 4. 0À¸·Î ÃÊ±âÈ­µÈ ºí·»µå ¸Ê »ý¼º
	D3D11_TEXTURE2D_DESC blendMapDesc;
	blendMapDesc.Width = heightMapDesc.Width;
	blendMapDesc.Height = heightMapDesc.Height;
	blendMapDesc.MipLevels = 1;
	blendMapDesc.ArraySize = 1;
	blendMapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ºí·»µå¸Ê Æ÷¸Ë (°¢ ·¹ÀÌ¾î´ç 8ºñÆ® UNORM °¡ÁßÄ¡)
	blendMapDesc.SampleDesc.Count = 1;
	blendMapDesc.SampleDesc.Quality = 0;
	blendMapDesc.Usage = D3D11_USAGE_DEFAULT;
	blendMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	blendMapDesc.CPUAccessFlags = 0;
	blendMapDesc.MiscFlags = 0;

	const size_t blendMapSize = blendMapDesc.Width * blendMapDesc.Height;
	std::vector<TerrainBlendMapFormat> blendMapData(blendMapSize);	// TerrainBlendMapFormat »ý¼ºÀÚ¿¡¼­ 0À¸·Î ÃÊ±âÈ­
	// ZeroMemory(blendMapData.data(), blendMapSize);
	D3D11_SUBRESOURCE_DATA blendMapInitialData;
	blendMapInitialData.pSysMem = blendMapData.data();
	blendMapInitialData.SysMemPitch = sizeof(TerrainBlendMapFormat) * blendMapDesc.Width;
	blendMapInitialData.SysMemSlicePitch = 0;
	ComPtr<ID3D11Texture2D> cpBlendMap;
	hr = pDevice->CreateTexture2D(&blendMapDesc, &blendMapInitialData, cpBlendMap.GetAddressOf());
	if (FAILED(hr))
		return false;

	ComPtr<ID3D11ShaderResourceView> cpBlendMapSRV;
	hr = pDevice->CreateShaderResourceView(cpBlendMap.Get(), nullptr, cpBlendMapSRV.GetAddressOf());
	if (FAILED(hr))
		return false;

	Texture2D blendMap(cpBlendMap, cpBlendMapSRV);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬

	m_cellSize = cellSize;
	m_heightScale = heightScale;
	m_patchCtrlPtIndexCount = patchCtrlPtIndexCount;
	m_heightMap = std::move(heightMap);
	m_normalMap = std::move(normalMap);
	m_blendMap = std::move(blendMap);
	m_heightData = std::move(heightData);
	m_cpPatchCtrlPtBuffer = std::move(cpPatchCtrlPtBuffer);
	m_cpPatchCtrlPtIndexBuffer = std::move(cpPatchCtrlPtIndexBuffer);

	return true;
}

bool Terrain::SetTextureLayer(Texture2D diffuseMapLayer, Texture2D normalMapLayer)
{
	m_diffuseMapLayer = std::move(diffuseMapLayer);
	m_normalMapLayer = std::move(normalMapLayer);

	return true;
}

bool Terrain::SetBlendMap(Texture2D blendMap)
{
	if (blendMap == m_blendMap)
		return true;

	ID3D11Texture2D* pBlendMapTex2D = blendMap.GetTexture2D();
	ID3D11Texture2D* pHeightMapTex2D = m_heightMap.GetTexture2D();

	if (!pBlendMapTex2D || !pHeightMapTex2D)
		return false;

	// blendMapÀÌ Height Map°ú µ¿ÀÏÇÑ ÇØ»óµµÀÎÁö °Ë»ç
	D3D11_TEXTURE2D_DESC blendMapDesc;
	pBlendMapTex2D->GetDesc(&blendMapDesc);

	D3D11_TEXTURE2D_DESC heightMapDesc;
	pHeightMapTex2D->GetDesc(&heightMapDesc);

	if ((blendMapDesc.Width != heightMapDesc.Width) || (blendMapDesc.Height != heightMapDesc.Height))
		return false;

	// blendMapÀÇ Æ÷¸Ë °Ë»ç
	if (blendMapDesc.Format != DXGI_FORMAT_R8G8B8A8_UNORM)
		return false;

	m_blendMap = std::move(blendMap);

	return true;
}
