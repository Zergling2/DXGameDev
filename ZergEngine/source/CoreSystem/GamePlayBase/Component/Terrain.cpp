#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>
#include <ZergEngine\CoreSystem\InputLayout.h>

using namespace ze;

constexpr float DEFAULT_TILING_SCALE = 64.0f;

Terrain::Terrain()
	: IComponent(TerrainManager::GetInstance()->AssignUniqueId())
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
	return m_heightScale * static_cast<float>(std::numeric_limits<uint16_t>::max());
}

IComponentManager* Terrain::GetComponentManager() const
{
	return TerrainManager::GetInstance();
}

bool Terrain::SetHeightMap(Texture2D heightMap, float cellSize, float heightScale)
{
	ID3D11Texture2D* pTex2D = heightMap.GetTex2DComInterface();
	if (!pTex2D)
		return false;

	if (cellSize < 0.1f || cellSize > 1.0f)
		return false;

	if (heightScale < 0.01f || heightScale > 1.0f)
		return false;

	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// ��û�� �ؽ��İ� ���̸����� ���� �� �ִ��� �˻�
	D3D11_TEXTURE2D_DESC heightMapDesc;
	pTex2D->GetDesc(&heightMapDesc);
	// �ؽ��� ������ DXGI_FORMAT_R16_UNORM Ÿ������ Ȯ��
	if (heightMapDesc.Format != DXGI_FORMAT_R16_UNORM)	// => UNORM Ÿ�� (������ ���������� ���̴����� Sample �� 0.0 ~ 1.0���� ����ȭ ���� ������ ����)
		return false;

	// D3D11_BIND_SHADER_RESOURCE �÷��׷� �����Ǿ����� Ȯ��
	if (!(heightMapDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE))
		return false;

	// Usage Ȯ��
	if (heightMapDesc.Usage != D3D11_USAGE_DEFAULT)
		return false;

	if (heightMapDesc.CPUAccessFlags != 0)
		return false;

	if (heightMapDesc.SampleDesc.Count != 1)
		return false;

	// ���̸� �ʺ�� ���̰� 64�� ��� + 1���� Ȯ��
	if (heightMapDesc.Width < CELLS_PER_TERRAIN_PATCH + 1 || heightMapDesc.Width % CELLS_PER_TERRAIN_PATCH != 1 ||
		heightMapDesc.Height < CELLS_PER_TERRAIN_PATCH + 1 || heightMapDesc.Height % CELLS_PER_TERRAIN_PATCH != 1)
		return false;
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������




	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// ��� ������Ʈ
	uint32_t patchCtrlPtCountRow = (heightMapDesc.Height - 1) / CELLS_PER_TERRAIN_PATCH + 1;
	uint32_t patchCtrlPtCountCol = (heightMapDesc.Width - 1) / CELLS_PER_TERRAIN_PATCH + 1;
	uint32_t patchCtrlPtIndexCount = (patchCtrlPtCountRow - 1) * (patchCtrlPtCountCol - 1) * 4;	// ��ġ�� �ε��� 4�� (�簢 ��ġ)
	std::vector<float> heightData(heightMapDesc.Width * heightMapDesc.Height);

	const float terrainSizeAlongX = static_cast<float>((patchCtrlPtCountCol - 1) * CELLS_PER_TERRAIN_PATCH) * cellSize;
	const float terrainSizeAlongZ = static_cast<float>((patchCtrlPtCountRow - 1) * CELLS_PER_TERRAIN_PATCH) * cellSize;
	const float maxHeight = heightScale * static_cast<float>(std::numeric_limits<uint16_t>::max());

	ID3D11Device* pDevice = GraphicDevice::GetInstance()->GetDeviceComInterface();
	ID3D11DeviceContext* pImmediateContext = GraphicDevice::GetInstance()->GetImmediateContextComInterface();
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// Step 1. ���� ������ ���� ����� & ���� ������ �ý��� �޸𸮿� ����
	IndexConverter2DTo1D aic;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initialData;
	HRESULT hr = S_OK;
	std::vector<VFTerrainPatchControlPoint> patchCtrlPts(patchCtrlPtCountRow * patchCtrlPtCountCol);
	ComPtr<ID3D11Texture2D> cpHeightMapForCPURead;

	D3D11_TEXTURE2D_DESC heightMapForCPUReadDesc = heightMapDesc;
	heightMapForCPUReadDesc.Usage = D3D11_USAGE_STAGING;	// CPU���� �б� ���ؼ� STAGING �ؽ��� ����
	heightMapForCPUReadDesc.BindFlags = 0;
	heightMapForCPUReadDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	hr = pDevice->CreateTexture2D(&heightMapForCPUReadDesc, nullptr, cpHeightMapForCPURead.GetAddressOf());
	if (FAILED(hr))
		return false;

	// ���̸� �ؽ��� �����͸� �б� ���� ������¡ �ؽ��ķ� ����
	pImmediateContext->CopyResource(cpHeightMapForCPURead.Get(), heightMap.GetTex2DComInterface());

	// ����� ������¡ �ؽ��ĸ� �б�
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = pImmediateContext->Map(cpHeightMapForCPURead.Get(), D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &mapped);
	if (FAILED(hr))
		return false;

	const float tuc = (1.0f / static_cast<float>(heightMapDesc.Width)) / 2.0f;	// �ؽ��� ��ǥ ���� ������ (�ؼ��� �߾� ����Ű�� ���ؼ�)
	const float tvc = (1.0f / static_cast<float>(heightMapDesc.Height)) / 2.0f;	// �ؽ��� ��ǥ ���� ������ (�ؼ��� �߾� ����Ű�� ���ؼ�)
	const float terrainHalfSizeX = static_cast<float>(heightMapDesc.Width - 1) * cellSize / 2.0f;
	const float terrainHalfSizeZ = static_cast<float>(heightMapDesc.Height - 1) * cellSize / 2.0f;
	const uint16_t* pHeightMapData = reinterpret_cast<uint16_t*>(mapped.pData);
	for (uint32_t i = 0; i < patchCtrlPtCountRow; ++i)
	{
		for (uint32_t j = 0; j < patchCtrlPtCountCol; ++j)
		{
			VFTerrainPatchControlPoint& v = patchCtrlPts[i * patchCtrlPtCountCol + j];
			const uint16_t data = pHeightMapData[(i * CELLS_PER_TERRAIN_PATCH) * heightMapDesc.Width + (j * CELLS_PER_TERRAIN_PATCH)];
			v.m_position.x = static_cast<float>(j * CELLS_PER_TERRAIN_PATCH) * cellSize - terrainHalfSizeX;
			v.m_position.y = static_cast<float>(data) / static_cast<float>(std::numeric_limits<uint16_t>::max()) * maxHeight;
			v.m_position.z = -static_cast<float>(i * CELLS_PER_TERRAIN_PATCH) * cellSize + terrainHalfSizeZ;

			v.m_texCoord.x = static_cast<float>(j * CELLS_PER_TERRAIN_PATCH) / static_cast<float>(heightMapDesc.Width) + tuc;
			v.m_texCoord.y = static_cast<float>(i * CELLS_PER_TERRAIN_PATCH) / static_cast<float>(heightMapDesc.Height) + tvc;

			// i�� j�� ���� �Ʒ�, ���� ������ ���� ��ġ�� ����Ű�� ��쿡�� �ٿ�� ����� �ϸ� �ȵȴ�. (Index out of range)
			if (i < patchCtrlPtCountRow - 1 && j < patchCtrlPtCountCol - 1)
			{
				// �ٿ�� ��� (65 x 65 �˻�) �� ���� ������ �ý��� �޸𸮿� ����
				const uint32_t ke = i * CELLS_PER_TERRAIN_PATCH + 65;
				const uint32_t le = j * CELLS_PER_TERRAIN_PATCH + 65;
				uint16_t min = std::numeric_limits<uint16_t>::max();
				uint16_t max = std::numeric_limits<uint16_t>::min();
				aic.SetColumnSize(heightMapDesc.Width);
				for (uint32_t k = i * CELLS_PER_TERRAIN_PATCH; k < ke; ++k)
				{
					for (uint32_t l = j * CELLS_PER_TERRAIN_PATCH; l < le; ++l)
					{
						const size_t index = aic.GetIndex(k, l);
						const uint16_t data = pHeightMapData[index];

						// 1. �ý��� �޸𸮿� ���� ������ ����
						const float normalizedHeight = static_cast<float>(data) / static_cast<float>(std::numeric_limits<uint16_t>::max());
						const float realHeight = normalizedHeight * maxHeight;
						heightData[index] = realHeight;

						// 2. �ٿ�� ���
						if (data < min)
							min = data;
						if (data > max)
							max = data;
					}
				}

				v.m_boundsY.x = static_cast<float>(min) / static_cast<float>(std::numeric_limits<uint16_t>::max()) * maxHeight;
				v.m_boundsY.y = static_cast<float>(max) / static_cast<float>(std::numeric_limits<uint16_t>::max()) * maxHeight;
				if (v.m_boundsY.y - v.m_boundsY.x < 1.0f)	// ��ġ�� ��鿡 ����� ��� �������� �ø��� ����� �� �����Ƿ� �ٿ�� ������ �β��� ������Ų��.
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

	// �� �о����� ����
	pImmediateContext->Unmap(cpHeightMapForCPURead.Get(), D3D11CalcSubresource(0, 0, 0));

	// ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(VFTerrainPatchControlPoint) * patchCtrlPts.size());
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = static_cast<UINT>(sizeof(VFTerrainPatchControlPoint));

	// ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = patchCtrlPts.data();
	initialData.SysMemPitch = 0;	// �ؽ��� ���ҽ������� ��ȿ�� ��
	initialData.SysMemSlicePitch = 0;

	ComPtr<ID3D11Buffer> cpPatchCtrlPtBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &initialData, cpPatchCtrlPtBuffer.GetAddressOf());
	if (FAILED(hr))
		return false;
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// Step 2. ���� ������ �ε��� ���� ����
	std::vector<uint32_t> indexBuffer(patchCtrlPtIndexCount);

	uint32_t k = 0;
	// ��ġ ������ŭ �ݺ�
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
			indexBuffer[k] = static_cast<uint32_t>(aic.GetIndex(i, j));
			indexBuffer[k + 1] = static_cast<uint32_t>(aic.GetIndex(i, j + 1));
			indexBuffer[k + 2] = static_cast<uint32_t>(aic.GetIndex(i + 1, j));
			indexBuffer[k + 3] = static_cast<uint32_t>(aic.GetIndex(i + 1, j + 1));
			k += 4;
		}
	}

	// ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * patchCtrlPtIndexCount);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;	// �ε��� ���۴� ����� ���� ����
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
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// Step 3. �븻 �� ���� (���� ���� �����ϸ� ���)
	aic.SetColumnSize(heightMapDesc.Width);
	const float* pHeightData = heightData.data();
	XMFLOAT3A tangent;
	XMFLOAT3A bitangent;
	std::vector<XMHALF4> normal(heightMapDesc.Width * heightMapDesc.Height);	// R16G16B16A16_FLOAT
	// 1. LEFT TOP �� x ��
	tangent.x = cellSize;
	tangent.y = pHeightData[aic.GetIndex(0, 1)] - pHeightData[aic.GetIndex(0, 0)];
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.y = pHeightData[aic.GetIndex(1, 0)] - pHeightData[aic.GetIndex(0, 0)];
	bitangent.z = -cellSize;
	XMStoreHalf4(&normal[aic.GetIndex(0, 0)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	// 2. RIGHT TOP �� x ��
	tangent.x = -cellSize;
	tangent.y = pHeightData[aic.GetIndex(0, heightMapDesc.Width - 2)] - pHeightData[aic.GetIndex(0, heightMapDesc.Width - 1)];
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.y = pHeightData[aic.GetIndex(1, heightMapDesc.Width - 1)] - pHeightData[aic.GetIndex(0, heightMapDesc.Width - 1)];
	bitangent.z = -cellSize;
	XMStoreHalf4(&normal[aic.GetIndex(0, heightMapDesc.Width - 1)], XMVector3Cross(XMLoadFloat3A(&bitangent), XMLoadFloat3A(&tangent)));
	// 3. LEFT BOTTOM �� x ��
	tangent.x = cellSize;
	tangent.y = pHeightData[aic.GetIndex(heightMapDesc.Height - 1, 1)] - pHeightData[aic.GetIndex(heightMapDesc.Height - 1, 0)];
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.y = pHeightData[aic.GetIndex(heightMapDesc.Height - 2, 0)] - pHeightData[aic.GetIndex(heightMapDesc.Height - 1, 0)];
	bitangent.z = cellSize;
	XMStoreHalf4(&normal[aic.GetIndex(heightMapDesc.Height - 1, 0)], XMVector3Cross(XMLoadFloat3A(&bitangent), XMLoadFloat3A(&tangent)));
	// 4. RIGHT BOTTOM �� x ��
	tangent.x = -cellSize;
	tangent.y = pHeightData[aic.GetIndex(heightMapDesc.Height - 1, heightMapDesc.Width - 2)] - pHeightData[aic.GetIndex(heightMapDesc.Height - 1, heightMapDesc.Width - 1)];
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.y = pHeightData[aic.GetIndex(heightMapDesc.Height - 2, heightMapDesc.Width - 1)] - pHeightData[aic.GetIndex(heightMapDesc.Height - 1, heightMapDesc.Width - 1)];
	bitangent.z = cellSize;
	XMStoreHalf4(&normal[aic.GetIndex(heightMapDesc.Height - 1, heightMapDesc.Width - 1)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	// 5. LEFT EDGE �� x ��2
	tangent.x = cellSize;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = cellSize * -2.0f;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Height - 1); ++i)
	{
		tangent.y = pHeightData[aic.GetIndex(i, 1)] - pHeightData[aic.GetIndex(i, 0)];
		bitangent.y = pHeightData[aic.GetIndex(i + 1, 0)] - pHeightData[aic.GetIndex(i - 1, 0)];
		XMStoreHalf4(&normal[aic.GetIndex(i, 0)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	}
	// 6. TOP EDGE ��2 x ��
	tangent.x = cellSize * 2.0f;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = -cellSize;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Width - 1); ++i)
	{
		tangent.y = pHeightData[aic.GetIndex(0, i + 1)] - pHeightData[aic.GetIndex(0, i - 1)];
		bitangent.y = pHeightData[aic.GetIndex(1, i)] - pHeightData[aic.GetIndex(0, i)];
		XMStoreHalf4(&normal[aic.GetIndex(0, i)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	}
	// 7. RIGHT EDGE �� x ��2
	tangent.x = -cellSize;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = cellSize * 2.0f;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Height - 1); ++i)
	{
		tangent.y = pHeightData[aic.GetIndex(i, heightMapDesc.Width - 2)] - pHeightData[aic.GetIndex(i, heightMapDesc.Width - 1)];
		bitangent.y = pHeightData[aic.GetIndex(i - 1, heightMapDesc.Width - 1)] - pHeightData[aic.GetIndex(i + 1, heightMapDesc.Width - 1)];
		XMStoreHalf4(&normal[aic.GetIndex(i, heightMapDesc.Width - 1)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	}
	// 8. BOTTOM EDGE ��2 x ��
	tangent.x = cellSize * -2.0f;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = cellSize;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Width - 1); ++i)
	{
		tangent.y = pHeightData[aic.GetIndex(heightMapDesc.Height - 1, i - 1)] - pHeightData[aic.GetIndex(heightMapDesc.Height - 1, i + 1)];
		bitangent.y = pHeightData[aic.GetIndex(heightMapDesc.Height - 2, i)] - pHeightData[aic.GetIndex(heightMapDesc.Height - 1, i)];
		XMStoreHalf4(&normal[aic.GetIndex(heightMapDesc.Height - 1, i)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
	}
	// 9. ELSE ��2 x ��2
	tangent.x = cellSize * 2.0f;
	tangent.z = 0.0f;
	bitangent.x = 0.0f;
	bitangent.z = cellSize * -2.0f;
	for (uint32_t i = 1; i < static_cast<uint32_t>(heightMapDesc.Height - 1); ++i)
	{
		for (uint32_t j = 1; j < static_cast<uint32_t>(heightMapDesc.Width - 1); ++j)
		{
			tangent.y = pHeightData[aic.GetIndex(i, j + 1)] - pHeightData[aic.GetIndex(i, j - 1)];
			bitangent.y = pHeightData[aic.GetIndex(i + 1, j)] - pHeightData[aic.GetIndex(i - 1, j)];
			XMStoreHalf4(&normal[aic.GetIndex(i, j)], XMVector3Cross(XMLoadFloat3A(&tangent), XMLoadFloat3A(&bitangent)));
		}
	}

	// ���������� �븻�� ��� ����ȭ
	for (size_t i = 0; i < normal.size(); ++i)
	{
		// XMStoreHalf4(&normal[i], XMVector3NormalizeEst(XMLoadHalf4(&normal[i])));
		XMStoreHalf4(&normal[i], XMVector3Normalize(XMLoadHalf4(&normal[i])));
	}

	D3D11_TEXTURE2D_DESC normalMapDesc;
	normalMapDesc.Width = heightMapDesc.Width;
	normalMapDesc.Height = heightMapDesc.Height;
	normalMapDesc.MipLevels = 1;
	normalMapDesc.ArraySize = 1;
	normalMapDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;	// Sample ������ ���� (R32G32B32_FLOAT�� ����Ʈ ���ø��� ����)
	normalMapDesc.SampleDesc.Count = 1;
	normalMapDesc.SampleDesc.Quality = 0;
	normalMapDesc.Usage = D3D11_USAGE_DEFAULT;
	normalMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	normalMapDesc.CPUAccessFlags = 0;
	normalMapDesc.MiscFlags = 0;

	// ZeroMemory(&initialData, sizeof(initialData));
	initialData.pSysMem = normal.data();
	initialData.SysMemPitch = heightMapDesc.Width * sizeof(XMHALF4);
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
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	// Step 4. 0���� �ʱ�ȭ�� ���� �� ����
	D3D11_TEXTURE2D_DESC blendMapDesc;
	blendMapDesc.Width = heightMapDesc.Width;
	blendMapDesc.Height = heightMapDesc.Height;
	blendMapDesc.MipLevels = 1;
	blendMapDesc.ArraySize = 1;
	blendMapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	blendMapDesc.SampleDesc.Count = 1;
	blendMapDesc.SampleDesc.Quality = 0;
	blendMapDesc.Usage = D3D11_USAGE_DEFAULT;
	blendMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	blendMapDesc.CPUAccessFlags = 0;
	blendMapDesc.MiscFlags = 0;

	const size_t blendMapByteSize = 4 * blendMapDesc.Width * blendMapDesc.Height;
	std::vector<uint8_t> blendMapData(blendMapByteSize);	// 0���� �ʱ�ȭ�� ������
	// ZeroMemory(blendMapData.data(), blendMapByteSize);
	D3D11_SUBRESOURCE_DATA blendMapInitialData;
	blendMapInitialData.pSysMem = blendMapData.data();
	blendMapInitialData.SysMemPitch = sizeof(uint8_t) * 4 * blendMapDesc.Width;
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
	// ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������

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

	ID3D11Texture2D* pBlendMapTex2D = blendMap.GetTex2DComInterface();
	ID3D11Texture2D* pHeightMapTex2D = m_heightMap.GetTex2DComInterface();

	if (!pBlendMapTex2D || !pHeightMapTex2D)
		return false;

	// blendMap�� Height Map�� ������ �ػ����� �˻�
	D3D11_TEXTURE2D_DESC blendMapDesc;
	pBlendMapTex2D->GetDesc(&blendMapDesc);

	D3D11_TEXTURE2D_DESC heightMapDesc;
	pHeightMapTex2D->GetDesc(&heightMapDesc);

	if ((blendMapDesc.Width != heightMapDesc.Width) || (blendMapDesc.Height != heightMapDesc.Height))
		return false;

	// blendMap�� ���� �˻�
	if (blendMapDesc.Format != DXGI_FORMAT_R8G8B8A8_UNORM)
		return false;

	m_blendMap = std::move(blendMap);

	return true;
}
