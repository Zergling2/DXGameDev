#pragma once

#include <ZergEngine\Common\EngineHelper.h>
#include <ZergEngine\Common\EngineConstants.h>

namespace ze
{
	class InputLayoutHelper
	{
	public:
		static UINT GetStructureByteStride(VERTEX_FORMAT_TYPE vft);
	};

	class InputLayout
	{
	public:
		InputLayout();

		void Init(ID3D11Device* pDevice, const D3D11_INPUT_ELEMENT_DESC* pDesc, UINT elementCount,
			const byte* pShaderByteCode, size_t shaderByteCodeSize);
		inline void Release() { m_cpInputLayout.Reset(); }

		inline ID3D11InputLayout* GetComInterface() const { return m_cpInputLayout.Get(); }
	private:
		ComPtr<ID3D11InputLayout> m_cpInputLayout;
	};

	struct VFPosition
	{
	public:
		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPosition::INPUT_ELEMENT_COUNT; }
		VFPosition() = default;
		VFPosition(const VFPosition&) = default;
		VFPosition& operator=(const VFPosition&) = default;
		VFPosition(const XMFLOAT3& position) noexcept
			: m_position(position)
		{
		}
		VFPosition(FXMVECTOR position) noexcept
		{
			XMStoreFloat3(&m_position, position);
		}
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 1;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;
	};

	// Vertex struct holding position and color information.
	struct VFPositionColor	// Used for only debug wireframe mode
	{
	public:
		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionColor::INPUT_ELEMENT_COUNT; }

		VFPositionColor() = default;
		VFPositionColor(const VFPositionColor&) = default;
		VFPositionColor& operator=(const VFPositionColor&) = default;
		VFPositionColor(const XMFLOAT3& position, const XMFLOAT4& color) noexcept
			: m_position(position)
			, m_color(color)
		{
		}
		VFPositionColor(FXMVECTOR position, FXMVECTOR color) noexcept
		{
			XMStoreFloat3(&m_position, position);
			XMStoreFloat4(&m_color, color);
		}
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 2;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;
		XMFLOAT4 m_color;

	};

	// Vertex struct holding position and normal vector.
	struct VFPositionNormal
	{
	public:
		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionNormal::INPUT_ELEMENT_COUNT; }

		VFPositionNormal() = default;
		VFPositionNormal(const VFPositionNormal&) = default;
		VFPositionNormal& operator=(const VFPositionNormal&) = default;
		VFPositionNormal(const XMFLOAT3& position, const XMFLOAT3& normal) noexcept
			: m_position(position)
			, m_normal(normal)
		{
		}
		VFPositionNormal(FXMVECTOR position, FXMVECTOR normal) noexcept
		{
			XMStoreFloat3(&m_position, position);
			XMStoreFloat3(&m_normal, normal);
		}
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 2;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;
		XMFLOAT3 m_normal;
	};

	struct VFPositionTexCoord
	{
	public:
		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionTexCoord::INPUT_ELEMENT_COUNT; }

		VFPositionTexCoord() = default;
		VFPositionTexCoord(const VFPositionTexCoord&) = default;
		VFPositionTexCoord& operator=(const VFPositionTexCoord&) = default;
		VFPositionTexCoord(const XMFLOAT3& position, const XMFLOAT2& texCoord) noexcept
			: m_position(position)
			, m_texCoord(texCoord)
		{
		}
		VFPositionTexCoord(FXMVECTOR position, FXMVECTOR texCoord) noexcept
		{
			XMStoreFloat3(&m_position, position);
			XMStoreFloat2(&m_texCoord, texCoord);
		}
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 2;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;
		XMFLOAT2 m_texCoord;
	};

	struct VFPositionNormalTexCoord
	{
	public:
		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionNormalTexCoord::INPUT_ELEMENT_COUNT; }

		VFPositionNormalTexCoord() = default;
		VFPositionNormalTexCoord(const VFPositionNormalTexCoord&) = default;
		VFPositionNormalTexCoord& operator=(const VFPositionNormalTexCoord&) = default;
		VFPositionNormalTexCoord(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& texCoord) noexcept
			: m_position(position)
			, m_normal(normal)
			, m_texCoord(texCoord)
		{
		}
		VFPositionNormalTexCoord(FXMVECTOR position, FXMVECTOR normal, FXMVECTOR texCoord) noexcept
		{
			XMStoreFloat3(&m_position, position);
			XMStoreFloat3(&m_normal, normal);
			XMStoreFloat2(&m_texCoord, texCoord);
		}
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 3;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;
		XMFLOAT3 m_normal;
		XMFLOAT2 m_texCoord;
	};

	struct VFTerrainPatchControlPoint
	{
	public:
		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFTerrainPatchControlPoint::INPUT_ELEMENT_COUNT; }

		VFTerrainPatchControlPoint() = default;
		VFTerrainPatchControlPoint(const VFTerrainPatchControlPoint&) = default;
		VFTerrainPatchControlPoint& operator=(const VFTerrainPatchControlPoint&) = default;
		VFTerrainPatchControlPoint(const XMFLOAT3& position, const XMFLOAT2& texCoord, const XMFLOAT2& boundsY) noexcept
			: m_position(position)
			, m_texCoord(texCoord)
			, m_boundsY(boundsY)
		{
		}
		VFTerrainPatchControlPoint(FXMVECTOR position, FXMVECTOR texCoord, FXMVECTOR boundsY) noexcept
		{
			XMStoreFloat3(&m_position, position);
			XMStoreFloat2(&m_texCoord, texCoord);
			XMStoreFloat2(&m_boundsY, boundsY);
		}
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 3;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;	// POSITION
		XMFLOAT2 m_texCoord;	// TEXCOORD0
		XMFLOAT2 m_boundsY;		// TEXCOORD1	(Min Y, Max Y)
	};
}
