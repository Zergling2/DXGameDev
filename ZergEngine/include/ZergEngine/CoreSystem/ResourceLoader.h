#pragma once

#include <ZergEngine\CoreSystem\EngineConstants.h>
#include <ZergEngine\CoreSystem\Resource\Texture.h>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <memory>
#include <string>

struct aiScene;
struct aiNode;
struct aiBone;
struct aiAnimation;

// ##################################
// Forward declaration
namespace DirectX
{
	class ScratchImage;
}

// ##################################

namespace ze
{
	class StaticMesh;
	class Armature;
	class SkinnedMesh;
	class Material;
	class RawVector;

	class IndexSet
	{
	public:
		struct Position
		{
			Position(uint32_t vi)
				: m_vi(vi)
			{
			}
			bool operator<(const Position& rhs) const
			{
				return this->m_vi < rhs.m_vi;
			}
			uint32_t m_vi;
		};

		struct PositionNormal
		{
			PositionNormal(uint32_t vi, uint32_t vni)
				: m_vi(vi)
				, m_vni(vni)
			{
			}
			bool operator<(const PositionNormal& rhs) const
			{
				if (this->m_vi == rhs.m_vi)
					return this->m_vni < rhs.m_vni;
				else
					return this->m_vi < rhs.m_vi;
			}
			uint32_t m_vi;
			uint32_t m_vni;
		};

		struct PositionTexCoord
		{
			PositionTexCoord(uint32_t vi, uint32_t vti)
				: m_vi(vi)
				, m_vti(vti)
			{
			}
			bool operator<(const PositionTexCoord& rhs) const
			{
				if (this->m_vi == rhs.m_vi)
					return this->m_vti < rhs.m_vti;
				else
					return this->m_vi < rhs.m_vi;
			}
			uint32_t m_vi;
			uint32_t m_vti;
		};

		struct PositionNormalTexCoord
		{
			PositionNormalTexCoord(uint32_t vi, uint32_t vni, uint32_t vti)
				: m_vi(vi)
				, m_vni(vni)
				, m_vti(vti)
			{
			}
			bool operator<(const PositionNormalTexCoord& rhs) const
			{
				if (this->m_vi == rhs.m_vi)
				{
					if (this->m_vni == rhs.m_vni)
						return this->m_vti < rhs.m_vti;
					else
						return this->m_vni < rhs.m_vni;
				}
				else
					return this->m_vi < rhs.m_vi;
			}
			uint32_t m_vi;
			uint32_t m_vni;
			uint32_t m_vti;
		};
	};

	struct DeferredMtlLinkingData
	{
		DeferredMtlLinkingData(const wchar_t* mtlName, size_t meshIndex, size_t subsetIndex)
			: m_mtlName(mtlName)
			, m_meshIndex(meshIndex)
			, m_subsetIndex(subsetIndex)
		{
		}
		std::wstring m_mtlName;
		size_t m_meshIndex;
		size_t m_subsetIndex;
	};

	struct VertexPack
	{
		std::vector<XMFLOAT3> v;
		std::vector<XMFLOAT3> vn;
		std::vector<XMFLOAT2> vt;
	};

	struct IndexMapPack
	{
		std::map<IndexSet::Position, uint32_t> vIndexMap;
		std::map<IndexSet::PositionTexCoord, uint32_t> vvtIndexMap;
		std::map<IndexSet::PositionNormal, uint32_t> vvnIndexMap;
		std::map<IndexSet::PositionNormalTexCoord, uint32_t> vvnvtIndexMap;
	};

	struct ModelData
	{
		std::vector<std::shared_ptr<StaticMesh>> staticMeshes;
		std::vector<std::shared_ptr<Armature>> armatures;
		std::vector<std::shared_ptr<SkinnedMesh>> skinnedMeshes;
	};

	class ResourceLoader
	{
		friend class Runtime;

		struct TempModelData
		{
			std::vector<std::shared_ptr<StaticMesh>> staticMeshes;
			struct ArmatureData
			{
				std::set<std::string> nameSet;
				std::vector<std::unordered_map<std::string, BYTE>> boneIndexMap;
				std::vector<std::shared_ptr<Armature>> armatures;
			}armatureData;
			std::vector<std::shared_ptr<SkinnedMesh>> skinnedMeshes;
		};
	public:
		static ResourceLoader* GetInstance() { return s_pInstance; }
	private:
		ResourceLoader();
		~ResourceLoader();

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();
	public:
		ModelData LoadModel(PCWSTR path);

		// Wavefront OBJ 포맷은 단일 obj 파일에 여러 개의 메시를 포함할 수 있습니다.
		Texture2D LoadTexture2D(PCWSTR path, bool generateMipMaps = true);	// 리소스에 밉 맵이 포함된 경우(dds 파일 등)에는 generateMipMaps가 무시됩니다.
		Texture2D GetErrorTexture2D() const { return m_errTex; }
		std::shared_ptr<Material> CreateMaterial();
		bool CreateHeightMapFromRawData(Texture2D& heightMap, const uint16_t* pData, SIZE resolution);
		// std::vector<std::shared_ptr<StaticMesh>> LoadWavefrontOBJ(PCWSTR path);	// Deprecated
		// std::vector<std::shared_ptr<StaticMesh>> LoadWavefrontOBJ_deprecated(PCWSTR path, bool importTexture);
	private:
		void DFSAiNodeLoadModel(TempModelData& tmd, const aiScene* pAiScene, const aiNode* pAiNode);
		void AiLoadStaticMeshNode(TempModelData& tmd, const aiScene* pAiScene, const aiNode* pAiNode);
		void AiLoadAnimation(const TempModelData& tmd, const aiScene* pAiScene);
		void AiLoadSkinnedMeshNode(TempModelData& tmd, const aiScene* pAiScene, const aiNode* pAiNode);
		HRESULT GenerateMipMapsForBCFormat(const ScratchImage& src, ScratchImage& result);

		/*
		bool ParseWavefrontOBJObject(FILE* pOBJFile, long* pofpos, VertexPack& vp, StaticMesh* pMesh);
		bool ParseWavefrontOBJFaces(FILE* pOBJFile, long* pnffpos, VertexFormatType vft, const VertexPack& vp,
			IndexMapPack& imp, StaticMesh* pMesh, RawVector& tempVB, std::vector<uint32_t>& tempIB);
		
		Texture2D LoadCubeMapTexture_deprecated(PCWSTR path);		// dds 포맷만 지원
		Texture2D LoadTexture_deprecated(PCWSTR path);
		bool ReadFaces_deprecated(FILE* pOBJFile, long* pnffpos, const VertexFormatType vft, const VertexPack& vp,
			IndexMapPack& imp, StaticMesh& mesh, size_t meshIndex, RawVector& tempVB, std::vector<uint32_t>& tempIB, std::vector<DeferredMtlLinkingData>& dml,
			SubsetAttribute& subsetAttribute);
		bool ReadObject_deprecated(FILE* pOBJFile, long* pofpos, VertexPack& vp, StaticMesh& mesh, const size_t meshIndex,
			std::vector<DeferredMtlLinkingData>& dml);
		*/
	private:
		static ResourceLoader* s_pInstance;
		Texture2D m_errTex;
	};
}
