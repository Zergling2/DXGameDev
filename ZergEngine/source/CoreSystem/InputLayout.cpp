#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

#define INPUT_LAYOUT_STATIC_ASSERT_MESSAGE "Vertex struct/layout mismatch"

UINT InputLayoutHelper::GetStructureByteStride(VertexFormatType vft)
{
	switch (vft)
	{
	case VertexFormatType::Position:
		return static_cast<UINT>(sizeof(VFPosition));
	case VertexFormatType::PositionColor:
		return static_cast<UINT>(sizeof(VFPositionColor));
	case VertexFormatType::PositionNormal:
		return static_cast<UINT>(sizeof(VFPositionNormal));
	case VertexFormatType::PositionTexCoord:
		return static_cast<UINT>(sizeof(VFPositionTexCoord));
	case VertexFormatType::PositionNormalTexCoord:
		return static_cast<UINT>(sizeof(VFPositionNormalTexCoord));
	case VertexFormatType::PositionNormalTangentTexCoord:
		return static_cast<UINT>(sizeof(VFPositionNormalTangentTexCoord));
	case VertexFormatType::PositionNormalTangentTexCoordSkinned:
		return static_cast<UINT>(sizeof(VFPositionNormalTangentTexCoordSkinned));
	case VertexFormatType::TerrainPatchCtrlPt:
		return static_cast<UINT>(sizeof(VFTerrainPatchControlPoint));
	case VertexFormatType::Shaded2DQuad:
		return static_cast<UINT>(sizeof(VFShaded2DQuad));
	default:
		Debug::ForceCrashWithMessageBox(
			L"Error",
			L"InputLayoutHelper::GetStructureByteStride()\nUnknown vertex format type: %d\n", static_cast<int>(vft)
		);
		return 0;
	}
}

InputLayout::InputLayout()
	: m_cpInputLayout(nullptr)
{
}

void InputLayout::Init(ID3D11Device* pDevice, const D3D11_INPUT_ELEMENT_DESC* pDesc, UINT elementCount,
	const byte* pShaderByteCode, size_t shaderByteCodeSize)
{
	this->Release();

	HRESULT hr;

	hr = pDevice->CreateInputLayout(pDesc, elementCount, pShaderByteCode, shaderByteCodeSize, m_cpInputLayout.GetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"InputLayout::Init()", hr);
}

void InputLayout::Release()
{
	m_cpInputLayout.Reset();
}

//--------------------------------------------------------------------------------------
// Vertex struct holding position .
const D3D11_INPUT_ELEMENT_DESC VFPosition::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//--------------------------------------------------------------------------------------
// Vertex struct holding position and color information.
const D3D11_INPUT_ELEMENT_DESC VFPositionColor::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//--------------------------------------------------------------------------------------
// Vertex struct holding position and normal vector.
const D3D11_INPUT_ELEMENT_DESC VFPositionNormal::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//--------------------------------------------------------------------------------------
// Vertex struct holding position and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC VFPositionTexCoord::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC VFPositionNormalTexCoord::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, texture mapping, tangent information.
const D3D11_INPUT_ELEMENT_DESC VFPositionNormalTangentTexCoord::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector, texture mapping, tangent information, skinning data.
const D3D11_INPUT_ELEMENT_DESC VFPositionNormalTangentTexCoordSkinned::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}	// HLSL에서 uint4로 바인딩
};

//--------------------------------------------------------------------------------------
const D3D11_INPUT_ELEMENT_DESC VFTerrainPatchControlPoint::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "YBOUNDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//--------------------------------------------------------------------------------------
const D3D11_INPUT_ELEMENT_DESC VFShaded2DQuad::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "OFFSET", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLORWEIGHTS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

//--------------------------------------------------------------------------------------
const D3D11_INPUT_ELEMENT_DESC VFCheckbox::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "OFFSET", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLORWEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLORINDEX", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

bool VFPositionNormalTangentTexCoordSkinned::AddSkinningData(BYTE boneIndex, float weight)
{
	bool success = false;

	for (size_t i = 0; i < _countof(m_weights); ++i)
	{
		if (m_weights[i] == 0.0f)
		{
			m_boneIndices[i] = boneIndex;
			m_weights[i] = weight;
			success = true;
			break;
		}
	}

	return success;
}
