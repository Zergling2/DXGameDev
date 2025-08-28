#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

#define INPUT_LAYOUT_STATIC_ASSERT_MESSAGE "Vertex struct/layout mismatch"

UINT InputLayoutHelper::GetStructureByteStride(VERTEX_FORMAT_TYPE vft)
{
	switch (vft)
	{
	case VERTEX_FORMAT_TYPE::POSITION:
		return static_cast<UINT>(sizeof(VFPosition));
	case VERTEX_FORMAT_TYPE::POSITION_COLOR:
		return static_cast<UINT>(sizeof(VFPositionColor));
	case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
		return static_cast<UINT>(sizeof(VFPositionNormal));
	case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
		return static_cast<UINT>(sizeof(VFPositionTexCoord));
	case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
		return static_cast<UINT>(sizeof(VFPositionNormalTexCoord));
	case VERTEX_FORMAT_TYPE::TERRAIN_PATCH_CTRL_PT:
		return static_cast<UINT>(sizeof(VFTerrainPatchControlPoint));
	case VERTEX_FORMAT_TYPE::BUTTON:
		return static_cast<UINT>(sizeof(VFButton));
	default:
		Debug::ForceCrashWithMessageBox(
			L"Fatal Error",
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
	HRESULT hr;

	hr = pDevice->CreateInputLayout(pDesc, elementCount, pShaderByteCode, shaderByteCodeSize, m_cpInputLayout.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTMessageBox(L"InputLayout::Init()", hr);
}

//--------------------------------------------------------------------------------------
// Vertex struct holding position .
const D3D11_INPUT_ELEMENT_DESC VFPosition::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VFPosition) == 12, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);


//--------------------------------------------------------------------------------------
// Vertex struct holding position and color information.
const D3D11_INPUT_ELEMENT_DESC VFPositionColor::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VFPositionColor) == 28, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);


//--------------------------------------------------------------------------------------
// Vertex struct holding position and normal vector.
const D3D11_INPUT_ELEMENT_DESC VFPositionNormal::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VFPositionNormal) == 24, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);

//--------------------------------------------------------------------------------------
// Vertex struct holding position and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC VFPositionTexCoord::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VFPositionTexCoord) == 20, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);


//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC VFPositionNormalTexCoord::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VFPositionNormalTexCoord) == 32, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);

//--------------------------------------------------------------------------------------
// Vertex struct holding position, texture mapping information and bounding volume.
const D3D11_INPUT_ELEMENT_DESC VFTerrainPatchControlPoint::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VFTerrainPatchControlPoint) == 28, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);

//--------------------------------------------------------------------------------------
// Vertex struct holding position, texture mapping information and bounding volume.
const D3D11_INPUT_ELEMENT_DESC VFButton::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
static_assert(sizeof(VFButton) == 24, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);
