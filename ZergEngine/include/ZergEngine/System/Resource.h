#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class Material;
	class Texture2D;
	class Mesh;
	class Skybox;
	class Terrain;
	struct TerrainMapDescriptor;
	class RawVector;

	/*
	struct SubsetAttribute
	{
		bool m_shadeSmooth;
		const wchar_t* m_mtlName;
	};
	*/

	class IndexSet
	{
	public:
		struct Position
		{
			inline Position(index_format vi)
				: m_vi(vi)
			{
			}
			bool operator<(const Position& rhs) const
			{
				return this->m_vi < rhs.m_vi;
			}
			index_format m_vi;
		};

		struct PositionNormal
		{
			inline PositionNormal(index_format vi, index_format vni)
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
			index_format m_vi;
			index_format m_vni;
		};

		struct PositionTexCoord
		{
			inline PositionTexCoord(index_format vi, index_format vti)
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
			index_format m_vi;
			index_format m_vti;
		};

		struct PositionNormalTexCoord
		{
			inline PositionNormalTexCoord(index_format vi, index_format vni, index_format vti)
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
			index_format m_vi;
			index_format m_vni;
			index_format m_vti;
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
		std::map<IndexSet::Position, index_format> vIndexMap;
		std::map<IndexSet::PositionTexCoord, index_format> vvtIndexMap;
		std::map<IndexSet::PositionNormal, index_format> vvnIndexMap;
		std::map<IndexSet::PositionNormalTexCoord, index_format> vvnvtIndexMap;
	};

	class Resource
	{
		friend class Engine;
		friend class SceneManager;
	public:
		static std::vector<std::shared_ptr<Mesh>> LoadMesh(const wchar_t* path);
		static std::shared_ptr<Skybox> LoadSkybox(const wchar_t* path);

		// Create for mesh texturing purposes.
		static std::shared_ptr<Texture2D> LoadTexture(const wchar_t* path);
		static std::shared_ptr<Texture2D> LoadNormalMap(const wchar_t* path);
		static std::shared_ptr<Terrain> LoadTerrain(const wchar_t* path, const TerrainMapDescriptor& tmi);
		static std::shared_ptr<Material> CreateMaterial();
		// static std::vector<std::shared_ptr<Mesh>> LoadWavefrontOBJ_deprecated(const wchar_t* path, bool importTexture);
	private:
		static bool ReadObject(FILE* const objFile, long* pofpos, VertexPack& vp, Mesh& mesh, const size_t meshIndex);
		static bool ReadFaces(FILE* const objFile, long* pnffpos, const VERTEX_FORMAT_TYPE vft, const VertexPack& vp,
			IndexMapPack& imp, Mesh& mesh, size_t meshIndex, RawVector& tempVB, std::vector<uint32_t>& tempIB);
		/*
		static std::shared_ptr<Texture2D> LoadTexture_deprecated(const wchar_t* path);
		static bool ReadFaces_deprecated(FILE* const objFile, long* pnffpos, const VERTEX_FORMAT_TYPE vft, const VertexPack& vp,
			IndexMapPack& imp, Mesh& mesh, size_t meshIndex, RawVector& tempVB, std::vector<uint32_t>& tempIB, std::vector<DeferredMtlLinkingData>& dml,
			SubsetAttribute& subsetAttribute);
		static bool ReadObject_deprecated(FILE* const objFile, long* pofpos, VertexPack& vp, Mesh& mesh, const size_t meshIndex,
			std::vector<DeferredMtlLinkingData>& dml);
		*/
	private:
		// static std::unordered_map<std::wstring, std::weak_ptr<Mesh>> s_meshMap;
		// static std::unordered_map<std::wstring, std::weak_ptr<Texture2D>> s_textureMap;
	};
}
