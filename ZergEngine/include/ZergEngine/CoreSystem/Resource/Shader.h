#pragma once

#include <ZergEngine\Common\EngineHelper.h>

#define HLSL_PAD FLOAT

namespace ze
{
	enum class TEXTURE_SLOT_NUM : uint32_t
	{
		RENDER_RESULT_TEX = 0,
		SKYBOX_CUBE = 0,
		HEIGHT_MAP = 0,
		LIGHT_MAP = 0,
		BASE_MAP = 1,
		NORMAL_MAP = 2,
		SPECULAR_MAP = 3
	};

	class HLSLStruct
	{
	public:
		hlsl_struct Material
		{
			inline void Initialize() { mtlFlag = 0; }
			inline void UseMaterial()		{ mtlFlag |= 0x00000001; }
			inline void UseLightMap()		{ mtlFlag |= 0x00000002; }
			inline void UseBaseMap()		{ mtlFlag |= 0x00000004; }
			inline void UseNormalMap()		{ mtlFlag |= 0x00000008; }
			inline void UseSpecularMap()	{ mtlFlag |= 0x00000010; }
		private:
			uint32_t mtlFlag;
			HLSL_PAD pad[3];
		public:
			// Material
			XMFLOAT4A ambient;
			XMFLOAT4A diffuse;
			XMFLOAT4A specular; // r/g/b/p
			XMFLOAT4A reflect;
		};

		hlsl_struct DirectionalLight
		{
			XMFLOAT4A ambient;
			XMFLOAT4A diffuse;
			XMFLOAT4A specular;

			XMFLOAT3 directionW;		// need to be normalized!
			HLSL_PAD pad;
		};

		hlsl_struct PointLight
		{
			XMFLOAT4A ambient;
			XMFLOAT4A diffuse;
			XMFLOAT4A specular;

			XMFLOAT3 positionW;
			FLOAT range;

			XMFLOAT3 att;     // a0/a1/a2     a0 + a1d + a2d^2
			HLSL_PAD pad;
		};

		hlsl_struct SpotLight
		{
			XMFLOAT4A ambient;
			XMFLOAT4A diffuse;
			XMFLOAT4A specular;

			XMFLOAT3 positionW;
			FLOAT range;

			XMFLOAT3 directionW;
			FLOAT spotExp;

			XMFLOAT3 att;     // a0/a1/a2     a0 + a1d + a2d^2
			HLSL_PAD pad;
		};
	};

	class ConstantBuffer
	{
		friend class Graphics;
	public:
		cbuffer PerFrame
		{
			DECLARE_SLOT_NUMBER(0);

			uint32_t cbpf_dlCount;
			uint32_t cbpf_plCount;
			uint32_t cbpf_slCount;

			HLSLStruct::DirectionalLight cbpf_dl[4];
			HLSLStruct::PointLight cbpf_pl[4];
			HLSLStruct::SpotLight cbpf_sl[4];
		};

		cbuffer PerCamera
		{
			DECLARE_SLOT_NUMBER(1);

			XMFLOAT3 cbpc_cameraPosW;
			HLSL_PAD cbpc_pad;

			FLOAT cbpc_tessMinDist;
			FLOAT cbpc_tessMaxDist;
			FLOAT cbpc_minTessExponent;
			FLOAT cbpc_maxTessExponent;

			XMFLOAT4A cbpc_frustumPlane[6];
		};

		cbuffer PerMesh
		{
			DECLARE_SLOT_NUMBER(2);

			union
			{
				XMFLOAT4X4A cbpm_w;		// World
				XMFLOAT4X4A cbpsky_w;		// World
			};
			union
			{
				XMFLOAT4X4A cbpm_wInvTr;	// Inversed world transform matrix (��յ� �����ϸ� �� �ùٸ� �븻 ���� ��ȯ�� ���� �ʿ�)
				XMFLOAT4X4A cbpsky_wInvTr;	// Inversed world transform matrix (��յ� �����ϸ� �� �ùٸ� �븻 ���� ��ȯ�� ���� �ʿ�)
			};
			union
			{
				XMFLOAT4X4A cbpm_wvp;		// World * View * Proj
				XMFLOAT4X4A cbpsky_wvp;		// World * View * Proj
			};
		};

