#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>
#include <ZergEngine\Common\EngineConstants.h>
#include <ZergEngine\CoreSystem\DataStructure\Frustum.h>

#define hlslstruct struct alignas(16)
#define HLSLPad float

namespace ze
{
	class IConstantBuffer abstract
	{
	public:
		IConstantBuffer() noexcept
			: m_pConstantBuffer(nullptr)
		{
		}
		virtual ~IConstantBuffer();

		void Release();
		ID3D11Buffer* GetComInterface() const { return m_pConstantBuffer; }
	protected:
		void InitImpl(ID3D11Device* pDevice, size_t bufferSize) noexcept;
		void UpdateImpl(ID3D11DeviceContext* pDeviceContext, const void* pData, size_t size) noexcept;
	protected:
		ID3D11Buffer* m_pConstantBuffer;
	};

	template<typename T>
	class ConstantBuffer : public IConstantBuffer
	{
	public:
		ConstantBuffer() = default;
		virtual ~ConstantBuffer() = default;

		void Init(ID3D11Device* pDevice) { InitImpl(pDevice, sizeof(T)); }
		void Update(ID3D11DeviceContext* pDeviceContext, const T* pData) { ConstantBuffer::UpdateImpl(pDeviceContext, pData, sizeof(T)); }
	};

	// ��������������������������������������������������������������������������������������������������
	// ��                CONSTANT BUFFER                ��
	// ��������������������������������������������������������������������������������������������������
	enum class MATERIAL_FLAG : uint32_t
	{
		NONE				= 0,
		USE_MATERIAL		= 0x80000000,
		USE_LIGHT_MAP		= 1 << 0,
		USE_DIFFUSE_MAP		= 1 << 1,
		USE_NORMAL_MAP		= 1 << 2,
		USE_SPECULAR_MAP	= 1 << 3,
	};

	enum class TERRAIN_LAYER_FLAG : uint32_t
	{
		NONE				= 0,
		USE_DIFFUSE_LAYER	= 0x00000001,
		USE_NORMAL_LAYER	= 0x00000002
	};

	hlslstruct MaterialData
	{
		MaterialData() noexcept
			: mtlFlag(static_cast<uint32_t>(MATERIAL_FLAG::NONE))
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4A(&ambient, XMVectorZero());
			XMStoreFloat4A(&diffuse, XMVectorZero());
			XMStoreFloat4A(&specular, XMVectorZero());
			XMStoreFloat4A(&reflect, XMVectorZero());
#endif
		}
	public:
		uint32_t mtlFlag;
		HLSLPad pad0;
		HLSLPad pad1;
		HLSLPad pad2;

		// Material
		XMFLOAT4A ambient;
		XMFLOAT4A diffuse;
		XMFLOAT4A specular; // r/g/b/p
		XMFLOAT4A reflect;
	};

	hlslstruct DirectionalLightData
	{
		DirectionalLightData() noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4A(&ambient, XMVectorZero());
			XMStoreFloat4A(&diffuse, XMVectorZero());
			XMStoreFloat4A(&specular, XMVectorZero());

			XMStoreFloat3(&directionW, XMVector3Normalize(g_XMOne3));
#endif
		}
		XMFLOAT4A ambient;
		XMFLOAT4A diffuse;
		XMFLOAT4A specular;

		XMFLOAT3 directionW;	// Need to be normalized!
		HLSLPad pad;
	};

	hlslstruct PointLightData
	{
		PointLightData() noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4A(&ambient, XMVectorZero());
			XMStoreFloat4A(&diffuse, XMVectorZero());
			XMStoreFloat4A(&specular, XMVectorZero());

			XMStoreFloat3(&positionW, XMVectorZero());
			range = 10.0f;

			XMStoreFloat3(&att, g_XMIdentityR2);	// 0, 0, 1, 0
#endif
		}
		XMFLOAT4A ambient;
		XMFLOAT4A diffuse;
		XMFLOAT4A specular;

		XMFLOAT3 positionW;
		FLOAT range;

		XMFLOAT3 att;     // a0/a1/a2     a0 + a1d + a2d^2
		HLSLPad pad;
	};

	hlslstruct SpotLightData
	{
		SpotLightData() noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			XMStoreFloat4A(&ambient, XMVectorZero());
			XMStoreFloat4A(&diffuse, XMVectorZero());
			XMStoreFloat4A(&specular, XMVectorZero());

			XMStoreFloat3(&positionW, XMVectorZero());
			range = 10.0f;

			XMStoreFloat3(&directionW, XMVector3Normalize(g_XMOne));
			spotExp = 10.0f;

			XMStoreFloat3(&att, g_XMIdentityR2);	// 0, 0, 1, 0
#endif
		}
		XMFLOAT4A ambient;
		XMFLOAT4A diffuse;
		XMFLOAT4A specular;

		XMFLOAT3 positionW;
		FLOAT range;

		XMFLOAT3 directionW;	// Need to be normalized!
		FLOAT spotExp;

		XMFLOAT3 att;     // a0/a1/a2     a0 + a1d + a2d^2
		HLSLPad pad;
	};

	hlslstruct CbPerFrame
	{
		uint32_t dlCount;
		uint32_t plCount;
		uint32_t slCount;
		HLSLPad pad;

		DirectionalLightData dl[MAX_GLOBAL_LIGHT_COUNT];
		PointLightData pl[MAX_GLOBAL_LIGHT_COUNT];
		SpotLightData sl[MAX_GLOBAL_LIGHT_COUNT];
	};

	hlslstruct CbPerCamera
	{
		XMFLOAT3 cameraPosW;
		HLSLPad pad0;

		FLOAT tessMinDist;
		FLOAT tessMaxDist;
		FLOAT minTessExponent;
		FLOAT maxTessExponent;

		Frustum frustumW;

		XMFLOAT4X4A vp;		// View * Proj
	};

	hlslstruct CbPerMesh
	{
		XMFLOAT4X4A w;			// World
		XMFLOAT4X4A wInvTr;		// Inversed world transform matrix (��յ� �����ϸ� �� �ùٸ� �븻 ���� ��ȯ�� ���� �ʿ�)
	};

	hlslstruct CbPerTerrain
	{
		FLOAT maxHeight;
		FLOAT tilingScale;
		uint32_t layerArraySize;
		uint32_t layerFlag;
	};

	hlslstruct CbPerDrawQuad
	{
		FLOAT width;
		FLOAT height;
		FLOAT topLeftX;
		FLOAT topLeftY;
	};

	hlslstruct CbPerSubset
	{
		MaterialData mtl;
	};

	hlslstruct CbPerUIRender
	{
		XMFLOAT2 toNDCSpaceRatio;	// 
	};

	hlslstruct CbPerPCQuad
	{
		XMFLOAT4A color;
		XMFLOAT2 size;
		XMFLOAT2 position;
	};

	hlslstruct CbPerPTQuad
	{
		XMFLOAT2 size;
		XMFLOAT2 position;
	};

	hlslstruct CbPerButton
	{
		XMFLOAT4A color;
		XMFLOAT2 size;
		XMFLOAT2 position;
		uint32_t shadeIndex;	// [0] not pressed, [1] pressed
	};
}
