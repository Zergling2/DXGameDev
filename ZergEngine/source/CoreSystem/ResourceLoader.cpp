#include <ZergEngine\CoreSystem\ResourceLoader.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\DataStructure\RawVector.h>
#include <ZergEngine\CoreSystem\Resource\Mesh.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <DirectXTex\DirectXTex.h>

using namespace ze;

ResourceLoader* ResourceLoader::s_pInstance = nullptr;

constexpr int MAX_LINE_LENGTH = 256;
PCWSTR OBJ_MTL_DELIM = L" \t\n";

ResourceLoader::ResourceLoader()
	: m_errTex()
{
}

ResourceLoader::~ResourceLoader()
{
}

void ResourceLoader::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new ResourceLoader();
}

void ResourceLoader::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void ResourceLoader::Init()
{
	// 보라돌이 텍스처 생성
	// m_errTex;
}

void ResourceLoader::UnInit()
{
}

std::vector<std::shared_ptr<Mesh>> ResourceLoader::LoadWavefrontOBJ(PCWSTR path)
{
	FILE* pMeshFile = nullptr;
	std::vector<std::shared_ptr<Mesh>> meshes;

	do
	{
		VertexPack vp;

		// UTF-8 -> UTF-16 자동 인코딩 변환하는 것으로 확인
		// HexEditor로 열었을 때와 이 함수로 읽어들였을 때 값이 다름. (변환이 됨)
		errno_t e;
		e = _wfopen_s(&pMeshFile, path, L"rt, ccs=UTF-8");
		if (e != 0)
			Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open mesh file.\n%s", path);

		Helper::AutoCRTFileCloser afc(pMeshFile);

		// Wavefront OBJ parsing
		WCHAR line[MAX_LINE_LENGTH];
		PWSTR nextToken = nullptr;
		PCWSTR token;
		while (fgetws(line, _countof(line), pMeshFile))
		{
			token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);
			const WCHAR firstChar = token ? token[0] : L'\0';

			if (!token || firstChar == L'#')
				continue;

			if (firstChar == L'o' && !wcscmp(token, L"o"))
			{
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);		// token = object name
				if (token != nullptr)
				{
					std::shared_ptr<Mesh> spMesh = std::make_shared<Mesh>(token);
					Mesh* pMesh = spMesh.get();
					meshes.push_back(std::move(spMesh));
					long ofpos;
					if (ResourceLoader::ParseWavefrontOBJObject(pMeshFile, &ofpos, vp, pMesh))
						fseek(pMeshFile, ofpos, SEEK_SET);
				}
			}
		}
	} while (false);

	return meshes;
}