		cbuffer PerTerrain
		{
			DECLARE_SLOT_NUMBER(2);

			XMFLOAT4X4A cbpt_w;		// World
			XMFLOAT4X4A cbpt_wInvTr;	// Inversed world transform matrix (��յ� �����ϸ� �� �ùٸ� �븻 ���� ��ȯ�� ���� �ʿ�)
			XMFLOAT4X4A cbpt_wvp;		// World * View * Proj

			uint32_t cbpt_terrainMtlCount;

			FLOAT cbpt_terrainTextureTiling;	// ������ ���̴����� Tiled �ؽ��� ��ǥ ��� �� �ʿ�
			FLOAT cbpt_terrainCellSpacing;	// �ȼ� ���̴����� �븻 ��� �� �ʿ�
			FLOAT cbpt_terrainTexelSpacingU;	// �ȼ� ���̴����� �븻 ��� �� �ʿ�
			FLOAT cbpt_terrainTexelSpacingV;	// �ȼ� ���̴����� �븻 ��� �� �ʿ�

			HLSLStruct::Material cbpt_terrainMtl[5];
		};

		cbuffer PerSubset
		{
			DECLARE_SLOT_NUMBER(3);

			HLSLStruct::Material cbps_subsetMtl;
		};

		cbuffer PerCameraMerge
		{
			DECLARE_SLOT_NUMBER(4);

			FLOAT cbpcm_width;
			FLOAT cbpcm_height;
			FLOAT cbpcm_topLeftX;
			FLOAT cbpcm_topLeftY;
		};

		using PerMSCameraMerge = PerCameraMerge;
		using PerSkybox = PerMesh;
	};

	class ShaderResourceSlot
	{
	public:
		class RenderResultSlot
		{
			DECLARE_SLOT_NUMBER(0);
		};

		class SkyboxCubemapSlot
		{
			DECLARE_SLOT_NUMBER(0);
		};
		
		class HeightmapSlot
		{
			DECLARE_SLOT_NUMBER(0);
		};

		class LightmapSlot
		{
			DECLARE_SLOT_NUMBER(0);
		};

		class BasemapSlot
		{
			DECLARE_SLOT_NUMBER(1);
		};

		class NormalmapSlot
		{
			DECLARE_SLOT_NUMBER(2);
		};

		class SpecularmapSlot
		{
			DECLARE_SLOT_NUMBER(3);
		};
	};

	// ���̴� Ŭ������ ����
	// �����Լ� ���̺� ������ ũ�� ������ ���� ���̴� �������̽��� ���� X
	class VertexShader
	{
	public:
		VertexShader()
			: m_cpShader(nullptr)
		{
		}
		~VertexShader() = default;

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		inline void Release() { m_cpShader.Reset(); }

		inline ID3D11VertexShader* GetComInterface() { return m_cpShader.Get(); }
	private:
		ComPtr<ID3D11VertexShader> m_cpShader;
	};

	class HullShader
	{
	public:
		HullShader()
			: m_cpShader(nullptr)
		{
		}
		~HullShader() = default;

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		inline void Release() { m_cpShader.Reset(); }

		inline ID3D11HullShader* GetComInterface() { return m_cpShader.Get(); }
	private:
		ComPtr<ID3D11HullShader> m_cpShader;
	};

	class DomainShader
	{
	public:
		DomainShader()
			: m_cpShader(nullptr)
		{
		}
		~DomainShader() = default;

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		inline void Release() { m_cpShader.Reset(); }

		inline ID3D11DomainShader* GetComInterface() { return m_cpShader.Get(); }
	private:
		ComPtr<ID3D11DomainShader> m_cpShader;
	};

	class PixelShader
	{
	public:
		PixelShader()
			: m_cpShader(nullptr)
		{
		}
		~PixelShader() = default;

		void Init(ID3D11Device* pDevice, const byte* pShaderByteCode, size_t shaderByteCodeSize);
		inline void Release() { m_cpShader.Reset(); }

		inline ID3D11PixelShader* GetComInterface() { return m_cpShader.Get(); }
	private:
		ComPtr<ID3D11PixelShader> m_cpShader;
	};
}
