#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TerrainManager.h>

using namespace ze;

Terrain::Terrain()
	: IComponent(TerrainManager::GetInstance()->AssignUniqueId())
	, m_spTerrainData(nullptr)
{
}

IComponentManager* Terrain::GetComponentManager() const
{
	return TerrainManager::GetInstance();
}

/*
std::shared_ptr<Terrain> Terrain::LoadTerrain(PCWSTR path, const TerrainData& td)
{
	HRESULT hr;
	std::shared_ptr<Terrain> terrain;

	FILE* file = nullptr;
	do
	{
		wchar_t filePath[MAX_PATH];
		StringCbCopyW(filePath, sizeof(filePath), FileSystem::GetResourcePath());
		StringCbCatW(filePath, sizeof(filePath), path);
		const wchar_t* ext = wcsrchr(filePath, L'.');

		if (!ext || (wcscmp(L".r8", ext) != 0 && wcscmp(L".r16", ext) != 0 && wcscmp(L".R8", ext) != 0 && wcscmp(L".R16", ext) != 0))
		{
			AsyncLogBuffer* pLogBuffer = AsyncConsoleLogger::GetLogBuffer();
			StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, L"[Resource::LoadTerrain] Invalid file extension. %s\n", path);
			AsyncConsoleLogger::Write(pLogBuffer);
			break;
		}

		errno_t e = _wfopen_s(&file, filePath, L"rb");
		if (e != 0)
			break;

		// 파일 크기가 매개변수로 전달된 디스크립터와 일치하는지 검사
		_fseeki64(file, 0, SEEK_END);
		const uint32_t fileSize = static_cast<uint32_t>(_ftelli64(file));
		_fseeki64(file, 0, SEEK_SET);
		if (static_cast<uint32_t>(tmd.heightMapSize.row) *		// max: 32769
			static_cast<uint32_t>(tmd.heightMapSize.column) *	// max: 32769
			static_cast<uint32_t>(tmd.elementFormat) != fileSize)
		{
			AsyncLogBuffer* pLogBuffer = AsyncConsoleLogger::GetLogBuffer();
			StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, L"[Resource::LoadTerrain] File size does not match the size specified in the descriptor. %s\n", path);
			AsyncConsoleLogger::Write(pLogBuffer);
			break;
		}

		// 예외 검사 후 terrain 객체 생성
		terrain = std::make_shared<Terrain>(tmd);

		// Height Map 로드
		{
			constexpr float base8bit = static_cast<float>(255);
			constexpr float base16bit = static_cast<float>(65535);
			byte buffer[2048];
			const size_t elementSize = static_cast<size_t>(tmd.elementFormat);
			const size_t elementCountToRead = sizeof(buffer) / elementSize;

			const uint32_t readElementGoal = static_cast<uint32_t>(tmd.heightMapSize.row) * static_cast<uint32_t>(tmd.heightMapSize.column);
			uint32_t currentReadElement = 0;
			uint32_t heightMapIndexCursor = 0;
			switch (tmd.elementFormat)
			{
			case HEIGHT_MAP_FORMAT::RAW_8BIT:
				while (currentReadElement < readElementGoal)
				{
					uint32_t readElement = static_cast<uint32_t>(fread_s(buffer, sizeof(buffer), elementSize, elementCountToRead, file));
					if (readElement == 0)
						FATAL_ERROR;

					for (uint32_t i = 0; i < readElement; ++i)
					{
						terrain->m_pHeightData[heightMapIndexCursor] =
							(static_cast<float>(reinterpret_cast<uint8_t*>(buffer)[i]) / base8bit) * tmd.heightScale + tmd.heightBase;

						++heightMapIndexCursor;
					}

					currentReadElement += readElement;
				}
				break;
			case HEIGHT_MAP_FORMAT::RAW_16BIT:
				while (currentReadElement < readElementGoal)
				{
					uint32_t readElement = static_cast<uint32_t>(fread_s(buffer, sizeof(buffer), elementSize, elementCountToRead, file));
					if (readElement == 0)
						FATAL_ERROR;

					for (uint32_t i = 0; i < readElement; ++i)
					{
						terrain->m_pHeightData[heightMapIndexCursor] =
							(static_cast<float>(reinterpret_cast<uint16_t*>(buffer)[i]) / base16bit) * tmd.heightScale + tmd.heightBase;

						++heightMapIndexCursor;
					}

					currentReadElement += readElement;
				}
				break;
			default:
				FATAL_ERROR;
				break;
			}
		}

		D3D11_BUFFER_DESC descBuffer;
		descBuffer.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
		descBuffer.CPUAccessFlags = 0;
		descBuffer.MiscFlags = 0;
		// 버텍스 버퍼(컨트롤 포인트 버퍼) 생성
		{
			D3D11_SUBRESOURCE_DATA sbrcControlPoints;
			std::vector<VertexFormat::Terrain> controlPoints(terrain->m_numPatchVertexRow * terrain->m_numPatchVertexCol);
			uint32_t patchVertexIndexCursor = 0;
			const float ltX = terrain->GetTerrainWidth() * -0.5f;
			const float ltZ = terrain->GetTerrainDepth() * 0.5f;
			const float fltRowTexelCoordBase = static_cast<float>(tmd.heightMapSize.row);
			const float fltColTexelCoordBase = static_cast<float>(tmd.heightMapSize.column);
			const float texelHalfSpacingU = (1.0f / fltColTexelCoordBase) * 0.5f;
			const float texelHalfSpacingV = (1.0f / fltRowTexelCoordBase) * 0.5f;
			for (uint16_t row = 0; row < tmd.heightMapSize.row; row += CELLS_PERTERRAIN_PATCH)
			{
				const float z = ltZ - static_cast<float>(row) * terrain->GetCellSpacing();
				for (uint16_t col = 0; col < tmd.heightMapSize.column; col += CELLS_PERTERRAIN_PATCH)
				{
					const float x = ltX + static_cast<float>(col) * terrain->GetCellSpacing();
					controlPoints[patchVertexIndexCursor].m_position = XMFLOAT3(x, 0.0f, z);
					// 가만 생각해보면 양 끝 좌표는 0.0과 1.0이 아니다! (착오가 있었음)
					controlPoints[patchVertexIndexCursor].mTexCoord = XMFLOAT2(
						static_cast<float>(col + 1) / fltColTexelCoordBase - texelHalfSpacingU,
						static_cast<float>(row + 1) / fltRowTexelCoordBase - texelHalfSpacingV
					);

					// Y 바운드 계산 (65 x 65 검색)
					if (row != tmd.heightMapSize.row - 1 && col != tmd.heightMapSize.column - 1)
					{
						float minY = std::numeric_limits<float>::max();
						float maxY = std::numeric_limits<float>::lowest();
						for (uint16_t rowOffset = 0; rowOffset <= CELLS_PERTERRAIN_PATCH; ++rowOffset)
						{
							for (uint16_t colOffset = 0; colOffset <= CELLS_PERTERRAIN_PATCH; ++colOffset)
							{
								const uint32_t texelIndex =
									static_cast<uint32_t>(row + rowOffset) * static_cast<uint32_t>(tmd.heightMapSize.column) + (col + colOffset);
								const float height = terrain->m_pHeightData[texelIndex];
								if (height < minY)
									minY = height;
								if (height > maxY)
									maxY = height;
							}
						}

						// 바운딩 박스화
						if (std::abs(maxY - minY) < 0.1f)
						{
							maxY += 0.05f;
							minY -= 0.05f;
						}

						controlPoints[patchVertexIndexCursor].m_boundsY = XMFLOAT2(minY, maxY);
					}
					else
					{
						controlPoints[patchVertexIndexCursor].m_boundsY = XMFLOAT2(0.0f, 0.0f);
					}

					++patchVertexIndexCursor;
				}
			}

			descBuffer.ByteWidth = static_cast<UINT>(sizeof(VertexFormat::Terrain) * controlPoints.size());
			descBuffer.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
			descBuffer.StructureByteStride = static_cast<UINT>(sizeof(VertexFormat::Terrain));

			sbrcControlPoints.pSysMem = controlPoints.data();
			// sbrcControlPoints.SysMemPitch = 0;
			// sbrcControlPoints.SysMemSlicePitch = 0;
			hr = Graphics::GetDevice()->CreateBuffer(&descBuffer, &sbrcControlPoints, terrain->m_patchCtrlPtBuffer.GetAddressOf());
			if (FAILED(hr))
			{
				hrlog(THIS_FILE_NAME, __LINE__, hr);
				break;
			}
		}

		// 인덱스 버퍼 생성
		{
			D3D11_SUBRESOURCE_DATA sbrcControlPointIndices;

			descBuffer.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * terrain->m_patchCtrlPtIndexCount);
			descBuffer.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
			descBuffer.StructureByteStride = 0;
			std::vector<uint32_t> tempIB(terrain->m_patchCtrlPtIndexCount);

			uint32_t k = 0;
			// 패치 개수만큼 반복
			for (uint16_t row = 0; row < terrain->m_numPatchVertexRow - 1; ++row)
			{
				for (uint16_t col = 0; col < terrain->m_numPatchVertexCol - 1; ++col)
				{
					// [0]  [1]
					//
					// [2]  [3]
					tempIB[k] = (row)*terrain->m_numPatchVertexCol + (col);
					tempIB[k + 1] = (row)*terrain->m_numPatchVertexCol + (col + 1);
					tempIB[k + 2] = (row + 1) * terrain->m_numPatchVertexCol + (col);
					tempIB[k + 3] = (row + 1) * terrain->m_numPatchVertexCol + (col + 1);
					k += 4;
				}
			}

			sbrcControlPointIndices.pSysMem = tempIB.data();
			// sbrcControlPointIndices.SysMemPitch = 0;
			// sbrcControlPointIndices.SysMemSlicePitch = 0;
			hr = Graphics::GetDevice()->CreateBuffer(&descBuffer, &sbrcControlPointIndices, terrain->m_patchCtrlPtIndexBuffer.GetAddressOf());
			if (FAILED(hr))
			{
				hrlog(THIS_FILE_NAME, __LINE__, hr);
				break;
			}
		}

		// 셰이더 리소스 뷰 생성
		{
			D3D11_TEXTURE2D_DESC descHeightMap;
			D3D11_SUBRESOURCE_DATA sbrcHeightMap;

			descHeightMap.Width = tmd.heightMapSize.column;
			descHeightMap.Height = tmd.heightMapSize.row;
			descHeightMap.MipLevels = 1;
			descHeightMap.ArraySize = 1;
			descHeightMap.Format = DXGI_FORMAT_R16_FLOAT;	// 16bit float (PackedVector::HALF)
			descHeightMap.SampleDesc.Count = 1;
			descHeightMap.SampleDesc.Quality = 0;
			descHeightMap.Usage = D3D11_USAGE_IMMUTABLE;
			descHeightMap.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			descHeightMap.CPUAccessFlags = 0;
			descHeightMap.MiscFlags = 0;

			const uint32_t texelCount = static_cast<uint32_t>(tmd.heightMapSize.row) * static_cast<uint32_t>(tmd.heightMapSize.column);
			std::vector<PackedVector::HALF> halfFPHeightMap(texelCount);
			PackedVector::XMConvertFloatToHalfStream(halfFPHeightMap.data(), sizeof(PackedVector::HALF),
				terrain->m_pHeightData, sizeof(float), texelCount);
			sbrcHeightMap.pSysMem = halfFPHeightMap.data();
			sbrcHeightMap.SysMemPitch = descHeightMap.Width * sizeof(PackedVector::HALF);
			sbrcHeightMap.SysMemSlicePitch = 0;
			ComPtr<ID3D11Texture2D> cpHeightmap;
			hr = Graphics::GetDevice()->CreateTexture2D(&descHeightMap, &sbrcHeightMap, cpHeightmap.GetAddressOf());
			if (FAILED(hr))
			{
				hrlog(THIS_FILE_NAME, __LINE__, hr);
				break;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC descHeightMapSRV;
			descHeightMapSRV.Format = descHeightMap.Format;
			descHeightMapSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			descHeightMapSRV.Texture2D.MostDetailedMip = 0;
			descHeightMapSRV.Texture2D.MipLevels = -1;
			hr = Graphics::GetDevice()->CreateShaderResourceView(cpHeightmap.Get(), &descHeightMapSRV, terrain->m_srvHeightMap.GetAddressOf());
			if (FAILED(hr))
			{
				hrlog(THIS_FILE_NAME, __LINE__, hr);
				break;
			}
		}
	} while (false);

	if (file)
		fclose(file);

	return terrain;
}
*/