Texture2D ResourceLoader::LoadTexture2D(PCWSTR path, bool generateMipMaps)
{
	HRESULT hr;

	PCWSTR ext = wcsrchr(path, L'.');
	if (ext == nullptr)
		Debug::ForceCrashWithMessageBox(L"Error", L"Unknown file extension: %s", path);

	// 이미지 로드
	ScratchImage mipChain;
	{
		ScratchImage image;

		if (!wcscmp(L".dds", ext) || !wcscmp(L".DDS", ext))
			hr = LoadFromDDSFile(path, DDS_FLAGS::DDS_FLAGS_NONE, nullptr, image);	// dds, DDS
		else if (!wcscmp(L".tga", ext) || !wcscmp(L".TGA", ext))
			hr = LoadFromTGAFile(path, nullptr, image);								// tga, TGA
		else
			hr = LoadFromWICFile(path, WIC_FLAGS::WIC_FLAGS_NONE, nullptr, image);	// png, jpg, jpeg, bmp, ...

		if (FAILED(hr))
			Debug::ForceCrashWithMessageBox(L"Error", L"Failed to load image with LoadFromXXXFile.\n%s\n", path);

		const TexMetadata& imageMetadata = image.GetMetadata();

		if (imageMetadata.mipLevels > 1 || generateMipMaps == false)
		{
			// 이미 밉맵이 있는 경우이거나 밉맵 생성이 요청되지 않은 경우
			mipChain = std::move(image);
		}
		else
		{
			if (IsCompressed(imageMetadata.format))
				hr = this->GenerateMipMapsForBCFormat(image, mipChain);
			else
				hr = GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);

			if (FAILED(hr))
				Debug::ForceCrashWithHRESULTMessageBox(L"Failed to generate mip maps.", hr);
		}
	}

	// 디스크립터 세팅
	const TexMetadata& metadata = mipChain.GetMetadata();
	D3D11_TEXTURE2D_DESC descTexture;
	ZeroMemory(&descTexture, sizeof(descTexture));
	descTexture.Width = static_cast<UINT>(metadata.width);
	descTexture.Height = static_cast<UINT>(metadata.height);
	descTexture.MipLevels = static_cast<UINT>(metadata.mipLevels);
	descTexture.ArraySize = static_cast<UINT>(metadata.arraySize);
	descTexture.Format = metadata.format;
	descTexture.SampleDesc.Count = 1;	// 렌더 타겟이 아니므로 안티앨리어싱 X
	descTexture.SampleDesc.Quality = 0;	// 렌더 타겟이 아니므로 안티앨리어싱 X
	descTexture.Usage = D3D11_USAGE_DEFAULT;
	descTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE;		// 셰이더 리소스 목적으로 생성
	descTexture.MiscFlags = metadata.IsCubemap() ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

	// 텍스처 생성 및 초기화
	ComPtr<ID3D11Texture2D> cpTex2d;
	std::vector<D3D11_SUBRESOURCE_DATA> initialData(metadata.arraySize * metadata.mipLevels);
	for (size_t i = 0; i < metadata.arraySize; ++i)
	{
		for (size_t j = 0; j < metadata.mipLevels; ++j)
		{
			const size_t index = i * metadata.mipLevels + j;
			const Image* pImg = mipChain.GetImage(j, i, 0);

			initialData[index].pSysMem = pImg->pixels;
			initialData[index].SysMemPitch = static_cast<UINT>(pImg->rowPitch);		// 텍스쳐 너비 * 텍셀 당 바이트 크기
			// initialData[index].SysMemSlicePitch = 0;		// 3D 텍스쳐에서만 의미 있음
		}
	}
	hr = GraphicDevice::GetInstance()->GetDeviceComInterface()->CreateTexture2D(
		&descTexture, initialData.data(), cpTex2d.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateTexture2D()", hr);

	// 셰이더 리소스 뷰 생성
	ComPtr<ID3D11ShaderResourceView> cpSRV;
	hr = GraphicDevice::GetInstance()->GetDeviceComInterface()->CreateShaderResourceView(
		cpTex2d.Get(), nullptr, cpSRV.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateShaderResourceView()", hr);

	return Texture2D(cpTex2d, cpSRV);
}

std::shared_ptr<Material> ResourceLoader::CreateMaterial()
{
	return std::make_shared<Material>();
}

bool ResourceLoader::CreateHeightMapFromRawData(Texture2D& heightMap, const uint16_t* pData, SIZE resolution)
{
	if (!pData)
		return false;

	// 너비와 높이가 64의 배수 + 1인지 확인
	if (resolution.cx < CELLS_PER_TERRAIN_PATCH + 1 || resolution.cx % CELLS_PER_TERRAIN_PATCH != 1 ||
		resolution.cy < CELLS_PER_TERRAIN_PATCH + 1 || resolution.cy % CELLS_PER_TERRAIN_PATCH != 1)
		return false;

	D3D11_TEXTURE2D_DESC heightMapDesc;
	// ZeroMemory(&heightMapDesc, sizeof(heightMapDesc));
	heightMapDesc.Width = resolution.cx;
	heightMapDesc.Height = resolution.cy;
	heightMapDesc.MipLevels = 1;
	heightMapDesc.ArraySize = 1;
	heightMapDesc.Format = DXGI_FORMAT_R16_UNORM;
	heightMapDesc.SampleDesc.Count = 1;
	heightMapDesc.SampleDesc.Quality = 0;
	heightMapDesc.Usage = D3D11_USAGE_DEFAULT;
	heightMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	heightMapDesc.CPUAccessFlags = 0;
	heightMapDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = pData;
	initialData.SysMemPitch = resolution.cx * sizeof(uint16_t);
	initialData.SysMemSlicePitch = 0;

	HRESULT hr;
	ID3D11Device* pDevice = GraphicDevice::GetInstance()->GetDeviceComInterface();
	

	ComPtr<ID3D11Texture2D> cpTex2D;
	hr = pDevice->CreateTexture2D(&heightMapDesc, &initialData, cpTex2D.GetAddressOf());
	if (FAILED(hr))
		return false;

	ComPtr<ID3D11ShaderResourceView> cpSRV;
	hr = pDevice->CreateShaderResourceView(cpTex2D.Get(), nullptr, cpSRV.GetAddressOf());
	if (FAILED(hr))
		return false;

	heightMap.m_cpTex2D = std::move(cpTex2D);
	heightMap.m_cpSRV = std::move(cpSRV);

	return true;
}

HRESULT ResourceLoader::GenerateMipMapsForBCFormat(const ScratchImage& src, ScratchImage& result)
{
	// Block compressed (BC) 포맷일 경우 Decompress 후 밉맵 생성 및 다시 Compress 필요
	// https://github.com/microsoft/DirectXTex/wiki/GenerateMipMaps
	// https://github.com/microsoft/DirectXTex/wiki/Decompress
	// https://github.com/microsoft/DirectXTex/wiki/Compress

	HRESULT hr = S_OK;

	do
	{
		const TexMetadata& metadata = src.GetMetadata();
		DXGI_FORMAT srcFormat = metadata.format;

		// 압축 포맷인지 확인
		if (!IsCompressed(srcFormat))
		{
			hr = E_FAIL;
			break;
		}

		// 적절한 비압축 포맷 선택
		DXGI_FORMAT decompFmt;
		switch (srcFormat)
		{
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			decompFmt = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			decompFmt = DXGI_FORMAT_R8_UNORM;
			break;
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
			decompFmt = DXGI_FORMAT_R8G8_UNORM;
			break;
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
			decompFmt = DXGI_FORMAT_R16G16B16A16_FLOAT;
			break;
		default:
			decompFmt = DXGI_FORMAT_UNKNOWN;
			break;
		}

		// 압축 해제
		ScratchImage decompressed;
		hr = Decompress(
			src.GetImages(),
			src.GetImageCount(),
			metadata,
			decompFmt,
			decompressed);
		if (FAILED(hr))
			break;

		// 밉맵 생성
		ScratchImage mipChain;
		hr = GenerateMipMaps(
			decompressed.GetImages(),
			decompressed.GetImageCount(),
			decompressed.GetMetadata(),
			TEX_FILTER_DEFAULT,
			0,
			mipChain
		);
		if (FAILED(hr))
			break;

		hr = Compress(
			mipChain.GetImages(),
			mipChain.GetImageCount(),
			mipChain.GetMetadata(),
			srcFormat,
			TEX_COMPRESS_DEFAULT,
			1.0f,
			result);
		if (FAILED(hr))
			break;
	} while (false);

	return hr;
}

bool ResourceLoader::ParseWavefrontOBJObject(FILE* pOBJFile, long* pofpos, VertexPack& vp, Mesh* pMesh)
{
	HRESULT hr;
	// object 정보를 읽는다.
	// f가 나오면 ParseWavefrontOBJFaces 함수로 면 정보들을 읽는다.
	WCHAR line[MAX_LINE_LENGTH];
	WCHAR* nextToken = nullptr;
	WCHAR* wcstoxEnd;
	PCWSTR token;

	bool ret_o = false;
	VERTEX_FORMAT_TYPE vft = VERTEX_FORMAT_TYPE::UNKNOWN;
	RawVector tempVB;
	std::vector<uint32_t> tempIB;

	IndexMapPack imp;

	long fpos = ftell(pOBJFile);
	while (fgetws(line, _countof(line), pOBJFile))
	{
		token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);
		const WCHAR firstChar = token ? token[0] : L'\0';

		if (!token || firstChar == L'#')
		{
		}
		else if (firstChar == L'o' && !wcscmp(token, L"o"))
		{
			ret_o = true;		// 또 다른 Mesh 스타트 심볼 등장
			*pofpos = fpos;
			break;
		}
		else if (firstChar == L'v')
		{
			if (!wcscmp(token, L"v"))
			{
				// Vertex Position
				XMFLOAT3 position;
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				position.x = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				position.y = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				position.z = -wcstof(token, &wcstoxEnd);		// import right hand coord system model
				vp.v.push_back(position);
			}
			else if (!wcscmp(token, L"vt"))
			{
				// Vertex TexCoord
				XMFLOAT2 uv;
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				uv.x = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				uv.y = 1.0f - wcstof(token, &wcstoxEnd);		// import right hand coord system model
				vp.vt.push_back(uv);
			}
			else if (!wcscmp(token, L"vn"))
			{
				// Vertex Normal
				XMFLOAT3 normal;
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				normal.x = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				normal.y = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				normal.z = -wcstof(token, &wcstoxEnd);			// import right hand coord system model
				vp.vn.push_back(normal);
			}
		}
		else if (firstChar == L'f' && !wcscmp(token, L"f"))
		{
			// Vertex Format Layout 구하기 (Object의 Subset마다 서로 다른 Vertex Layout을 가지는 것은 불가능하다고 간주)
			// 메시의 Vertex Format Layout을 아직 모르는 경우 최초 1회 레이아웃을 파악한다.
			if (vft == VERTEX_FORMAT_TYPE::UNKNOWN)
			{
				PCWSTR firstIndexSet = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				PCWSTR firstSlash = wcschr(firstIndexSet, L'/');
				PCWSTR lastSlash = wcsrchr(firstIndexSet, L'/');
				if (!firstSlash)							// v
					vft = VERTEX_FORMAT_TYPE::POSITION;
				else if (firstSlash == lastSlash)			// v/vt
					vft = VERTEX_FORMAT_TYPE::POSITION_TEXCOORD;
				else if (firstSlash + 1 == lastSlash)		// v//vn
					vft = VERTEX_FORMAT_TYPE::POSITION_NORMAL;
				else										// v/vt/vn
					vft = VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD;

				pMesh->m_vft = vft;
			}

			fseek(pOBJFile, fpos, SEEK_SET);
			long nffpos;
			if (ResourceLoader::ParseWavefrontOBJFaces(pOBJFile, &nffpos, vft, vp, imp, pMesh, tempVB, tempIB))
				fseek(pOBJFile, nffpos, SEEK_SET);
		}

		fpos = ftell(pOBJFile);	// save file stream pointer
	}

	assert(pMesh->m_vft != VERTEX_FORMAT_TYPE::UNKNOWN);

	{
		// AABB 정보 생성
		ptrdiff_t stride;

		// Alignment: 4바이트
		switch (pMesh->m_vft)
		{
		case VERTEX_FORMAT_TYPE::POSITION:
			stride = sizeof(VFPosition);
			break;
		case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
			stride = sizeof(VFPositionNormal);
			break;
		case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
			stride = sizeof(VFPositionTexCoord);
			break;
		case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
			stride = sizeof(VFPositionNormalTexCoord);
			break;
		default:
			stride = 0;
			break;
		}
		assert(stride != 0);

		XMVECTOR min = XMVectorReplicate(std::numeric_limits<FLOAT>::max());
		XMVECTOR max = XMVectorReplicate(std::numeric_limits<FLOAT>::lowest());

		intptr_t cursor = reinterpret_cast<intptr_t>(tempVB.Data());
		const size_t count = tempVB.ByteSize() / stride;
		for (size_t i = 0; i < count; ++i)
		{
			XMFLOAT3* pPosition = reinterpret_cast<XMFLOAT3*>(cursor);
			XMVECTOR position = XMLoadFloat3(pPosition);
			min = XMVectorMin(min, position);
			max = XMVectorMax(max, position);

			cursor += stride;
		}

		// Center, Extent 계산
		XMFLOAT3A center;
		XMFLOAT3A extent;
		XMStoreFloat3A(&center, XMVectorScale(XMVectorAdd(min, max), 0.5f));
		XMStoreFloat3A(&extent, XMVectorAbs(XMVectorScale(XMVectorSubtract(max, min), 0.5f)));

		// extent의 성분 중 너무 작은 값이 있으면 프러스텀 컬링에 실패할 수 있으므로 약간의 크기를 보장한다.
		XMVECTOR minExtent = XMVectorReplicate(0.05f);
		XMStoreFloat3A(&extent, XMVectorMax(XMLoadFloat3A(&extent), minExtent));

		pMesh->m_aabb.Center = center;
		pMesh->m_aabb.Extents = extent;
	}

	{
		// Create a vertex buffer
		D3D11_BUFFER_DESC bufferDesc;
		D3D11_SUBRESOURCE_DATA initialData;

		bufferDesc.ByteWidth = static_cast<UINT>(tempVB.ByteSize());
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = InputLayoutHelper::GetStructureByteStride(vft);

		initialData.pSysMem = tempVB.Data();
		// initialData.SysMemPitch = 0;		// unused
		// initialData.SysMemSlicePitch = 0;	// unused

		ComPtr<ID3D11Buffer> cpVB;
		hr = GraphicDevice::GetInstance()->GetDeviceComInterface()->CreateBuffer(&bufferDesc, &initialData, cpVB.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateBuffer()", hr);

		// Create an index buffer
		bufferDesc.ByteWidth = static_cast<UINT>(tempIB.size() * sizeof(uint32_t));
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		// bufferDesc.CPUAccessFlags = 0;
		// bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(uint32_t);

		initialData.pSysMem = tempIB.data();
		// initialData.SysMemPitch = 0;			// unused
		// initialData.SysMemSlicePitch = 0;	// unused

		ComPtr<ID3D11Buffer> cpIB;
		hr = GraphicDevice::GetInstance()->GetDeviceComInterface()->CreateBuffer(&bufferDesc, &initialData, cpIB.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateBuffer()", hr);

		pMesh->m_cpVB = std::move(cpVB);
		pMesh->m_cpIB = std::move(cpIB);
	}

	return ret_o;
}

bool ResourceLoader::ParseWavefrontOBJFaces(FILE* pOBJFile, long* pnffpos, VERTEX_FORMAT_TYPE vft, const VertexPack& vp,
	IndexMapPack& imp, Mesh* pMesh, RawVector& tempVB, std::vector<uint32_t>& tempIB)
{
	// f만 읽다가 아닌 경우 리턴

	WCHAR line[MAX_LINE_LENGTH];
	PWSTR nextToken = nullptr;
	PCWSTR token;
	uint32_t indexCount = 0;
	bool ret_nf = false;		// not f

	// Create a subset
	pMesh->m_subsets.push_back(Subset());
	const size_t currentSubsetIndex = pMesh->m_subsets.size() - 1;
	Subset& currentSubset = pMesh->m_subsets[currentSubsetIndex];
	currentSubset.m_startIndexLocation = static_cast<uint32_t>(tempIB.size());		// Set index base

	long fpos = ftell(pOBJFile);
	while (fgetws(line, _countof(line), pOBJFile))
	{
		token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);
		const WCHAR firstChar = token ? token[0] : L'\0';

		if (!token || firstChar == L'#')
		{
		}
		else if (firstChar == L'f' && !wcscmp(token, L"f"))
		{
			constexpr uint32_t FACE_VERTEX_COUNT = 3;		// Consider only triangulated faces!
			indexCount += FACE_VERTEX_COUNT;

			// Read vertices
			PWSTR indexSet[FACE_VERTEX_COUNT] =
			{
				wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken),
				wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken),
				wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken)
			};
			PWSTR const temp = indexSet[0];		// obj 파일이 오른손좌표계 기준이므로 왼손좌표계 기준으로 winding 순서 변경
			indexSet[0] = indexSet[2];
			indexSet[2] = temp;

			// Only considered Triangulated mesh.
			for (uint32_t i = 0; i < FACE_VERTEX_COUNT; ++i)
			{
				PCWSTR const index[3] =
				{
					wcstok_s(indexSet[i], L"/", &nextToken),
					wcstok_s(nullptr, L"/", &nextToken),
					wcstok_s(nullptr, L"/", &nextToken)
				};

				PWSTR toulEnd;
				uint32_t vIndex;
				uint32_t vtIndex;
				uint32_t vnIndex;

				switch (vft)
				{
				case VERTEX_FORMAT_TYPE::POSITION:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					{
						const IndexSet::Position item(vIndex);
						auto iter = imp.vIndexMap.find(item);
						if (iter == imp.vIndexMap.cend())
						{
							VFPosition vData(vp.v[vIndex]);
							tempVB.PushBack(&vData, sizeof(vData));

							const uint32_t vbIndex = static_cast<uint32_t>(tempVB.ByteSize() / sizeof(vData) - 1);
							imp.vIndexMap.insert(std::make_pair(item, vbIndex));
							tempIB.push_back(vbIndex);
						}
						else
						{
							tempIB.push_back(iter->second);
						}
					}
					break;
				case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vnIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionNormal item(vIndex, vnIndex);
						auto iter = imp.vvnIndexMap.find(item);
						if (iter == imp.vvnIndexMap.cend())
						{
							VFPositionNormal vData(vp.v[vIndex], vp.vn[vnIndex]);
							tempVB.PushBack(&vData, sizeof(vData));

							const uint32_t vbIndex = static_cast<uint32_t>(tempVB.ByteSize() / sizeof(vData) - 1);
							imp.vvnIndexMap.insert(std::make_pair(item, vbIndex));
							tempIB.push_back(vbIndex);
						}
						else
						{
							tempIB.push_back(iter->second);
						}
					}
					break;
				case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vtIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionTexCoord item(vIndex, vtIndex);
						auto iter = imp.vvtIndexMap.find(item);
						if (iter == imp.vvtIndexMap.cend())
						{
							VFPositionTexCoord vData(vp.v[vIndex], vp.vt[vtIndex]);
							tempVB.PushBack(&vData, sizeof(vData));

							const uint32_t vbIndex = static_cast<uint32_t>(tempVB.ByteSize() / sizeof(vData) - 1);
							imp.vvtIndexMap.insert(std::make_pair(item, vbIndex));
							tempIB.push_back(vbIndex);
						}
						else
						{
							tempIB.push_back(iter->second);
						}
					}
					break;
				case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vtIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					vnIndex = wcstoul(index[2], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionNormalTexCoord item(vIndex, vnIndex, vtIndex);
						auto iter = imp.vvnvtIndexMap.find(item);
						if (iter == imp.vvnvtIndexMap.cend())
						{
							VFPositionNormalTexCoord vData(vp.v[vIndex], vp.vn[vnIndex], vp.vt[vtIndex]);
							tempVB.PushBack(&vData, sizeof(vData));

							const uint32_t vbIndex = static_cast<uint32_t>(tempVB.ByteSize() / sizeof(vData) - 1);
							imp.vvnvtIndexMap.insert(std::make_pair(item, vbIndex));
							tempIB.push_back(vbIndex);
						}
						else
						{
							tempIB.push_back(iter->second);
						}
					}
					break;
				}
			}
		}
		else
		{
			ret_nf = true;	// f가 아닌 심볼을 읽어 함수를 빠져나간다.
			*pnffpos = fpos;
			break;
		}

		fpos = ftell(pOBJFile);	// 커서 백업
	}

	currentSubset.m_indexCount = indexCount;		// Set index count

	return ret_nf;
}


