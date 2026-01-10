#include <ZergEngine\CoreSystem\ResourceLoader.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\FileSystem.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\DataStructure\RawVector.h>
#include <ZergEngine\CoreSystem\Resource\Armature.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>
#include <ZergEngine\CoreSystem\Resource\BoneIndexType.h>
#include <ZergEngine\CoreSystem\Resource\StaticMesh.h>
#include <ZergEngine\CoreSystem\Resource\SkinnedMesh.h>
#include <ZergEngine\CoreSystem\Resource\Material.h>
#include <ZergEngine\CoreSystem\Helper.h>
#include <DirectXTex\DirectXTex.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <queue>

using namespace ze;

ResourceLoader* ResourceLoader::s_pInstance = nullptr;

constexpr int MAX_LINE_LENGTH = 256;
PCWSTR OBJ_MTL_DELIM = L" \t\n";

void XMStoreFloat4x4(DirectX::XMFLOAT4X4* pOut, const aiMatrix4x4* pAiMatrix4x4)
{
	pOut->_11 = static_cast<float>(pAiMatrix4x4->a1);
	pOut->_12 = static_cast<float>(pAiMatrix4x4->b1);
	pOut->_13 = static_cast<float>(pAiMatrix4x4->c1);
	pOut->_14 = static_cast<float>(pAiMatrix4x4->d1);

	pOut->_21 = static_cast<float>(pAiMatrix4x4->a2);
	pOut->_22 = static_cast<float>(pAiMatrix4x4->b2);
	pOut->_23 = static_cast<float>(pAiMatrix4x4->c2);
	pOut->_24 = static_cast<float>(pAiMatrix4x4->d2);

	pOut->_31 = static_cast<float>(pAiMatrix4x4->a3);
	pOut->_32 = static_cast<float>(pAiMatrix4x4->b3);
	pOut->_33 = static_cast<float>(pAiMatrix4x4->c3);
	pOut->_34 = static_cast<float>(pAiMatrix4x4->d3);

	pOut->_41 = static_cast<float>(pAiMatrix4x4->a4);
	pOut->_42 = static_cast<float>(pAiMatrix4x4->b4);
	pOut->_43 = static_cast<float>(pAiMatrix4x4->c4);
	pOut->_44 = static_cast<float>(pAiMatrix4x4->d4);
}

void XMStoreFloat4x4A(DirectX::XMFLOAT4X4* pOut, const aiMatrix4x4* pAiMatrix4x4)
{
	XMStoreFloat4x4(pOut, pAiMatrix4x4);
}

