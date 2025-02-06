#include <ZergEngine\System\Terrain.h>

using namespace zergengine;

Terrain::Terrain(const TerrainMapDescriptor& tmd)
	: m_tmd(tmd)
	, m_numPatchVertexRow((tmd.heightMapSize.row - 1) / CELLS_PER_TERRAIN_PATCH + 1)
	, m_numPatchVertexCol((tmd.heightMapSize.column - 1) / CELLS_PER_TERRAIN_PATCH + 1)
	, m_patchCtrlPtIndexCount((m_numPatchVertexRow - 1) * (m_numPatchVertexCol - 1) * 4)
	, m_pHeightData(new(std::nothrow) float[tmd.heightMapSize.row * tmd.heightMapSize.column])
	, m_patchCtrlPtBuffer(nullptr)
	, m_patchCtrlPtIndexBuffer(nullptr)
	, m_srvHeightMap(nullptr)
{
}

Terrain::~Terrain()
{
	if (m_pHeightData)
		delete[] m_pHeightData;
}