/*
Texture2D ResourceLoader::LoadCubeMapTexture(PCWSTR path)
{
	HRESULT hr;

	WCHAR filePath[MAX_PATH];
	hr = FileSystem::GetInstance()->RelativePathToFullPath(path, filePath, sizeof(filePath));
	if (FAILED(hr))
		Debug::ForceCrashWithMessageBox(L"Error", L"Failed to create full file path.\n%s", path);

	PCWSTR ext = wcsrchr(filePath, L'.');
	if (ext == nullptr)
		Debug::ForceCrashWithMessageBox(L"Error", L"Unknown file extension: %s", path);

	// 이미지 로드
	ScratchImage mipChain;

	if (wcscmp(L".dds", ext) != 0 && wcscmp(L".DDS", ext) != 0)
		Debug::ForceCrashWithMessageBox(L"Error", L"%s is not supported file format for cubemap texture.", path);

	hr = LoadFromDDSFile(filePath, DDS_FLAGS::DDS_FLAGS_NONE, nullptr, mipChain);	// dds, DDS
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"LoadCubeMapTexture() > LoadFromDDSFile()", hr);

	// 디스크립터 세팅
	const TexMetadata& metadata = mipChain.GetMetadata();
	if (!metadata.IsCubemap())
		Debug::ForceCrashWithMessageBox(L"Error", L"%s is not a cubemap texture.", path);

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = static_cast<UINT>(metadata.width);
	textureDesc.Height = static_cast<UINT>(metadata.height);
	textureDesc.MipLevels = static_cast<UINT>(metadata.mipLevels);
	textureDesc.ArraySize = static_cast<UINT>(metadata.arraySize);
	textureDesc.Format = metadata.format;
	textureDesc.SampleDesc.Count = 1;		// 렌더타겟용 큐브맵이 아니므로 안티앨리어싱 X
	textureDesc.SampleDesc.Quality = 0;		// 렌더타겟용 큐브맵이 아니므로 안티앨리어싱 X
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;		// 스카이박스 큐브 텍스쳐

	// 텍스처 생성
	ComPtr<ID3D11Texture2D> cpTex2D;
	{

		// CreateTexture2D creates a 2D texture resource, which can contain a number of 2D subresources.
		// The number of textures is specified in the texture description.
		// All textures in a resource must have the same format, size, and number of mipmap levels.
		//
		// All resources are made up of one or more subresources. To load data into the texture,
		// applications can supply the data initially as an 'array of D3D11_SUBRESOURCE_DATA structures' pointed to by pInitialData,
		// or it may use one of the D3DX texture functions such as D3DX11CreateTextureFromFile. (D3DX 함수는 UWP 사용 불가...)
		// For a 32 x 32 texture with a full mipmap chain, the pInitialData array has the following 6 elements:
		//
		// (텍스쳐 배열의 경우 0번 이미지의 밉맵 포인터 나열 후 1번 이미지의 밉맵 포인터 나열, ...)
		// pInitialData[0] = 32x32		pInitialData[6] = 32x32
		// pInitialData[1] = 16x16		pInitialData[7] = 16x16
		// pInitialData[2] = 8x8		pInitialData[8] = 8x8
		// pInitialData[3] = 4x4		pInitialData[9] = 4x4
		// pInitialData[4] = 2x2		pInitialData[10] = 2x2
		// pInitialData[5] = 1x1		pInitialData[11] = 1x1

		std::vector<D3D11_SUBRESOURCE_DATA> initialData(metadata.arraySize * metadata.mipLevels);
		for (size_t i = 0; i < metadata.arraySize; ++i)
		{
			for (size_t j = 0; j < metadata.mipLevels; ++j)
			{
				const size_t index = i * metadata.mipLevels + j;
				const Image* pImg = mipChain.GetImage(j, i, 0);

				initialData[index].pSysMem = pImg->pixels;
				initialData[index].SysMemPitch = static_cast<UINT>(pImg->rowPitch);// 텍스쳐 너비 * 텍셀 당 바이트 크기
				// initialData[index].SysMemSlicePitch = 0;		// 3D 텍스쳐에서만 의미 있음
			}
		}

		hr = GraphicDevice::GetInstance()->GetDeviceComInterface()->CreateTexture2D(
			&textureDesc, initialData.data(), cpTex2D.GetAddressOf()
		);
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateTexture2D()", hr);
	}

	// 셰이더 리소스 뷰 생성
	ComPtr<ID3D11ShaderResourceView> cpSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;	// 사용할 가장 디테일한 밉 레벨
	srvDesc.TextureCube.MipLevels = -1;		// -1로 설정 시 MostDetailedMip에 설정한 밉 레벨부터 최소 퀄리티 밉맵까지 사용
	hr = GraphicDevice::GetInstance()->GetDeviceComInterface()->CreateShaderResourceView(
		cpTex2D.Get(), &srvDesc, cpSRV.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateShaderResourceView()", hr);

	return Texture2D(cpTex2D, cpSRV);
}
*/