DirectX::XMMATRIX XMLoadAiMatrix4x4(const aiMatrix4x4* pAiMatrix4x4)
{
	XMFLOAT4X4A xmMatrix;
	XMStoreFloat4x4A(&xmMatrix, pAiMatrix4x4);

	return XMLoadFloat4x4A(&xmMatrix);
}

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
	// 텍스쳐 로드에 실패했을 시 반환할 텍스쳐 생성 (Magenta, Black 체크무늬 텍스쳐)
	// 디스크립터 세팅
	D3D11_TEXTURE2D_DESC descTexture;
	ZeroMemory(&descTexture, sizeof(descTexture));
	constexpr UINT width = 256;
	constexpr UINT height = 256;
	descTexture.Width = width;
	descTexture.Height = height;
	descTexture.MipLevels = 0;	// 밉맵 체인 생성
	descTexture.ArraySize = 1;
	descTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descTexture.SampleDesc.Count = 1;
	descTexture.SampleDesc.Quality = 0;
	descTexture.Usage = D3D11_USAGE_DEFAULT;
	descTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;		// 셰이더 리소스 목적으로 생성 + 밉맵 생성을 위해 렌더타겟 플래그 설정
	descTexture.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 텍스처 생성 및 초기화
	struct R8G8B8A8
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	std::vector<R8G8B8A8> texels(width * height);	// DXGI_FORMAT_R8G8B8A8_UNORM
	constexpr UINT CHECK_DOT_SIZE = 32;	// 256x256에서 32x32 픽셀 크기로 번갈아가며 색상 배열
	bool isMagenta = false;
	constexpr R8G8B8A8 MAGENTA_COLOR{ 255, 0, 255, 255 };
	constexpr R8G8B8A8 BLACK_COLOR{ 0, 0, 0, 255 };
	for (UINT i = 0; i < width; i += CHECK_DOT_SIZE)
	{
		for (UINT j = 0; j < height; j += CHECK_DOT_SIZE)
		{
			const R8G8B8A8 texel = isMagenta ? MAGENTA_COLOR : BLACK_COLOR;
			for (UINT k = i; k < i + CHECK_DOT_SIZE; ++k)
			{
				for (UINT l = j; l < j + CHECK_DOT_SIZE; ++l)
				{
					texels[k * width + l] = texel;
				}
			}
			isMagenta = !isMagenta;
		}
		isMagenta = !isMagenta;
	}
	// D3D11_SUBRESOURCE_DATA initialData;
	// initialData.pSysMem = texels.data();
	// initialData.SysMemPitch = sizeof(R8G8B8A8) * width;
	// initialData.SysMemSlicePitch = 0;	// 3D 텍스쳐에서만 사용됨

	ComPtr<ID3D11Texture2D> cpTex2D;
	HRESULT hr = GraphicDevice::GetInstance()->GetDevice()->CreateTexture2D(
		&descTexture, nullptr, cpTex2D.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateTexture2D()", hr);

	GraphicDevice::GetInstance()->GetImmediateContext()->UpdateSubresource(cpTex2D.Get(), D3D11CalcSubresource(0, 0, 0),
		nullptr, texels.data(), sizeof(R8G8B8A8)* width, 0);

	// 셰이더 리소스 뷰 생성
	ComPtr<ID3D11ShaderResourceView> cpSRV;
	hr = GraphicDevice::GetInstance()->GetDevice()->CreateShaderResourceView(
		cpTex2D.Get(), nullptr, cpSRV.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateShaderResourceView()", hr);

	// 밉맵 생성
	GraphicDevice::GetInstance()->GetImmediateContext()->GenerateMips(cpSRV.Get());

	m_errTex.m_cpTex2D = std::move(cpTex2D);
	m_errTex.m_cpSRV = std::move(cpSRV);
}

void ResourceLoader::UnInit()
{
	m_errTex.Reset();
}

void ResourceLoader::DFSAiNodeLoadModel(TempModelData& tmd, const aiScene* pAiScene, const aiNode* pAiNode)
{
	// 한 노드가 가리키는 모든 aiMesh들을 하나의 ze::Mesh에 담고 각 aiMesh들은 ze::Subset에 대응시키면 된다.
	if (pAiNode->mNumMeshes > 0)
	{
		bool isSkinnedMeshNode;
		{
			// 노드 내의 aiMesh들이 하나의 StaticMesh 또는 SkinnedMesh를 구성해야 하므로
			// 전부 다 뼈를 가지고 있던지, 아니면 전부 다 뼈를 갖고 있지 않던지 해야함.
			// 이것을 검사하는 코드
			bool hasBones = pAiScene->mMeshes[pAiNode->mMeshes[0]]->HasBones();
			for (unsigned int i = 1; i < pAiNode->mNumMeshes; ++i)
			{
				if (hasBones != pAiScene->mMeshes[pAiNode->mMeshes[i]]->HasBones())
					Debug::ForceCrashWithMessageBox(L"Error", L"Mesh consistency error! (HasBones)");
			}
			
			isSkinnedMeshNode = hasBones;
		}
		
		if (isSkinnedMeshNode)
			AiLoadSkinnedMeshNode(tmd, pAiScene, pAiNode);
		else
			AiLoadStaticMeshNode(tmd, pAiScene, pAiNode);
	}

	for (unsigned int i = 0; i < pAiNode->mNumChildren; ++i)
		DFSAiNodeLoadModel(tmd, pAiScene, pAiNode->mChildren[i]);
}

void ResourceLoader::AiLoadStaticMeshNode(TempModelData& tmd, const aiScene* pAiScene, const aiNode* pAiNode)
{
	WCHAR name[32];
	name[0] = L'\0';
	Helper::ConvertUTF8ToUTF16(pAiNode->mName.C_Str(), name, _countof(name));

	using MeshVertexFormat = VFPositionNormalTangentTexCoord;

	std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(name);
	std::vector<MeshVertexFormat> vertices;
	std::vector<uint32_t> indices;
	uint32_t startIndexLocation = 0;			// 초기화

	// AABB 계산 로직을 간단하게 하기 위해서 일단 첫 번째 메시의 AABB를 담아두고 반복적으로 병합해간다.
	Aabb finalAabb;
	const aiMesh* pFirstAiMesh = pAiScene->mMeshes[pAiNode->mMeshes[0]];
	XMVECTOR min = XMVectorSet(pFirstAiMesh->mAABB.mMin.x, pFirstAiMesh->mAABB.mMin.y, pFirstAiMesh->mAABB.mMin.z, 0.0f);
	XMVECTOR max = XMVectorSet(pFirstAiMesh->mAABB.mMax.x, pFirstAiMesh->mAABB.mMax.y, pFirstAiMesh->mAABB.mMax.z, 0.0f);
	XMStoreFloat3(&finalAabb.Center, XMVectorScale(XMVectorAdd(min, max), 0.5f));
	XMStoreFloat3(&finalAabb.Extents, XMVectorScale(XMVectorAbs(XMVectorSubtract(max, min)), 0.5f));

	for (unsigned int i = 0; i < pAiNode->mNumMeshes; ++i)
	{
		const aiMesh* pAiMesh = pAiScene->mMeshes[pAiNode->mMeshes[i]];

		const uint32_t vertexIndexBase = static_cast<uint32_t>(vertices.size());	// 현재 서브셋 정점들이 담기는 시작 인덱스

		// AABB 추출
		Aabb aabb;
		XMVECTOR min = XMVectorSet(pAiMesh->mAABB.mMin.x, pAiMesh->mAABB.mMin.y, pAiMesh->mAABB.mMin.z, 0.0f);
		XMVECTOR max = XMVectorSet(pAiMesh->mAABB.mMax.x, pAiMesh->mAABB.mMax.y, pAiMesh->mAABB.mMax.z, 0.0f);
		XMStoreFloat3(&aabb.Center, XMVectorScale(XMVectorAdd(min, max), 0.5f));
		XMStoreFloat3(&aabb.Extents, XMVectorScale(XMVectorAbs(XMVectorSubtract(max, min)), 0.5f));
		Aabb::CreateMerged(finalAabb, finalAabb, aabb);

		assert(pAiMesh->mVertices != nullptr);
		assert(pAiMesh->mNormals != nullptr);
		assert(pAiMesh->mTangents != nullptr);
		assert(pAiMesh->mTextureCoords != nullptr);
		// 정점 정보 읽기
		for (unsigned int v = 0; v < pAiMesh->mNumVertices; ++v)
		{
			MeshVertexFormat vertex;

			vertex.m_position.x = pAiMesh->mVertices[v].x;
			vertex.m_position.y = pAiMesh->mVertices[v].y;
			vertex.m_position.z = pAiMesh->mVertices[v].z;

			vertex.m_normal.x = pAiMesh->mNormals[v].x;
			vertex.m_normal.y = pAiMesh->mNormals[v].y;
			vertex.m_normal.z = pAiMesh->mNormals[v].z;

			vertex.m_tangent.x = pAiMesh->mTangents[v].x;
			vertex.m_tangent.y = pAiMesh->mTangents[v].y;
			vertex.m_tangent.z = pAiMesh->mTangents[v].z;

			constexpr size_t UV_CHANNEL_INDEX = 0;
			vertex.m_texCoord.x = pAiMesh->mTextureCoords[UV_CHANNEL_INDEX][v].x;
			vertex.m_texCoord.y = pAiMesh->mTextureCoords[UV_CHANNEL_INDEX][v].y;

			vertices.push_back(vertex);
		}

		uint32_t indexCount = 0;	// 서브셋의 인덱스 개수 (드로우콜시 필요 정보)
		for (unsigned int f = 0; f < pAiMesh->mNumFaces; ++f)	// Face들을 순회
		{
			const aiFace* pAiFace = &pAiMesh->mFaces[f];
			assert(pAiFace->mNumIndices == 3);
			for (unsigned int fi = 0; fi < pAiFace->mNumIndices; ++fi)	// Face들을 구성하는 정점들에 대한 인덱스를 획득
			{
				++indexCount;
				indices.push_back(pAiFace->mIndices[fi] + vertexIndexBase);
			}
		}

		// 서브셋 정보 저장
		mesh->m_subsets.push_back(MeshSubset(indexCount, startIndexLocation));

		startIndexLocation += indexCount;	// 다음 루프 메시를 위해
	}

	// 정보 세팅
	// 1. AABB 설정
	// 부피가 없는 축의 경우 부피 생성
	XMStoreFloat3(&finalAabb.Extents, XMVectorMax(XMLoadFloat3(&finalAabb.Extents), XMVectorReplicate(BOUNDING_BOX_MIN_EXTENT)));
	mesh->m_aabb = finalAabb;

	// 2. 모든 서브셋들이 공유할 버텍스버퍼, 인덱스버퍼 생성
	HRESULT hr;

	// Create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initialData;

	bufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(MeshVertexFormat));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormalTexCoord);

	initialData.pSysMem = vertices.data();
	// initialData.SysMemPitch = 0;		// unused
	// initialData.SysMemSlicePitch = 0;	// unused

	ComPtr<ID3D11Buffer> cpVB;
	hr = GraphicDevice::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, &initialData, cpVB.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateBuffer()", hr);

	// Create an index buffer
	bufferDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// bufferDesc.CPUAccessFlags = 0;
	// bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	initialData.pSysMem = indices.data();
	// initialData.SysMemPitch = 0;			// unused
	// initialData.SysMemSlicePitch = 0;	// unused

	ComPtr<ID3D11Buffer> cpIB;
	hr = GraphicDevice::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, &initialData, cpIB.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateBuffer()", hr);

	mesh->m_cpVB = std::move(cpVB);
	mesh->m_cpIB = std::move(cpIB);

	// 3. Vertex Format Type 설정
	// mesh->m_vft = VertexFormatType::PositionNormalTangentTexCoord;

	tmd.m_staticMeshes.push_back(std::move(mesh));
}

