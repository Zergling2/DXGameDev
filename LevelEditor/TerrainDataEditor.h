#pragma once

#include "framework.h"
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Terrain.h>

namespace ze
{
	class Terrain;
	class Ray;
}

// Terrain Hegith Data 파일 (.thd 파일) 헤더
struct THDFileHeader
{
	uint32_t m_width;
	uint32_t m_height;
};

// Terrain Blend Data 파일 (.tbd 파일) 헤더
struct TBDFileHeader
{
	uint32_t m_width;
	uint32_t m_height;
};

class TerrainDataEditor
{
public:
	TerrainDataEditor()
		: m_pTerrainToModify(nullptr)
		, m_heights()
		, m_normals()
	{
	}
	~TerrainDataEditor() = default;

	ze::Terrain* GetModifyingTerrain() const { return m_pTerrainToModify; }
	bool GetModifyingTerrainSize(uint32_t* pWidth, uint32_t* pHeight);
	void SetTerrainToModify(ze::Terrain* pTerrain);			// pTerrain이 소유하고 있는 자료형과 동일한 스테이징 자원들 생성

	// 수정 시 시스템 TODO
	// 1. CPU 메모리의 지형 데이터를 수정.
	// 2. ze::Terrain의 DEFAULT 타입 지형 리소스들을 UpdateSubresource로 업데이트 (D3D11_BOX 이용해 변경된 섹터만 업데이트 가능)
	// 지형 높이를 수정하는 경우 높이가 변경된 정점 상하좌우 인접 정점들의 노말에 영향을 주므로 노말을 재계산하고 섹터 업데이트가 필요하다면 업데이트
	// 해주어야 함.
	// 패치 제어점 정점버퍼도 64x64 섹터 가장자리 정점에 변경 가해진것을 올바르게 업데이트 해줘야함. (지형 테셀레이션 인자 계산에 영향)
	void RaiseOrLowerTerrain(bool raise, const ze::Ray& ray, float radius, float opacity);

	void PaintTexture(const ze::Ray& ray, float radius, uint8_t opacity);

	// opacity: 타겟 높이에 도달하는 속도
	void SetHeight(const ze::Ray& ray, float radius, uint16_t height, float opacity);

	void SmoothHeight(const ze::Ray& ray, float radius, float opacity);

	const uint16_t* GetHeightData() const { return m_heights.data(); }
	const ze::TerrainNormalMapFormat* GetNormalData() const { return m_normals.data(); }
	const ze::TerrainBlendMapFormat* GetBlendData() const { return m_blends.data(); }
private:
	ze::Terrain* m_pTerrainToModify;
	std::vector<uint16_t> m_heights;
	std::vector<ze::TerrainNormalMapFormat> m_normals;
	std::vector<ze::TerrainBlendMapFormat> m_blends;
};
