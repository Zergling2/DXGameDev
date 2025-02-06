#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class VertexFormat
	{
	public:
		struct Position
		{
		public:
			static constexpr uint32_t INPUT_LAYOUT_INDEX = static_cast<uint32_t>(VERTEX_FORMAT_TYPE::POSITION);
			Position() = default;
			Position(const Position&) = default;
			Position& operator=(const Position&) = default;
			Position(const XMFLOAT3& position) noexcept
				: m_position(position)
			{
			}
			Position(FXMVECTOR position) noexcept
			{
				XMStoreFloat3(&m_position, position);
			}
		public:
			XMFLOAT3 m_position;
		};

		// Vertex struct holding position and color information.
		struct PositionColor	// Used for only debug wireframe mode
		{
		public:
			static constexpr uint32_t INPUT_LAYOUT_INDEX = static_cast<uint32_t>(VERTEX_FORMAT_TYPE::POSITION_COLOR);
			PositionColor() = default;
			PositionColor(const PositionColor&) = default;
			PositionColor& operator=(const PositionColor&) = default;
			PositionColor(const XMFLOAT3& position, const XMFLOAT4& color) noexcept
				: m_position(position)
				, m_color(color)
			{
			}
			PositionColor(FXMVECTOR position, FXMVECTOR color) noexcept
			{
				XMStoreFloat3(&m_position, position);
				XMStoreFloat4(&m_color, color);
			}
		public:
			XMFLOAT3 m_position;
			XMFLOAT4 m_color;
		};

		// Vertex struct holding position and normal vector.
		struct PositionNormal
		{
		public:
			static constexpr uint32_t INPUT_LAYOUT_INDEX = static_cast<uint32_t>(VERTEX_FORMAT_TYPE::POSITION_NORMAL);
			PositionNormal() = default;
			PositionNormal(const PositionNormal&) = default;
			PositionNormal& operator=(const PositionNormal&) = default;
			PositionNormal(const XMFLOAT3& position, const XMFLOAT3& normal) noexcept
				: m_position(position)
				, m_normal(normal)
			{
			}
			PositionNormal(FXMVECTOR position, FXMVECTOR normal) noexcept
			{
				XMStoreFloat3(&m_position, position);
				XMStoreFloat3(&m_normal, normal);
			}
		public:
			XMFLOAT3 m_position;
			XMFLOAT3 m_normal;
		};

		struct PositionTexCoord
		{
		public:
			static constexpr uint32_t INPUT_LAYOUT_INDEX = static_cast<uint32_t>(VERTEX_FORMAT_TYPE::POSITION_TEXCOORD);
			PositionTexCoord() = default;
			PositionTexCoord(const PositionTexCoord&) = default;
			PositionTexCoord& operator=(const PositionTexCoord&) = default;
			PositionTexCoord(const XMFLOAT3& position, const XMFLOAT2& texCoord) noexcept
				: m_position(position)
				, m_texCoord(texCoord)
			{
			}
			PositionTexCoord(FXMVECTOR position, FXMVECTOR texCoord) noexcept
			{
				XMStoreFloat3(&m_position, position);
				XMStoreFloat2(&m_texCoord, texCoord);
			}
		public:
			XMFLOAT3 m_position;
			XMFLOAT2 m_texCoord;
		};

		struct PositionNormalTexCoord
		{
		public:
			static constexpr uint32_t INPUT_LAYOUT_INDEX = static_cast<uint32_t>(VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD);
			PositionNormalTexCoord() = default;
			PositionNormalTexCoord(const PositionNormalTexCoord&) = default;
			PositionNormalTexCoord& operator=(const PositionNormalTexCoord&) = default;
			PositionNormalTexCoord(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& texCoord) noexcept
				: m_position(position)
				, m_normal(normal)
				, m_texCoord(texCoord)
			{
			}
			PositionNormalTexCoord(FXMVECTOR position, FXMVECTOR normal, FXMVECTOR texCoord) noexcept
			{
				XMStoreFloat3(&m_position, position);
				XMStoreFloat3(&m_normal, normal);
				XMStoreFloat2(&m_texCoord, texCoord);
			}
		public:
			XMFLOAT3 m_position;
			XMFLOAT3 m_normal;
			XMFLOAT2 m_texCoord;
		};

		struct Terrain
		{
		public:
			static constexpr uint32_t INPUT_LAYOUT_INDEX = static_cast<uint32_t>(VERTEX_FORMAT_TYPE::TERRAIN);
			Terrain() = default;
			Terrain(const Terrain&) = default;
			Terrain& operator=(const Terrain&) = default;
			Terrain(const XMFLOAT3& position, const XMFLOAT2& texCoord, const XMFLOAT2& boundsY) noexcept
				: m_position(position)
				, m_texCoord(texCoord)
				, m_boundsY(boundsY)
			{
			}
			Terrain(FXMVECTOR position, FXMVECTOR texCoord, FXMVECTOR boundsY) noexcept
			{
				XMStoreFloat3(&m_position, position);
				XMStoreFloat2(&m_texCoord, texCoord);
				XMStoreFloat2(&m_boundsY, boundsY);
			}
		public:
			XMFLOAT3 m_position;	// POSITION
			XMFLOAT2 m_texCoord;	// TEXCOORD0
			XMFLOAT2 m_boundsY;		// TEXCOORD1	(Min Y, Max Y)
		};
	};

	class InputLayout
	{
		friend class Graphics;

		// Input layout for the
		// - VSStandardTransformP
		// - VSSkyboxTransform
		class Position
		{
			friend class Graphics;
		private:
			static constexpr size_t INPUT_ELEMENT_COUNT = 1;
			static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
		};

		// Input layout for the
		// - VSStandardTransformPC
		class PositionColor
		{
			friend class Graphics;
		private:
			static constexpr size_t INPUT_ELEMENT_COUNT = 2;
			static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
		};

		// Input layout for the
		// - VSStandardTransformPN
		class PositionNormal
		{
			friend class Graphics;
		private:
			static constexpr size_t INPUT_ELEMENT_COUNT = 2;
			static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
		};

		// Input layout for the
		// - VSStandardTransformPT
		class PositionTexCoord
		{
			friend class Graphics;
		private:
			static constexpr size_t INPUT_ELEMENT_COUNT = 2;
			static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
		};

		// Input layout for the
		// - VSStandardTransformPNT
		class PositionNormalTexCoord
		{
			friend class Graphics;
		private:
			static constexpr size_t INPUT_ELEMENT_COUNT = 3;
			static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
		};

		class Terrain
		{
			friend class Graphics;
		private:
			static constexpr size_t INPUT_ELEMENT_COUNT = 3;
			static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
		};
	public:
		static inline ID3D11InputLayout* GetInputLayout(VERTEX_FORMAT_TYPE vft) { return InputLayout::s_items[static_cast<size_t>(vft)].Get(); }
	private:
		static ComPtr<ID3D11InputLayout> s_items[static_cast<size_t>(VERTEX_FORMAT_TYPE::COUNT)];
	};

	class InputLayoutHelper
	{
	public:
		static UINT GetStructureByteStride(VERTEX_FORMAT_TYPE vft);
	};
}