void ResourceLoader::AiLoadAnimation(const TempModelData& tmd, const aiScene* pAiScene)
{
	// 함수에서 해야 할 일
	// TempModelData 내부에는 DFSAiNodeLoadModel & AiLoadSkinnedMeshNode 함수를 돌며 로드된 Armature들이 있다.
	// 이제 aiScene내의 Animation 클립들을 순회하며 Animation이 로드되어 있는 Armature들 중 어떤 Armature에 대한
	// 애니메이션인지 파악 후 해당 Armature의 뼈들에 뼈 키 프레임(BoneAnimation) 정보들을 추가해준다.

	for (unsigned int i = 0; i < pAiScene->mNumAnimations; ++i)
	{
		const aiAnimation* pAiAnimation = pAiScene->mAnimations[i];

		if (pAiAnimation->mNumChannels == 0)
			continue;

		// 어느 뼈대에 대한 애니메이션인지 검색
		// Armature 검색을 위한 샘플 노드 (0번 뼈를 검색해본다.)
		const aiNodeAnim* pAiNodeAnimSample = pAiAnimation->mChannels[0];

		Armature* pArmature = nullptr;

		for (size_t j = 0; j < tmd.m_armatureData.m_armatures.size(); ++j)
		{
			Armature* pCurrArmature = tmd.m_armatureData.m_armatures[j].get();

			auto iter = pCurrArmature->GetBoneIndexMap().find(pAiNodeAnimSample->mNodeName.C_Str());
			if (iter != pCurrArmature->GetBoneIndexMap().cend())
			{
				pArmature = pCurrArmature;
				break;
			}
		}

		// 뼈대 검색에 성공했어야만 함.
		assert(pArmature != nullptr);


		// 뼈대에 애니메이션 키 프레임 데이터들을 추가
		const double totalTick = pAiAnimation->mDuration;
		const double ticksPerSecond = pAiAnimation->mTicksPerSecond;
		const double duration = totalTick / ticksPerSecond;

		Animation* pAnimation = pArmature->AddNewAnimationNode(pAiAnimation->mName.C_Str(), static_cast<float>(duration));
		
		// 애니메이션의 채널(뼈)를 순회하며 뼈의 키 프레임 정보들을 저장한다.
		for (unsigned int c = 0; c < pAiAnimation->mNumChannels; ++c)
		{
			const aiNodeAnim* pAiNodeAnim = pAiAnimation->mChannels[c];

			auto iter = pArmature->GetBoneIndexMap().find(pAiNodeAnim->mNodeName.C_Str());
			assert(iter != pArmature->GetBoneIndexMap().cend());
			const bone_index_type boneIndex = iter->second;

			BoneAnimation& boneAnimation = pAnimation->m_upBoneAnims[boneIndex];
			
			for (unsigned int ski = 0; ski < pAiNodeAnim->mNumScalingKeys; ++ski)
			{
				const aiVectorKey* pAiVectorKey = &pAiNodeAnim->mScalingKeys[ski];

				ScaleKeyFrame kf;
				
				kf.m_time = static_cast<float>((pAiVectorKey->mTime / totalTick) * duration);
				kf.m_value.x = pAiVectorKey->mValue.x;
				kf.m_value.y = pAiVectorKey->mValue.y;
				kf.m_value.z = pAiVectorKey->mValue.z;

				boneAnimation.m_scaleKeyFrames.push_back(kf);
			}

			for (unsigned int rki = 0; rki < pAiNodeAnim->mNumRotationKeys; ++rki)
			{
				const aiQuatKey* pAiQuatKey = &pAiNodeAnim->mRotationKeys[rki];

				RotationKeyFrame kf;

				kf.m_time = static_cast<float>((pAiQuatKey->mTime / totalTick) * duration);
				kf.m_value.x = pAiQuatKey->mValue.x;
				kf.m_value.y = pAiQuatKey->mValue.y;
				kf.m_value.z = pAiQuatKey->mValue.z;
				kf.m_value.w = pAiQuatKey->mValue.w;

				boneAnimation.m_rotationKeyFrames.push_back(kf);
			}

			for (unsigned int pki = 0; pki < pAiNodeAnim->mNumPositionKeys; ++pki)
			{
				const aiVectorKey* pAiVectorKey = &pAiNodeAnim->mPositionKeys[pki];

				PositionKeyFrame kf;

				kf.m_time = static_cast<float>((pAiVectorKey->mTime / totalTick) * duration);
				kf.m_value.x = pAiVectorKey->mValue.x;
				kf.m_value.y = pAiVectorKey->mValue.y;
				kf.m_value.z = pAiVectorKey->mValue.z;

				boneAnimation.m_positionKeyFrames.push_back(kf);
			}
		}
	}
}

