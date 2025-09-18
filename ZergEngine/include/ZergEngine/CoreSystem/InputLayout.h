#pragma once

#include <ZergEngine\CoreSystem\EngineConstants.h>
#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class InputLayoutHelper
	{
	public:
		static UINT GetStructureByteStride(VertexFormatType vft);
	};

	class InputLayout
	{
	public:
		InputLayout();

		void Init(ID3D11Device* pDevice, const D3D11_INPUT_ELEMENT_DESC* pDesc, UINT elementCount,
			const byte* pShaderByteCode, size_t shaderByteCodeSize);
		void Release();

		ID3D11InputLayout* GetComInterface() const { return m_cpInputLayout.Get(); }
	private:
		ComPtr<ID3D11InputLayout> m_cpInputLayout;
	};

	struct VFPosition
	{
	public:
		VFPosition() = default;
		VFPosition(const VFPosition&) = default;
		VFPosition& operator=(const VFPosition&) = default;
		VFPosition(const XMFLOAT3& position) noexcept
			: m_position(position)
		{
		}

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPosition::INPUT_ELEMENT_COUNT; }
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
		VFPositionColor() = default;
		VFPositionColor(const VFPositionColor&) = default;
		VFPositionColor& operator=(const VFPositionColor&) = default;
		VFPositionColor(const XMFLOAT3& position, const XMFLOAT4& color) noexcept
			: m_position(position)
			, m_color(color)
		{
		}

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionColor::INPUT_ELEMENT_COUNT; }
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
		VFPositionNormal() = default;
		VFPositionNormal(const VFPositionNormal&) = default;
		VFPositionNormal& operator=(const VFPositionNormal&) = default;
		VFPositionNormal(const XMFLOAT3& position, const XMFLOAT3& normal) noexcept
			: m_position(position)
			, m_normal(normal)
		{
		}

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionNormal::INPUT_ELEMENT_COUNT; }
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
		VFPositionTexCoord() = default;
		VFPositionTexCoord(const VFPositionTexCoord&) = default;
		VFPositionTexCoord& operator=(const VFPositionTexCoord&) = default;
		VFPositionTexCoord(const XMFLOAT3& position, const XMFLOAT2& texCoord) noexcept
			: m_position(position)
			, m_texCoord(texCoord)
		{
		}

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionTexCoord::INPUT_ELEMENT_COUNT; }
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
		VFPositionNormalTexCoord() = default;
		VFPositionNormalTexCoord(const VFPositionNormalTexCoord&) = default;
		VFPositionNormalTexCoord& operator=(const VFPositionNormalTexCoord&) = default;
		VFPositionNormalTexCoord(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& texCoord) noexcept
			: m_position(position)
			, m_normal(normal)
			, m_texCoord(texCoord)
		{
		}

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionNormalTexCoord::INPUT_ELEMENT_COUNT; }
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 3;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;
		XMFLOAT3 m_normal;
		XMFLOAT2 m_texCoord;
	};

	class VFPositionNormalTangentTexCoord
	{
	public:
		VFPositionNormalTangentTexCoord() = default;
		VFPositionNormalTangentTexCoord(const VFPositionNormalTangentTexCoord&) = default;
		VFPositionNormalTangentTexCoord& operator=(const VFPositionNormalTangentTexCoord&) = default;
		VFPositionNormalTangentTexCoord(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT3& tangent, const XMFLOAT2& texCoord) noexcept
			: m_position(position)
			, m_normal(normal)
			, m_tangent(tangent)
			, m_texCoord(texCoord)
		{
		}

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionNormalTangentTexCoord::INPUT_ELEMENT_COUNT; }
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 4;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;
		XMFLOAT3 m_normal;
		XMFLOAT3 m_tangent;
		XMFLOAT2 m_texCoord;
	};

	struct VFTerrainPatchControlPoint
	{
	public:
		VFTerrainPatchControlPoint() = default;
		VFTerrainPatchControlPoint(const VFTerrainPatchControlPoint&) = default;
		VFTerrainPatchControlPoint& operator=(const VFTerrainPatchControlPoint&) = default;
		VFTerrainPatchControlPoint(const XMFLOAT3& position, const XMFLOAT2& texCoord, const XMFLOAT2& boundsY) noexcept
			: m_position(position)
			, m_texCoord(texCoord)
			, m_boundsY(boundsY)
		{
		}

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFTerrainPatchControlPoint::INPUT_ELEMENT_COUNT; }
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 3;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;	// POSITION
		XMFLOAT2 m_texCoord;	// TEXCOORD0
		XMFLOAT2 m_boundsY;		// TEXCOORD1	(x: ���� ��ġ�� �ּ� ���̰�, y: ���� ��ġ�� �ִ� ���̰�)
	};

	class VFButton
	{
	public:
		VFButton() = default;
		VFButton(const VFButton&) = default;
		VFButton& operator=(const VFButton&) = default;
		VFButton(const XMFLOAT2& position, const XMFLOAT2& offset, const XMFLOAT2& shade) noexcept
			: m_position(position)
			, m_offset(offset)
			, m_shade(shade)
		{
		}

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFButton::INPUT_ELEMENT_COUNT; }
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 3;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT2 m_position;	// POSITION
		XMFLOAT2 m_offset;		// ��ư ���� ��ȯ ������
		XMFLOAT2 m_shade;		// ��ư ���� ����ġ [0] ������ �ʾ��� ��, [1] ������ ��
	};
}
