#include <ZergEngine\CoreSystem\PhysicsDebugDrawer.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <ZergEngine\CoreSystem\Math.h>

using namespace ze;

constexpr size_t MAX_DEBUG_LINES_PER_FRAME = 1024;
constexpr UINT DEBUG_LINE_BUFFER_BYTE_WIDTH = sizeof(DebugLineVertexFormat) * MAX_DEBUG_LINES_PER_FRAME * 2;

PhysicsDebugDrawer::PhysicsDebugDrawer()
	: m_debugLineVertices()
	, m_debugMode(btIDebugDraw::DBG_NoDebug)
	, m_cpDebugLineVertices()
	, m_cpRSDebugLineDrawing()
{
	m_debugLineVertices.reserve(MAX_DEBUG_LINES_PER_FRAME * 2);
}

bool PhysicsDebugDrawer::Init(ID3D11Device* pDevice)
{
	SyncFileLogger& sfl = Runtime::GetInstance()->GetSyncFileLogger();

	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.ByteWidth = DEBUG_LINE_BUFFER_BYTE_WIDTH;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	// bufferDesc.StructureByteStride = static_cast<UINT>(sizeof(DebugLineVertexFormat));

	ComPtr<ID3D11Buffer> cpDebugLineVertices;
	hr = pDevice->CreateBuffer(&bufferDesc, nullptr, cpDebugLineVertices.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID3D11Device::CreateBuffer", hr);
		return false;
	}
	
	// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_rasterizer_desc
	// Line-rendering algorithm			MultisampleEnable		AntialiasedLineEnable
	// Aliased							FALSE					FALSE
	// Alpha antialiased				FALSE					TRUE
	// Quadrilateral					TRUE					FALSE
	// Quadrilateral					TRUE					TRUE

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;		// 디버그 라인은 후면컬링 필요없음
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = -100;
	rasterizerDesc.DepthBiasClamp = 1.0f;
	rasterizerDesc.SlopeScaledDepthBias = -1.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = TRUE;	// 상단 표 참고
	rasterizerDesc.AntialiasedLineEnable = FALSE;	// 상단 표 참고

	ComPtr<ID3D11RasterizerState> cpRSDebugLineDrawing;
	hr = pDevice->CreateRasterizerState(&rasterizerDesc, cpRSDebugLineDrawing.GetAddressOf());
	if (FAILED(hr))
	{
		sfl.WriteFormat(HRESULT_ERROR_LOG_FMT, L"ID3D11Device::CreateRasterizerState", hr);
		return false;
	}

	m_cpDebugLineVertices = std::move(cpDebugLineVertices);
	m_cpRSDebugLineDrawing = std::move(cpRSDebugLineDrawing);

	return true;
}

void PhysicsDebugDrawer::Release()
{
	m_cpDebugLineVertices.Reset();
	m_cpRSDebugLineDrawing.Reset();
}

void PhysicsDebugDrawer::UpdateResources(ID3D11DeviceContext* pDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mapped;

	// DYNAMIC 버퍼 크기 이상으로 업데이트하지 않도록 업데이트 크기 클램프
	UINT vertexCountToDraw = GetDebugLineVertexCountToDraw();
	size_t updateSize = vertexCountToDraw * sizeof(DebugLineVertexFormat);

	pDeviceContext->Map(
		m_cpDebugLineVertices.Get(),
		D3D11CalcSubresource(0, 0, 0),
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);

	std::memcpy(mapped.pData, m_debugLineVertices.data(), updateSize);

	pDeviceContext->Unmap(m_cpDebugLineVertices.Get(), D3D11CalcSubresource(0, 0, 0));
}

UINT PhysicsDebugDrawer::GetDebugLineVertexCountToDraw() const
{
	return Math::Clamp(static_cast<UINT>(m_debugLineVertices.size()), 0U, static_cast<UINT>(MAX_DEBUG_LINES_PER_FRAME * 2));
}

void PhysicsDebugDrawer::ClearDebugInstanceCache()
{
	m_debugLineVertices.clear();
}

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	// 지연 렌더링

	DebugLineVertexFormat vFrom;
	vFrom.m_position = BulletDXMath::BtToDX(from);
	vFrom.m_color.x = color.getX();
	vFrom.m_color.y = color.getY();
	vFrom.m_color.z = color.getZ();
	vFrom.m_color.w = 1.0f;


	DebugLineVertexFormat vTo;
	vTo.m_position = BulletDXMath::BtToDX(to);
	vTo.m_color.x = color.getX();
	vTo.m_color.y = color.getY();
	vTo.m_color.z = color.getZ();
	vTo.m_color.w = 1.0f;


	m_debugLineVertices.push_back(vFrom);
	m_debugLineVertices.push_back(vTo);
}

void PhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance,
	int lifeTime, const btVector3& color)
{
	// 지연 렌더링
	// 접촉점도 선으로 렌더링
	btVector3 to = PointOnB + normalOnB * distance;
	drawLine(PointOnB, to, color);	
}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString)
{
	printf(warningString);
	putchar('\n');
}

void PhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void PhysicsDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const
{
	return m_debugMode;
}