void ResourceLoader::AiLoadSkinnedMeshNode(TempModelData& tmd, const aiScene* pAiScene, const aiNode* pAiNode)
{
	// Skinned Mesh의 경우 AABB를 어떻게 계산할지?
	// 1. 직접 임의 값 설정
	// 2. 바운딩 박스 x1.25 정도로 설정

	WCHAR name[32];
	name[0] = L'\0';
	Helper::ConvertUTF8ToUTF16(pAiNode->mName.C_Str(), name, _countof(name));

	using MeshVertexFormat = VFPositionNormalTangentTexCoordSkinned;

	std::shared_ptr<SkinnedMesh> mesh = std::make_shared<SkinnedMesh>(name);
	std::vector<MeshVertexFormat> vertices;
	std::vector<uint32_t> indices;
	uint32_t startIndexLocation = 0;			// 초기화

	// AABB 계산 로직을 간단하게 하기 위해서 일단 첫 번째 메시의 AABB를 담아두고 반복적으로 병합해간다.
	Aabb finalAabb;
	const aiMesh* pFirstAiMesh = pAiScene->mMeshes[pAiNode->mMeshes[0]];
	XMVECTOR min = XMVectorSet(pFirstAiMesh->mAABB.mMin.x, pFirstAiMesh->mAABB.mMin.y, pFirstAiMesh->mAABB.mMin.z, 0.0f);
	XMVECTOR max = XMVectorSet(pFirstAiMesh->mAABB.mMax.x, pFirstAiMesh->mAABB.mMax.y, pFirstAiMesh->mAABB.mMax.z, 0.0f);
	XMStoreFloat3(&finalAabb.Center, XMVectorScale(XMVectorAdd(min, max), 0.5f));
	XMStoreFloat3(&finalAabb.Extents, XMVectorScale(XMVectorAbs(XMVectorSubtract(max, min)), 0.5f));

	std::vector<uint32_t> subsetVertexBaseIndex;
	for (unsigned int i = 0; i < pAiNode->mNumMeshes; ++i)
	{
		const aiMesh* pAiMesh = pAiScene->mMeshes[pAiNode->mMeshes[i]];

		const uint32_t vertexIndexBase = static_cast<uint32_t>(vertices.size());	// 현재 서브셋 정점들이 담기는 시작 인덱스
		subsetVertexBaseIndex.push_back(vertexIndexBase);

		// AABB 추출
		Aabb aabb;
		XMVECTOR min = XMVectorSet(pAiMesh->mAABB.mMin.x, pAiMesh->mAABB.mMin.y, pAiMesh->mAABB.mMin.z, 0.0f);
		XMVECTOR max = XMVectorSet(pAiMesh->mAABB.mMax.x, pAiMesh->mAABB.mMax.y, pAiMesh->mAABB.mMax.z, 0.0f);
		XMStoreFloat3(&aabb.Center, XMVectorScale(XMVectorAdd(min, max), 0.5f));
		XMStoreFloat3(&aabb.Extents, XMVectorScale(XMVectorAbs(XMVectorSubtract(max, min)), 0.5f));
		Aabb::CreateMerged(finalAabb, finalAabb, aabb);

		assert(pAiMesh->mVertices != nullptr);
		assert(pAiMesh->mNormals != nullptr);
		assert(pAiMesh->mTangents != nullptr);
		assert(pAiMesh->mTextureCoords != nullptr);
		// 정점 정보 읽기
		for (unsigned int v = 0; v < pAiMesh->mNumVertices; ++v)
		{
			MeshVertexFormat vertex;

			vertex.m_position.x = pAiMesh->mVertices[v].x;
			vertex.m_position.y = pAiMesh->mVertices[v].y;
			vertex.m_position.z = pAiMesh->mVertices[v].z;

			vertex.m_normal.x = pAiMesh->mNormals[v].x;
			vertex.m_normal.y = pAiMesh->mNormals[v].y;
			vertex.m_normal.z = pAiMesh->mNormals[v].z;

			vertex.m_tangent.x = pAiMesh->mTangents[v].x;
			vertex.m_tangent.y = pAiMesh->mTangents[v].y;
			vertex.m_tangent.z = pAiMesh->mTangents[v].z;

			constexpr size_t UV_CHANNEL_INDEX = 0;
			vertex.m_texCoord.x = pAiMesh->mTextureCoords[UV_CHANNEL_INDEX][v].x;
			vertex.m_texCoord.y = pAiMesh->mTextureCoords[UV_CHANNEL_INDEX][v].y;

			vertices.push_back(vertex);
		}

		uint32_t indexCount = 0;	// 서브셋의 인덱스 개수 (드로우콜시 필요 정보)
		for (unsigned int f = 0; f < pAiMesh->mNumFaces; ++f)	// Face들을 순회
		{
			const aiFace* pAiFace = &pAiMesh->mFaces[f];
			assert(pAiFace->mNumIndices == 3);
			for (unsigned int fi = 0; fi < pAiFace->mNumIndices; ++fi)	// Face들을 구성하는 정점들에 대한 인덱스를 획득
			{
				++indexCount;
				indices.push_back(pAiFace->mIndices[fi] + vertexIndexBase);
			}
		}

		// 서브셋 정보 저장
		mesh->m_subsets.push_back(MeshSubset(indexCount, startIndexLocation));

		startIndexLocation += indexCount;	// 다음 루프 메시를 위해
	}



	// ###################################################################
	// 루트 뼈 찾기 및 본 인덱스 맵 생성
	std::unordered_map<std::string, const aiBone*> allBones;
	for (unsigned int i = 0; i < pAiNode->mNumMeshes; ++i)
	{
		const aiMesh* pAiMesh = pAiScene->mMeshes[pAiNode->mMeshes[i]];

		for (unsigned int j = 0; j < pAiMesh->mNumBones; ++j)
		{
			const aiBone* pAiBone = pAiMesh->mBones[j];

			std::string boneName(pAiBone->mName.C_Str());
			allBones.insert(std::make_pair(std::move(boneName), pAiBone));
		}
	}
	assert(allBones.size() < (std::numeric_limits<bone_index_type>::max)());

	const aiNode* pAiRootBoneNode = nullptr;
	auto end = allBones.cend();
	for (auto iter = allBones.cbegin(); iter != end; ++iter)
	{
		const aiNode* pCurrentAiBoneNode = iter->second->mNode;
		const aiNode* pParentNode = pCurrentAiBoneNode->mParent;
		
		if (pParentNode != nullptr)
		{
			auto found = allBones.find(pParentNode->mName.C_Str());	// 부모 노드를 본 맵에서 검색(만약 검색에 실패하면 루트 노드로 볼 수 있음)
			if (found == allBones.cend())
			{
				pAiRootBoneNode = pCurrentAiBoneNode;
				break;
			}
		}
	}

	const size_t boneCount = allBones.size();
	assert(boneCount <= (std::numeric_limits<bone_index_type>::max)() - 1);

	bool armatureAlreadyLoaded;
	const char* armatureNodeName = pAiRootBoneNode->mParent->mName.C_Str();	// 블렌더 기준 루트 본의 부모가 Armature 노드임.
	if (tmd.m_armatureData.m_armatureNodeNames.find(armatureNodeName) != tmd.m_armatureData.m_armatureNodeNames.cend())
		armatureAlreadyLoaded = true;
	else
		armatureAlreadyLoaded = false;


	// 아래부터 Armature 데이터를 만들어내는 핵심 부분!
	// 다른 메시에 의해서 이미 뼈대 객체가 만들어진 경우에는 새로 만들지 않음 (서로 다른 메시가 동일한 뼈대를 참조하고 있는 경우에 해당)
	if (armatureAlreadyLoaded == false)
	{
		// Armature를 위한 데이터 할당
		std::unordered_map<std::string, bone_index_type> boneIndexMap;
		boneIndexMap.reserve(boneCount);
		std::unique_ptr<bone_index_type[]> upBoneHierarchy = std::make_unique<bone_index_type[]>(boneCount);
		XMFLOAT4X4A* pMdInvArray = static_cast<XMFLOAT4X4A*>(_aligned_malloc_dbg(sizeof(XMFLOAT4X4A) * boneCount, 16, __FILE__, __LINE__));
#ifndef _DEBUG
		std::unique_ptr<XMFLOAT4X4A[], void(*)(void*)> upMdInvArray(pMdInvArray, _aligned_free);	// Character space -> Bone space
#else
		std::unique_ptr<XMFLOAT4X4A[], void(*)(void*)> upMdInvArray(pMdInvArray, _aligned_free_dbg);	// Character space -> Bone space
#endif

		// Bone Index Map, MdInvArray, Bone Hierarchy 정보를 생성
		// 본 큐에 루트 본을 넣고 시작한다.
		size_t currBoneIndex = 0;
		std::queue<const aiNode*> boneNodeQueue;
		boneNodeQueue.push(pAiRootBoneNode);
		while (!boneNodeQueue.empty())
		{
			assert(currBoneIndex <= (std::numeric_limits<bone_index_type>::max)());


			const aiNode* pCurrBoneNode = boneNodeQueue.front();
			boneNodeQueue.pop();


			// ########################################################################
			// 1. 본 계층 구조를 채워나간다.
			auto found = boneIndexMap.find(pCurrBoneNode->mParent->mName.C_Str());	// A. 현재 뼈의 부모 뼈의 인덱스를 본 인덱스 맵에서 찾는다.
			if (found == boneIndexMap.cend())	// B. 만약 본 인덱스 맵에서 부모 뼈를 검색에 실패한 경우 (이 경우는 현재 본이 루트 본인 경우 뿐이다.)
			{
				// 루트 뼈인 경우는 본 계층에 자기 자신의 인덱스를 그대로 담아둔다.
				// 이렇게 하면 boneHierarchy[index] == index 조건이 참인 경우 루트 뼈임을 알 수 있음.
				upBoneHierarchy[currBoneIndex] = static_cast<bone_index_type>(currBoneIndex);
			}
			else	// C. 부모 본의 인덱스를 찾은 경우 부모 본의 인덱스를 기록해둔다.
			{
				upBoneHierarchy[currBoneIndex] = found->second;
			}
			// ########################################################################


			// ########################################################################
			// 2. MdInv 배열을 채워나간다.
			// Character space(Default pose) -> Bone space 행렬
			XMStoreFloat4x4A(&upMdInvArray[currBoneIndex], &allBones[pCurrBoneNode->mName.C_Str()]->mOffsetMatrix);
			// ########################################################################


			// ########################################################################
			// 3. 본 인덱스 맵도 채워나간다.
			boneIndexMap.insert(std::make_pair(std::string(pCurrBoneNode->mName.C_Str()), static_cast<bone_index_type>(currBoneIndex)));
			// ########################################################################


			for (unsigned int i = 0; i < pCurrBoneNode->mNumChildren; ++i)
				boneNodeQueue.push(pCurrBoneNode->mChildren[i]);

			++currBoneIndex;
		}
		// ###################################################################

		// 생성된 본 인덱스 맵을 바탕으로 스키닝 데이터 세팅
		for (unsigned int i = 0; i < pAiNode->mNumMeshes; ++i)
		{
			const aiMesh* pAiMesh = pAiScene->mMeshes[pAiNode->mMeshes[i]];	// 서브셋

			for (unsigned int j = 0; j < pAiMesh->mNumBones; ++j)
			{
				const aiBone* pAiBone = pAiMesh->mBones[j];

				const std::string boneName(pAiBone->mName.C_Str());

				auto iter = boneIndexMap.find(boneName);
				assert(iter != boneIndexMap.end());
				const bone_index_type boneIndex = iter->second;

				for (unsigned int k = 0; k < pAiBone->mNumWeights; ++k)
				{
					const aiVertexWeight* pAiVertexWeight = &pAiBone->mWeights[k];

					const uint32_t vertexIndex = pAiVertexWeight->mVertexId + subsetVertexBaseIndex[i];	// i번째 서브셋의 정점들의 베이스 인덱스 값을 더해줘야 함.
					const float weight = pAiVertexWeight->mWeight;

					assert(vertexIndex < vertices.size());
					bool ret = vertices[vertexIndex].AddSkinningData(boneIndex, weight);
					assert(ret == true);
				}
			}
		}

		// Armature 객체 생성 및 뼈대 정보 전달
		std::shared_ptr<Armature> armature = std::make_shared<Armature>(
			std::move(boneIndexMap),
			std::move(upBoneHierarchy),
			std::move(upMdInvArray)
		);

		tmd.m_armatureData.m_armatureNodeNames.insert(armatureNodeName);	// Armature 중복 생성 방지를 위해 기록
		tmd.m_armatureData.m_armatures.push_back(std::move(armature));		// Armature 저장
	}

	// 정보 세팅
	// 1. AABB 설정
	// mesh->m_aabb = finalAabb;

	// 2. 최종적으로 모든 서브셋들이 공유할 버텍스버퍼, 인덱스버퍼 생성
	HRESULT hr;

	// Create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initialData;

	bufferDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(MeshVertexFormat));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = InputLayoutHelper::GetStructureByteStride(VertexFormatType::PositionNormalTangentTexCoordSkinned);

	initialData.pSysMem = vertices.data();
	// initialData.SysMemPitch = 0;		// unused
	// initialData.SysMemSlicePitch = 0;	// unused

	ComPtr<ID3D11Buffer> cpVB;
	hr = GraphicDevice::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, &initialData, cpVB.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateBuffer()", hr);

	// Create an index buffer
	bufferDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	// bufferDesc.CPUAccessFlags = 0;
	// bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	initialData.pSysMem = indices.data();
	// initialData.SysMemPitch = 0;			// unused
	// initialData.SysMemSlicePitch = 0;	// unused

	ComPtr<ID3D11Buffer> cpIB;
	hr = GraphicDevice::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, &initialData, cpIB.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateBuffer()", hr);

	mesh->m_cpVB = std::move(cpVB);
	mesh->m_cpIB = std::move(cpIB);

	// 3. Vertex Format Type 설정
	// mesh->m_vft = VertexFormatType::PositionNormalTangentTexCoordSkinned;



	// 메시 벡터로 이동
	tmd.m_skinnedMeshes.push_back(std::move(mesh));
}