/*
bool Resource::ReadFaces_deprecated(FILE* pOBJFile, long* pnffpos, const VERTEX_FORMAT_TYPE vft, const VertexPack& vp,
	IndexMapPack& imp, Mesh& mesh, size_t meshIndex, RawVector& tempVB, std::vector<uint32_t>& tempIB, std::vector<DeferredMtlLinkingData>& dml,
	SubsetAttribute& subsetAttribute)
{
	wchar_t line[MAX_LINE_LENGTH];
	wchar_t* nextToken = nullptr;
	const wchar_t* token;
	uint32_t indexCount = 0;
	bool ret_nf = false;		// not f

	// Create a subset
	mesh.m_subsets.emplace_back(subsetAttribute.m_shadeSmooth);
	const size_t currentSubsetIndex = mesh.m_subsets.size() - 1;
	Subset& currentSubset = mesh.m_subsets[currentSubsetIndex];
	currentSubset.m_startIndexLocation = static_cast<uint32_t>(tempIB.size());		// Set index base

	if (subsetAttribute.m_mtlName)
		dml.emplace_back(subsetAttribute.m_mtlName, meshIndex, currentSubsetIndex);

	long fpos = ftell(pOBJFile);
	while (fgetws(line, _countof(line), pOBJFile))
	{
		token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);
		const wchar_t firstChar = token ? token[0] : L'\0';

		if (!token || firstChar == L'#')
		{
		}
		else if (firstChar == L'f' && !wcscmp(token, L"f"))
		{
			constexpr uint32_t FACE_VERTEX_COUNT = 3;		// Consider only triangulated faces!
			indexCount += FACE_VERTEX_COUNT;

			// Read vertices
			wchar_t* indexSet[FACE_VERTEX_COUNT] =
			{
				wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken),
				wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken),
				wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken)
			};
			wchar_t* const temp = indexSet[0];		// obj 파일이 오른손좌표계 기준이므로 왼손좌표계 기준으로 winding 순서 변경
			indexSet[0] = indexSet[2];
			indexSet[2] = temp;

			// Only considered Triangulated mesh.
			for (uint32_t i = 0; i < FACE_VERTEX_COUNT; ++i)
			{
				const wchar_t* index[3] =
				{
					wcstok_s(indexSet[i], L"/", &nextToken),
					wcstok_s(nullptr, L"/", &nextToken),
					wcstok_s(nullptr, L"/", &nextToken)
				};

				wchar_t* toulEnd;
				uint32_t vIndex;
				uint32_t vtIndex;
				uint32_t vnIndex;

				switch (vft)
				{
				case VERTEX_FORMAT_TYPE::POSITION:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					{
						const IndexSet::Position item(vIndex);
						auto iter = imp.vIndexMap.find(item);
						if (iter == imp.vIndexMap.cend())
						{
							VFPosition vData(vp.v[vIndex]);
							tempVB.push_back(&vData, sizeof(vData));

							const uint32_t vbIndex = static_cast<uint32_t>(tempVB.bytesize() / sizeof(vData) - 1);
							imp.vIndexMap.insert(std::make_pair(item, vbIndex));
							tempIB.push_back(vbIndex);
						}
						else
						{
							tempIB.push_back(iter->second);
						}
					}
					break;
				case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vnIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionNormal item(vIndex, vnIndex);
						auto iter = imp.vvnIndexMap.find(item);
						if (iter == imp.vvnIndexMap.cend())
						{
							VFPositionNormal vData(vp.v[vIndex], vp.vn[vnIndex]);
							tempVB.push_back(&vData, sizeof(vData));

							const uint32_t vbIndex = static_cast<uint32_t>(tempVB.bytesize() / sizeof(vData) - 1);
							imp.vvnIndexMap.insert(std::make_pair(item, vbIndex));
							tempIB.push_back(vbIndex);
						}
						else
						{
							tempIB.push_back(iter->second);
						}
					}
					break;
				case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vtIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionTexCoord item(vIndex, vtIndex);
						auto iter = imp.vvtIndexMap.find(item);
						if (iter == imp.vvtIndexMap.cend())
						{
							VFPositionTexCoord vData(vp.v[vIndex], vp.vt[vtIndex]);
							tempVB.push_back(&vData, sizeof(vData));

							const uint32_t vbIndex = static_cast<uint32_t>(tempVB.bytesize() / sizeof(vData) - 1);
							imp.vvtIndexMap.insert(std::make_pair(item, vbIndex));
							tempIB.push_back(vbIndex);
						}
						else
						{
							tempIB.push_back(iter->second);
						}
					}
					break;
				case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vtIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					vnIndex = wcstoul(index[2], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionNormalTexCoord item(vIndex, vnIndex, vtIndex);
						auto iter = imp.vvnvtIndexMap.find(item);
						if (iter == imp.vvnvtIndexMap.cend())
						{
							VFPositionNormalTexCoord vData(vp.v[vIndex], vp.vn[vnIndex], vp.vt[vtIndex]);
							tempVB.push_back(&vData, sizeof(vData));

							const uint32_t vbIndex = static_cast<uint32_t>(tempVB.bytesize() / sizeof(vData) - 1);
							imp.vvnvtIndexMap.insert(std::make_pair(item, vbIndex));
							tempIB.push_back(vbIndex);
						}
						else
						{
							tempIB.push_back(iter->second);
						}
					}
					break;
				}
			}
		}
		else
		{
			ret_nf = true;
			*pnffpos = fpos;
			break;
		}

		fpos = ftell(pOBJFile);
	}

	currentSubset.m_indexCount = indexCount;		// Set index count

	return ret_nf;
}

bool Resource::ReadObject_deprecated(FILE* pOBJFile, long* pofpos, VertexPack& vp, Mesh& mesh, const size_t meshIndex,
	std::vector<DeferredMtlLinkingData>& dml)
{
	wchar_t line[MAX_LINE_LENGTH];
	wchar_t mtlName[MAX_LINE_LENGTH - sizeof(L"usemtl ")];
	wchar_t* nextToken = nullptr;
	wchar_t* wcstoxEnd;
	const wchar_t* token;
	SubsetAttribute sa;
	sa.m_shadeSmooth = false;
	sa.m_mtlName = nullptr;
	bool ret_o = false;
	VERTEX_FORMAT_TYPE vft = VERTEX_FORMAT_TYPE::UNKNOWN;
	RawVector tempVB;
	std::vector<uint32_t> tempIB;

	IndexMapPack imp;

	long fpos = ftell(pOBJFile);
	while (fgetws(line, _countof(line), pOBJFile))
	{
		token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);
		const wchar_t firstChar = token ? token[0] : L'\0';

		if (!token || firstChar == L'#')
		{
		}
		else if (firstChar == L'o' && !wcscmp(token, L"o"))
		{
			ret_o = true;
			*pofpos = fpos;
			break;
		}
		else if (firstChar == L's' && !wcscmp(token, L"s"))
		{
			token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
			if ((token ? token[0] : L'\0') == L'0')
				sa.m_shadeSmooth = false;
			else
				sa.m_shadeSmooth = true;
		}
		else if (firstChar == L'u' && !wcscmp(token, L"usemtl"))
		{
			token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
			if (token)
			{
				StringCbCopyW(mtlName, sizeof(mtlName), token);
				sa.m_mtlName = mtlName;
			}
			else
			{
				sa.m_mtlName = nullptr;
			}
		}
		else if (firstChar == L'v')
		{
			if (!wcscmp(token, L"v"))
			{
				// Vertex Position
				XMFLOAT3 position;
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				position.x = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				position.y = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				position.z = -wcstof(token, &wcstoxEnd);		// import right hand coord system model
				vp.v.push_back(position);
			}
			else if (!wcscmp(token, L"vt"))
			{
				// Vertex TexCoord
				XMFLOAT2 uv;
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				uv.x = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				uv.y = 1.0f - wcstof(token, &wcstoxEnd);		// import right hand coord system model
				vp.vt.push_back(uv);
			}
			else if (!wcscmp(token, L"vn"))
			{
				// Vertex Normal
				XMFLOAT3 normal;
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				normal.x = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				normal.y = wcstof(token, &wcstoxEnd);
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				normal.z = -wcstof(token, &wcstoxEnd);			// import right hand coord system model
				vp.vn.push_back(normal);
			}
		}
		else if (firstChar == L'f' && !wcscmp(token, L"f"))
		{
			// Vertex Format Layout 구하기 (Object의 Subset마다 서로 다른 Vertex Layout을 가지는 것은 불가능하다고 간주)
			// 메시의 Vertex Format Layout을 아직 모르는 경우 최초 1회 레이아웃을 파악한다.
			if (vft == VERTEX_FORMAT_TYPE::UNKNOWN)
			{
				const wchar_t* firstIndexSet = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				const wchar_t* firstSlash = wcschr(firstIndexSet, L'/');
				const wchar_t* lastSlash = wcsrchr(firstIndexSet, L'/');
				if (!firstSlash)							// v
					vft = VERTEX_FORMAT_TYPE::POSITION;
				else if (firstSlash == lastSlash)			// v/vt
					vft = VERTEX_FORMAT_TYPE::POSITION_TEXCOORD;
				else if (firstSlash + 1 == lastSlash)		// v//vn
					vft = VERTEX_FORMAT_TYPE::POSITION_NORMAL;
				else										// v/vt/vn
					vft = VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD;

				mesh.m_vft = vft;
			}

			fseek(objFile, fpos, SEEK_SET);
			long nffpos;
			if (Resource::ReadFaces_deprecated(pOBJFile, &nffpos, vft, vp, imp, mesh, meshIndex, tempVB, tempIB, dml, sa))
				fseek(objFile, nffpos, SEEK_SET);
		}

		fpos = ftell(pOBJFile);	// save file stream pointer
	}

	assert(mesh.m_vft != VERTEX_FORMAT_TYPE::UNKNOWN);

	// Create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initialData;

	bufferDesc.ByteWidth = static_cast<UINT>(tempVB.bytesize());
	bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = InputLayoutHelper::GetStructureByteStride(vft);

	initialData.pSysMem = tempVB.data();
	// initialData.SysMemPitch = 0;		// unused
	// initialData.SysMemSlicePitch = 0;	// unused

	if (FAILED(Graphics::GetDevice()->CreateBuffer(&bufferDesc, &initialData, mesh.m_vb.GetAddressOf())))
	{
		AsyncLogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
		StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, L"Failed to create a vertex buffer!\n");
		AsyncFileLogger::Write(pLogBuffer);
	}

	// Create an index buffer
	bufferDesc.ByteWidth = static_cast<UINT>(tempIB.size() * sizeof(uint32_t));
	bufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	// bufferDesc.CPUAccessFlags = 0;
	// bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(uint32_t);
	initialData.pSysMem = tempIB.data();
	// initialData.SysMemPitch = 0;		// unused
	// initialData.SysMemSlicePitch = 0;	// unused

	if (FAILED(Graphics::GetDevice()->CreateBuffer(&bufferDesc, &initialData, mesh.m_ib.GetAddressOf())))
	{
		AsyncLogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
		StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, L"Failed to create an index buffer!\n");
		AsyncFileLogger::Write(pLogBuffer);
	}

	return ret_o;
}

std::vector<std::shared_ptr<Mesh>> Resource::LoadWavefrontOBJ_deprecated(PCWSTR path, bool importTexture)
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	VertexPack vp;

	errno_t e;
	wchar_t filePath[MAX_PATH];
	filePath[0] = L'\0';
	wchar_t texPath[MAX_PATH];
	StringCbCopyW(texPath, sizeof(texPath), path);
	wchar_t* rchr = wcsrchr(texPath, L'\\');
	wchar_t* texPathInitPtr;
	if (rchr)		// Resource\etc\...\...\xxx.png
	{
		texPathInitPtr = rchr + 1;
		*texPathInitPtr = L'\0';
	}
	else			// Resource\xxx.png
	{
		texPathInitPtr = texPath;
		*texPathInitPtr = L'\0';
	}

	// Open OBJ file
	e = wcscpy_s(filePath, FileSystem::GetInstance()->GetResourcePath());
	if (e != 0)
		return meshes;

	e = wcscat_s(filePath, path);
	if (e != 0)
		return meshes;

	FILE* file = nullptr;
	e = _wfopen_s(&file, filePath, L"rt, ccs=UTF-8");
	if (e != 0)
		return meshes;

	// OBJ parsing
	wchar_t line[MAX_LINE_LENGTH];
	wchar_t* nextToken = nullptr;
	wchar_t* convEnd;
	const wchar_t* token;
	bool usemtllib = false;
	std::vector<DeferredMtlLinkingData> dml;

	while (fgetws(line, _countof(line), file))
	{
		token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);
		const wchar_t firstChar = token ? token[0] : L'\0';

		if (!token || firstChar == L'#')
			continue;

		if (firstChar == L'o' && !wcscmp(token, L"o"))
		{
			token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);		// token = object name
			if (token != nullptr)
			{
				meshes.emplace_back(std::make_shared<Mesh>(token));
				const size_t meshIndex = meshes.size() - 1;
				Mesh& mesh = *meshes[meshIndex].get();
				long ofpos;
				if (Resource::ReadObject_deprecated(file, &ofpos, vp, mesh, meshIndex, dml))
					fseek(file, ofpos, SEEK_SET);
			}
		}
		else if (firstChar == L'm' && !wcscmp(token, L"mtllib"))
		{
			token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);		// token = mtl file name
			if (token != nullptr)
			{
				*(wcsrchr(filePath, L'\\') + 1) = L'\0';
				StringCbCatW(filePath, sizeof(filePath), token);
				usemtllib = true;
			}
		}
	}

	if (file != nullptr)
	{
		fclose(file);
		file = nullptr;
	}

	std::unordered_map<std::wstring, std::shared_ptr<Material>> mtls;
	// Open MTL file
	if (usemtllib)
	{
		e = _wfopen_s(&file, filePath, L"rt, ccs=UTF-8");
		if (e != 0)
		{
			meshes.clear();
			return meshes;
		}

		// MTL parsing
		std::shared_ptr<Material> currentMaterial;
		while (fgetws(line, _countof(line), file))
		{
			token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);

			if (!token || token[0] == L'#')
			{
			}
			else if (!wcscmp(token, L"newmtl"))
			{
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);	// token = material name
				if (!token)
				{
					currentMaterial.reset();
					continue;
				}
				else
				{
					auto insertedPair = mtls.emplace(std::wstring(token), std::make_shared<Material>());
					assert(insertedPair.second == true);
					currentMaterial = insertedPair.first->second;
				}
			}
			else if (!wcscmp(token, L"Ns"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					const FLOAT p = wcstof(token, &convEnd);

					currentMaterial->m_specular.w = p;
				}
			}
			else if (!wcscmp(token, L"Ka"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_ambient.x = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_ambient.y = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_ambient.z = wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"Kd"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_diffuse.x = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_diffuse.y = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_diffuse.z = wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"Ks"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_specular.x = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_specular.y = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_specular.z = wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"d"))		// 0.0 = 완전 투명, 1.0 = 완전 불투명
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_diffuse.w = wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"Tr"))		// 1.0 = 완전 투명, 0.0 = 완전 불투명
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_diffuse.w = 1.0f - wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"illum"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					const UINT illum = wcstol(token, &convEnd, 10);

					currentMaterial->m_illum = illum;
				}
			}
			else if (!wcscmp(token, L"map_Kd") && importTexture)
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);

					*texPathInitPtr = L'\0';
					StringCbCatW(texPath, sizeof(texPath), token);

					currentMaterial->m_diffuseMap = Resource::LoadTexture_deprecated(texPath);
				}
				}
			else
			{
				// Unknown keyword
				}
		}

		// Link deferred subsets
		for (const auto& waiting : dml)
		{
			const auto iter = mtls.find(waiting.m_mtlName);
			assert(iter != mtls.end());
			if (iter == mtls.end())
			{
				AsyncLogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
				StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE,
					L"Cannot find material name '%s' in mtl file.", waiting.m_mtlName.c_str());
				AsyncFileLogger::Write(pLogBuffer);
			}
			else
			{
				meshes[waiting.m_meshIndex]->m_subsets[waiting.m_subsetIndex].m_material = iter->second;
			}
		}
	}

	if (file != nullptr)
	{
		fclose(file);
		file = nullptr;
	}

	return meshes;
}

std::vector<std::shared_ptr<Mesh>> Resource::LoadWavefrontOBJ(const wchar_t* path, bool importTexture)
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	VertexPack vp;

	errno_t e;
	wchar_t filePath[MAX_PATH];
	filePath[0] = L'\0';
	wchar_t texPath[MAX_PATH];
	StringCbCopyW(texPath, sizeof(texPath), path);
	wchar_t* rchr = wcsrchr(texPath, L'\\');
	wchar_t* texPathInitPtr;
	if (rchr)		// Resource\etc\...\...\xxx.png
	{
		texPathInitPtr = rchr + 1;
		*texPathInitPtr = L'\0';
	}
	else			// Resource\xxx.png
	{
		texPathInitPtr = texPath;
		*texPathInitPtr = L'\0';
	}

	// Open OBJ file
	e = wcscpy_s(filePath, FileSystem::GetInstance()->GetResourcePath());
	if (e != 0)
		return meshes;

	e = wcscat_s(filePath, path);
	if (e != 0)
		return meshes;

	FILE* file = nullptr;
	e = _wfopen_s(&file, filePath, L"rt, ccs=UTF-8");
	if (e != 0)
		return meshes;

	// OBJ parsing
	wchar_t line[MAX_LINE_LENGTH];
	wchar_t* nextToken = nullptr;
	wchar_t* convEnd;
	const wchar_t* token;
	bool usemtllib = false;
	std::vector<DeferredMtlLinkingData> dml;

	while (fgetws(line, _countof(line), file))
	{
		token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);
		const wchar_t firstChar = token ? token[0] : L'\0';

		if (!token || firstChar == L'#')
			continue;

		if (firstChar == L'o' && !wcscmp(token, L"o"))
		{
			token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);		// token = object name
			if (token != nullptr)
			{
				meshes.emplace_back(std::make_shared<Mesh>(token));
				const size_t meshIndex = meshes.size() - 1;
				Mesh& mesh = *meshes[meshIndex].get();
				long ofpos;
				if (Resource::ParseWavefrontOBJObject(file, &ofpos, vp, mesh, meshIndex, dml))
					fseek(file, ofpos, SEEK_SET);
			}
		}
		else if (firstChar == L'm' && !wcscmp(token, L"mtllib"))
		{
			token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);		// token = mtl file name
			if (token != nullptr)
			{
				*(wcsrchr(filePath, L'\\') + 1) = L'\0';
				StringCbCatW(filePath, sizeof(filePath), token);
				usemtllib = true;
			}
		}
	}

	if (file != nullptr)
	{
		fclose(file);
		file = nullptr;
	}

	std::unordered_map<std::wstring, std::shared_ptr<Material>> mtls;
	// Open MTL file
	if (usemtllib)
	{
		e = _wfopen_s(&file, filePath, L"rt, ccs=UTF-8");
		if (e != 0)
		{
			meshes.clear();
			return meshes;
		}

		// MTL parsing
		std::shared_ptr<Material> currentMaterial;
		while (fgetws(line, _countof(line), file))
		{
			token = wcstok_s(line, OBJ_MTL_DELIM, &nextToken);

			if (!token || token[0] == L'#')
			{
			}
			else if (!wcscmp(token, L"newmtl"))
			{
				token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);	// token = material name
				if (!token)
				{
					currentMaterial.reset();
					continue;
				}
				else
				{
					auto insertedPair = mtls.emplace(std::wstring(token), std::make_shared<Material>());
					assert(insertedPair.second == true);
					currentMaterial = insertedPair.first->second;
				}
			}
			else if (!wcscmp(token, L"Ns"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					const FLOAT p = wcstof(token, &convEnd);

					currentMaterial->m_color.specular.w = p;
				}
			}
			else if (!wcscmp(token, L"Ka"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.ambient.x = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.ambient.y = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.ambient.z = wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"Kd"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.diffuse.x = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.diffuse.y = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.diffuse.z = wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"Ks"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.specular.x = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.specular.y = wcstof(token, &convEnd);
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.specular.z = wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"d"))		// 0.0 = 완전 투명, 1.0 = 완전 불투명
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.diffuse.w = wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"Tr"))		// 1.0 = 완전 투명, 0.0 = 완전 불투명
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					currentMaterial->m_color.diffuse.w = 1.0f - wcstof(token, &convEnd);
				}
			}
			else if (!wcscmp(token, L"illum"))
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
					const UINT illum = wcstol(token, &convEnd, 10);

					currentMaterial->SetIlluminationModel(illum);
				}
			}
			else if (!wcscmp(token, L"map_Kd") && importTexture)
			{
				if (currentMaterial)
				{
					token = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);

					*texPathInitPtr = L'\0';
					StringCbCatW(texPath, sizeof(texPath), token);

					currentMaterial->m_diffuseTexture = Resource::LoadTexture(texPath);
				}
			}
			else
			{
				// Unknown keyword
			}
		}

		// Link deferred subsets
		for (const auto& waiting : dml)
		{
			const auto iter = mtls.find(waiting.m_mtlName);
			assert(iter != mtls.end());
			if (iter == mtls.end())
			{
				AsyncLogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
				StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE,
					L"Cannot find material name '%s' in mtl file.", waiting.m_mtlName.c_str());
				AsyncFileLogger::Write(pLogBuffer);
			}
			else
			{
				meshes[waiting.m_meshIndex]->m_subsets[waiting.m_subsetIndex].m_material = iter->second;
			}
		}
	}

	if (file != nullptr)
	{
		fclose(file);
		file = nullptr;
	}

	return meshes;
}

std::shared_ptr<ShaderResourceTexture> Resource::LoadTexture_deprecated(PCWSTR path)
{
	bool useExistingContainer = false;
	const auto iter = Resource::sTextureMap.find(path);
	if (iter != Resource::sTextureMap.cend())
	{
		std::shared_ptr<ShaderResourceTexture> texture = iter->second.lock();

		if (texture)
			return texture;
		else
		{
			iter->second.reset();
			useExistingContainer = true;
		}
	}

	wchar_t filePath[MAX_PATH];
	StringCbCopyW(filePath, sizeof(filePath), FileSystem::GetInstance()->GetResourcePath());
	StringCbCatW(filePath, sizeof(filePath), path);
	const wchar_t* ext = wcsrchr(filePath, L'.');

	if (!ext)
	{
		AsyncLogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
		StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, L"[Resource::LoadTexture] Invalid file extension. %s\n", path);
		AsyncFileLogger::Write(pLogBuffer);

		return nullptr;
	}

	// 이미지 로드
	HRESULT hr = S_OK;
	ScratchImage image;

	if (!wcscmp(L".dds", ext) || !wcscmp(L".DDS", ext))
		hr = LoadFromDDSFile(filePath, DDS_FLAGS::DDS_FLAGS_NONE, nullptr, image);	// dds, DDS
	else if (!wcscmp(L".tga", ext) || !wcscmp(L".TGA", ext))
		hr = LoadFromTGAFile(filePath, nullptr, image);								// tga, TGA
	else
		hr = LoadFromWICFile(filePath, WIC_FLAGS::WIC_FLAGS_NONE, nullptr, image);	// png, jpg, jpeg, bmp, ...

	if (FAILED(hr))
	{
		AsyncLogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
		StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, L"[DirectX::LoadFromXXXFile] Failed. %ld\n", hr);
		AsyncFileLogger::Write(pLogBuffer);

		return nullptr;
	}

	// 밉맵 생성
	ScratchImage mipChain;
	// gif 등 여러 이미지 포함 가능 포맷은 현재 고려하지 않음
	hr = GenerateMipMaps(image.GetImages(), 1, image.GetMetadata(), TEX_FILTER_DEFAULT, 0, mipChain);
	// 첫 번째 이미지만 있다고 가정하고 해당 이미지에 대한 밉맵 생성
	if (FAILED(hr))
	{
		AsyncLogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
		StringCbPrintfW(pLogBuffer->buffer, AsyncLogBuffer::BUFFER_SIZE, L"[DirectX::GenerateMipMaps] Failed. %ld\n", hr);
		AsyncFileLogger::Write(pLogBuffer);

		return nullptr;
	}

	std::shared_ptr<ShaderResourceTexture> texture = std::make_shared<ShaderResourceTexture>();

	// 디스크립터 세팅
	const TexMetadata& metadata = mipChain.GetMetadata();
	D3D11_TEXTURE2D_DESC descTexture;
	ZeroMemory(&descTexture, sizeof(descTexture));
	descTexture.Width = static_cast<UINT>(metadata.width);
	descTexture.Height = static_cast<UINT>(metadata.height);
	descTexture.MipLevels = static_cast<UINT>(metadata.mipLevels);
	descTexture.ArraySize = 1;
	descTexture.Format = static_cast<DXGI_FORMAT>(metadata.format);
	descTexture.SampleDesc.Count = 1;
	descTexture.SampleDesc.Quality = 0;
	descTexture.Usage = D3D11_USAGE_DEFAULT;
	descTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE;		// 렌더 타겟용은 아님, 셰이더 리소스 용도로 생성

	// 텍스처 생성
	hr = Graphics::GetDevice()->CreateTexture2D(&descTexture, nullptr, texture->mTexture.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return nullptr;
	}

	// D3D 텍스쳐에 밉맵 복사		(이미지 한 장의 밉맵만 모두)
	for (UINT i = 0; i < static_cast<UINT>(metadata.mipLevels); ++i)
	{
		const Image* pImg = mipChain.GetImage(i, 0, 0);
		Graphics::GetImmediateContext()->UpdateSubresource(
			texture->mTexture.Get(),
			D3D11CalcSubresource(i, 0, static_cast<UINT>(metadata.mipLevels)),
			nullptr,
			pImg->pixels,
			static_cast<UINT>(pImg->rowPitch),
			static_cast<UINT>(pImg->slicePitch)
		);
	}

	// 셰이더 리소스 뷰 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	ZeroMemory(&descSRV, sizeof(descSRV));
	descSRV.Format = descTexture.Format;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MipLevels = descTexture.MipLevels;
	hr = Graphics::GetDevice()->CreateShaderResourceView(texture->mTexture.Get(), &descSRV, texture->m_srv.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return nullptr;
	}

	if (useExistingContainer)
		iter->second = texture;
	else
		Resource::sTextureMap.emplace(path, texture);

	return texture;
}

void Resource::ReleaseExpiredContainer()
{
	auto iter = Resource::sTextureMap.begin();
	auto end = Resource::sTextureMap.cend();

	while (iter != end)
	{
		if (iter->second.expired())
			iter = Resource::sTextureMap.erase(iter);
		else
			++iter;
	}
}
*/
