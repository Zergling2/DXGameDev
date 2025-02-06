#include <ZergEngine\System\Shader\InputLayout.h>
#include <ZergEngine\System\Debug.h>

using namespace zergengine;

#define INPUT_LAYOUT_STATIC_ASSERT_MESSAGE "Vertex struct/layout mismatch"

ComPtr<ID3D11InputLayout> InputLayout::s_items[];

//--------------------------------------------------------------------------------------
// Vertex struct holding position .
const D3D11_INPUT_ELEMENT_DESC InputLayout::Position::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VertexFormat::Position) == 12, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);


//--------------------------------------------------------------------------------------
// Vertex struct holding position and color information.
const D3D11_INPUT_ELEMENT_DESC InputLayout::PositionColor::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VertexFormat::PositionColor) == 28, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);


//--------------------------------------------------------------------------------------
// Vertex struct holding position and normal vector.
const D3D11_INPUT_ELEMENT_DESC InputLayout::PositionNormal::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VertexFormat::PositionNormal) == 24, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);

//--------------------------------------------------------------------------------------
// Vertex struct holding position and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC InputLayout::PositionTexCoord::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VertexFormat::PositionTexCoord) == 20, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);


//--------------------------------------------------------------------------------------
// Vertex struct holding position, normal vector and texture mapping information.
const D3D11_INPUT_ELEMENT_DESC InputLayout::PositionNormalTexCoord::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VertexFormat::PositionNormalTexCoord) == 32, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);

//--------------------------------------------------------------------------------------
// Vertex struct holding position, texture mapping information and bounding volume.
const D3D11_INPUT_ELEMENT_DESC InputLayout::Terrain::s_ied[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
static_assert(sizeof(VertexFormat::Terrain) == 28, INPUT_LAYOUT_STATIC_ASSERT_MESSAGE);

UINT InputLayoutHelper::GetStructureByteStride(VERTEX_FORMAT_TYPE vft)
{
	LogBuffer* pLogBuffer;

	switch (vft)
	{
	case VERTEX_FORMAT_TYPE::POSITION:
		return static_cast<UINT>(sizeof(VertexFormat::Position));
	case VERTEX_FORMAT_TYPE::POSITION_COLOR:
		return static_cast<UINT>(sizeof(VertexFormat::PositionColor));
	case VERTEX_FORMAT_TYPE::POSITION_NORMAL:
		return static_cast<UINT>(sizeof(VertexFormat::PositionNormal));
	case VERTEX_FORMAT_TYPE::POSITION_TEXCOORD:
		return static_cast<UINT>(sizeof(VertexFormat::PositionTexCoord));
	case VERTEX_FORMAT_TYPE::POSITION_NORMAL_TEXCOORD:
		return static_cast<UINT>(sizeof(VertexFormat::PositionNormalTexCoord));
	case VERTEX_FORMAT_TYPE::TERRAIN:
		return static_cast<UINT>(sizeof(VertexFormat::Terrain));
	default:
		pLogBuffer = AsyncFileLogger::GetLogBuffer();
		StringCbPrintfW(pLogBuffer->buffer, LogBuffer::BUFFER_SIZE, L"[InputLayoutHelper::GetStructureByteStride] (vlt: %u) Invalid vertex format!\n", static_cast<uint32_t>(vft));
		AsyncFileLogger::PushLog(pLogBuffer);
		return FATAL_ERROR;		// forcing crash
	}
}