ModelData ResourceLoader::LoadModel(PCWSTR path)
{
	TempModelData tmd;
	ModelData md;

	Assimp::Importer aiImporter;
	CHAR utf8Path[MAX_PATH];
	Helper::ConvertUTF16ToUTF8(path, utf8Path, sizeof(utf8Path));
	const aiScene* pAiScene = aiImporter.ReadFile(utf8Path, 
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PopulateArmatureData |
		aiProcess_LimitBoneWeights |
		aiProcess_GenBoundingBoxes
	);

	if (!pAiScene)
	{
		Debug::ForceCrashWithMessageBox("Error", "Assimp::Importer::ReadFile error: %s", aiImporter.GetErrorString());
		return md;
	}

	aiNode* pAiRootNode = pAiScene->mRootNode;
	DFSAiNodeLoadModel(tmd, pAiScene, pAiRootNode);

	AiLoadAnimation(tmd, pAiScene);

	assert(tmd.m_armatureData.m_armatureNodeNames.size() == tmd.m_armatureData.m_armatures.size());

	md.m_staticMeshes = std::move(tmd.m_staticMeshes);
	md.m_armatures = std::move(tmd.m_armatureData.m_armatures);
	md.m_skinnedMeshes = std::move(tmd.m_skinnedMeshes);

	return md;
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
	descTexture.SampleDesc.Count = 1;	// 렌더 타겟이 아니므로 MSAA X
	descTexture.SampleDesc.Quality = 0;	// 렌더 타겟이 아니므로 MSAA X
	descTexture.Usage = D3D11_USAGE_DEFAULT;
	descTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE;		// 셰이더 리소스 목적으로 생성
	descTexture.MiscFlags = metadata.IsCubemap() ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

	// 텍스처 생성 및 초기화
	ComPtr<ID3D11Texture2D> cpTex2D;
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
	hr = GraphicDevice::GetInstance()->GetDevice()->CreateTexture2D(
		&descTexture, initialData.data(), cpTex2D.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateTexture2D()", hr);

	// 셰이더 리소스 뷰 생성
	ComPtr<ID3D11ShaderResourceView> cpSRV;
	hr = GraphicDevice::GetInstance()->GetDevice()->CreateShaderResourceView(
		cpTex2D.Get(), nullptr, cpSRV.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateShaderResourceView()", hr);

	return Texture2D(cpTex2D, cpSRV);
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
	ID3D11Device* pDevice = GraphicDevice::GetInstance()->GetDevice();
	

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

/*
std::vector<std::shared_ptr<StaticMesh>> ResourceLoader::LoadWavefrontOBJ(PCWSTR path)
{
	FILE* pMeshFile = nullptr;
	std::vector<std::shared_ptr<StaticMesh>> meshes;

	do
	{
		VertexPack vp;

		// UTF-8 -> UTF-16 자동 인코딩 변환하는 것으로 확인
		// HexEditor로 열었을 때와 이 함수로 읽어들였을 때 값이 다름. (변환이 됨)
		errno_t e;
		e = _wfopen_s(&pMeshFile, path, L"rt, ccs=UTF-8");
		if (e != 0)
			Debug::ForceCrashWithMessageBox(L"Error", L"Failed to open mesh file.\n%s", path);

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
					std::shared_ptr<StaticMesh> spMesh = std::make_shared<StaticMesh>(token);
					StaticMesh* pMesh = spMesh.get();
					meshes.push_back(std::move(spMesh));
					long ofpos;
					if (ResourceLoader::ParseWavefrontOBJObject(pMeshFile, &ofpos, vp, pMesh))
						fseek(pMeshFile, ofpos, SEEK_SET);
				}
			}
		}
	} while (false);

	if (pMeshFile)
		fclose(pMeshFile);

	return meshes;
}
*/

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

/*
bool ResourceLoader::ParseWavefrontOBJObject(FILE* pOBJFile, long* pofpos, VertexPack& vp, StaticMesh* pMesh)
{
	HRESULT hr;
	// object 정보를 읽는다.
	// f가 나오면 ParseWavefrontOBJFaces 함수로 면 정보들을 읽는다.
	WCHAR line[MAX_LINE_LENGTH];
	WCHAR* nextToken = nullptr;
	WCHAR* wcstoxEnd;
	PCWSTR token;

	bool ret_o = false;
	VertexFormatType vft = VertexFormatType::UNKNOWN;
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
			ret_o = true;		// 또 다른 StaticMesh 스타트 심볼 등장
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
			if (vft == VertexFormatType::UNKNOWN)
			{
				PCWSTR firstIndexSet = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				PCWSTR firstSlash = wcschr(firstIndexSet, L'/');
				PCWSTR lastSlash = wcsrchr(firstIndexSet, L'/');
				if (!firstSlash)							// v
					vft = VertexFormatType::Position;
				else if (firstSlash == lastSlash)			// v/vt
					vft = VertexFormatType::PositionTexCoord;
				else if (firstSlash + 1 == lastSlash)		// v//vn
					vft = VertexFormatType::PositionNormal;
				else										// v/vt/vn
					vft = VertexFormatType::PositionNormalTexCoord;

				// pMesh->m_vft = vft;
			}

			fseek(pOBJFile, fpos, SEEK_SET);
			long nffpos;
			if (ResourceLoader::ParseWavefrontOBJFaces(pOBJFile, &nffpos, vft, vp, imp, pMesh, tempVB, tempIB))
				fseek(pOBJFile, nffpos, SEEK_SET);
		}

		fpos = ftell(pOBJFile);	// save file stream pointer
	}

	// assert(pMesh->m_vft != VertexFormatType::UNKNOWN);

	
	{
		// AABB 정보 생성
		ptrdiff_t stride;

		// Alignment: 4바이트
		switch (pMesh->m_vft)
		{
		case VertexFormatType::Position:
			stride = sizeof(VFPosition);
			break;
		case VertexFormatType::PositionNormal:
			stride = sizeof(VFPositionNormal);
			break;
		case VertexFormatType::PositionTexCoord:
			stride = sizeof(VFPositionTexCoord);
			break;
		case VertexFormatType::PositionNormalTexCoord:
			stride = sizeof(VFPositionNormalTexCoord);
			break;
		default:
			stride = 0;
			break;
		}
		assert(stride != 0);

		XMVECTOR min = XMVectorReplicate((std::numeric_limits<FLOAT>::max)());
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
		hr = GraphicDevice::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, &initialData, cpVB.GetAddressOf());
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
		hr = GraphicDevice::GetInstance()->GetDevice()->CreateBuffer(&bufferDesc, &initialData, cpIB.GetAddressOf());
		if (FAILED(hr))
			Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateBuffer()", hr);

		pMesh->m_cpVB = std::move(cpVB);
		pMesh->m_cpIB = std::move(cpIB);
	}

	return ret_o;
}

bool ResourceLoader::ParseWavefrontOBJFaces(FILE* pOBJFile, long* pnffpos, VertexFormatType vft, const VertexPack& vp,
	IndexMapPack& imp, StaticMesh* pMesh, RawVector& tempVB, std::vector<uint32_t>& tempIB)
{
	// f만 읽다가 아닌 경우 리턴

	WCHAR line[MAX_LINE_LENGTH];
	PWSTR nextToken = nullptr;
	PCWSTR token;
	uint32_t indexCount = 0;
	bool ret_nf = false;		// not f

	// Create a subset
	pMesh->m_subsets.push_back(MeshSubset());
	const size_t currentSubsetIndex = pMesh->m_subsets.size() - 1;
	MeshSubset& currentSubset = pMesh->m_subsets[currentSubsetIndex];
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
				case VertexFormatType::Position:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					{
						const IndexSet::Position item(vIndex);
						auto iter = imp.vIndexMap.find(item);
						if (iter == imp.vIndexMap.cend())
						{
							const VFPosition vData(vp.v[vIndex]);
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
				case VertexFormatType::PositionNormal:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vnIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionNormal item(vIndex, vnIndex);
						auto iter = imp.vvnIndexMap.find(item);
						if (iter == imp.vvnIndexMap.cend())
						{
							const VFPositionNormal vData(vp.v[vIndex], vp.vn[vnIndex]);
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
				case VertexFormatType::PositionTexCoord:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vtIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionTexCoord item(vIndex, vtIndex);
						auto iter = imp.vvtIndexMap.find(item);
						if (iter == imp.vvtIndexMap.cend())
						{
							const VFPositionTexCoord vData(vp.v[vIndex], vp.vt[vtIndex]);
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
				case VertexFormatType::PositionNormalTexCoord:
					vIndex = wcstoul(index[0], &toulEnd, 10) - 1;
					vtIndex = wcstoul(index[1], &toulEnd, 10) - 1;
					vnIndex = wcstoul(index[2], &toulEnd, 10) - 1;
					{
						const IndexSet::PositionNormalTexCoord item(vIndex, vnIndex, vtIndex);
						auto iter = imp.vvnvtIndexMap.find(item);
						if (iter == imp.vvnvtIndexMap.cend())
						{
							const VFPositionNormalTexCoord vData(vp.v[vIndex], vp.vn[vnIndex], vp.vt[vtIndex]);
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
*/

/*
bool Resource::ReadFaces_deprecated(FILE* pOBJFile, long* pnffpos, const VertexFormatType vft, const VertexPack& vp,
	IndexMapPack& imp, StaticMesh& mesh, size_t meshIndex, RawVector& tempVB, std::vector<uint32_t>& tempIB, std::vector<DeferredMtlLinkingData>& dml,
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
				case VertexFormatType::Position:
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
				case VertexFormatType::PositionNormal:
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
				case VertexFormatType::PositionTexCoord:
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
				case VertexFormatType::PositionNormalTexCoord:
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

bool Resource::ReadObject_deprecated(FILE* pOBJFile, long* pofpos, VertexPack& vp, StaticMesh& mesh, const size_t meshIndex,
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
	VertexFormatType vft = VertexFormatType::UNKNOWN;
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
			if (vft == VertexFormatType::UNKNOWN)
			{
				const wchar_t* firstIndexSet = wcstok_s(nullptr, OBJ_MTL_DELIM, &nextToken);
				const wchar_t* firstSlash = wcschr(firstIndexSet, L'/');
				const wchar_t* lastSlash = wcsrchr(firstIndexSet, L'/');
				if (!firstSlash)							// v
					vft = VertexFormatType::Position;
				else if (firstSlash == lastSlash)			// v/vt
					vft = VertexFormatType::PositionTexCoord;
				else if (firstSlash + 1 == lastSlash)		// v//vn
					vft = VertexFormatType::PositionNormal;
				else										// v/vt/vn
					vft = VertexFormatType::PositionNormalTexCoord;

				mesh.m_vft = vft;
			}

			fseek(objFile, fpos, SEEK_SET);
			long nffpos;
			if (Resource::ReadFaces_deprecated(pOBJFile, &nffpos, vft, vp, imp, mesh, meshIndex, tempVB, tempIB, dml, sa))
				fseek(objFile, nffpos, SEEK_SET);
		}

		fpos = ftell(pOBJFile);	// save file stream pointer
	}

	assert(mesh.m_vft != VertexFormatType::UNKNOWN);

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

*/


/*
std::vector<std::shared_ptr<StaticMesh>> Resource::LoadWavefrontOBJ(const wchar_t* path, bool importTexture)
{
	std::vector<std::shared_ptr<StaticMesh>> meshes;
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
				meshes.emplace_back(std::make_shared<StaticMesh>(token));
				const size_t meshIndex = meshes.size() - 1;
				StaticMesh& mesh = *meshes[meshIndex].get();
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

		hr = GraphicDevice::GetInstance()->GetDevice()->CreateTexture2D(
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
	hr = GraphicDevice::GetInstance()->GetDevice()->CreateShaderResourceView(
		cpTex2D.Get(), &srvDesc, cpSRV.GetAddressOf()
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"ID3D11Device::CreateShaderResourceView()", hr);

	return Texture2D(cpTex2D, cpSRV);
}
*/
