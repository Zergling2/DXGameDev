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

		ID3D11InputLayout* Get() const { return m_cpInputLayout.Get(); }
	private:
		ComPtr<ID3D11InputLayout> m_cpInputLayout;
	};

	struct VFPosition
	{
	public:
		VFPosition() = default;
		VFPosition(const VFPosition&) = default;
		VFPosition& operator=(const VFPosition&) = default;

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

	class VFPositionNormalTangentTexCoordSkinned
	{
	public:
		VFPositionNormalTangentTexCoordSkinned()
		{
			XMStoreFloat3(&m_position, XMVectorZero());
			XMStoreFloat3(&m_normal, XMVectorZero());
			XMStoreFloat3(&m_tangent, XMVectorZero());
			XMStoreFloat2(&m_texCoord, XMVectorZero());
			m_weights[0] = 0.0f;
			m_weights[1] = 0.0f;
			m_weights[2] = 0.0f;
			m_weights[3] = 0.0f;
			m_boneIndices[0] = 0;	// 항상 유효한 인덱스이긴 해야 함. (뼈를 참조하지 않더라도 weight가 0이면 영향을 없앨 수 있으므로)
			m_boneIndices[1] = 0;
			m_boneIndices[2] = 0;
			m_boneIndices[3] = 0;
		}
		VFPositionNormalTangentTexCoordSkinned(const VFPositionNormalTangentTexCoordSkinned&) = default;
		VFPositionNormalTangentTexCoordSkinned& operator=(const VFPositionNormalTangentTexCoordSkinned&) = default;

		bool AddSkinningData(BYTE boneIndex, float weight);

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFPositionNormalTangentTexCoordSkinned::INPUT_ELEMENT_COUNT; }
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 6;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;
		XMFLOAT3 m_normal;
		XMFLOAT3 m_tangent;
		XMFLOAT2 m_texCoord;
		FLOAT m_weights[4];
		BYTE m_boneIndices[4];
	};

	struct VFTerrainPatchControlPoint
	{
	public:
		VFTerrainPatchControlPoint() = default;
		VFTerrainPatchControlPoint(const VFTerrainPatchControlPoint&) = default;
		VFTerrainPatchControlPoint& operator=(const VFTerrainPatchControlPoint&) = default;

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFTerrainPatchControlPoint::INPUT_ELEMENT_COUNT; }
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 3;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT3 m_position;	// POSITION
		XMFLOAT2 m_texCoord;	// TEXCOORD
		XMFLOAT2 m_boundsY;		// YBOUNDS
	};

	class VFShaded2DQuad
	{
	public:
		VFShaded2DQuad() = default;
		VFShaded2DQuad(const VFShaded2DQuad&) = default;
		VFShaded2DQuad& operator=(const VFShaded2DQuad&) = default;

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFShaded2DQuad::INPUT_ELEMENT_COUNT; }
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 3;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT2 m_position;		// POSITION
		XMFLOAT2 m_offset;			// 스케일링 후 이동 오프셋 (음영 모서리의 두께를 스케일링과 분리)
		XMFLOAT2 m_colorWeights;	// 색상 가중치 [0] Convex 상태 음영 가중치 [1] Concave 상태 음영 가중치
	};

	class VFCheckbox
	{
	public:
		static constexpr uint32_t BOX_COLOR_INDEX = 0;
		static constexpr uint32_t CHECK_COLOR_INDEX = 1;
	public:
		VFCheckbox() = default;
		VFCheckbox(const VFCheckbox&) = default;
		VFCheckbox& operator=(const VFCheckbox&) = default;

		static const D3D11_INPUT_ELEMENT_DESC* GetInputElementDescriptor() { return s_ied; }
		static constexpr uint32_t GetInputElementCount() { return VFCheckbox::INPUT_ELEMENT_COUNT; }
	private:
		static constexpr size_t INPUT_ELEMENT_COUNT = 4;
		static const D3D11_INPUT_ELEMENT_DESC s_ied[INPUT_ELEMENT_COUNT];
	public:
		XMFLOAT2 m_position;
		XMFLOAT2 m_offset;
		FLOAT m_colorWeight;	// 배경 박스는 항상 오목 상태이므로 가중치 불변
		uint32_t m_colorIndex;	// 박스와 체크 심볼의 색상 분리 (Box color: [0], Check symbol color: [1])
	};
}
