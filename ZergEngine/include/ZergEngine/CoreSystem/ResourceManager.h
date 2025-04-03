#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\Common\EngineConstants.h>

namespace ze
{
	class Mesh;
	class Skybox;
	class Texture2D;
	class Material;
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
			inline Position(uint32_t vi)
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
			inline PositionNormal(uint32_t vi, uint32_t vni)
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
			inline PositionTexCoord(uint32_t vi, uint32_t vti)
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
			inline PositionNormalTexCoord(uint32_t vi, uint32_t vni, uint32_t vti)
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

	class ResourceManager : public ISubsystem
	{
		friend class Runtime;
		ZE_DECLARE_SINGLETON(ResourceManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;
	public:
		std::vector<std::shared_ptr<Mesh>> CreateMesh(PCWSTR path);
		std::shared_ptr<Texture2D> CreateTexture(PCWSTR path);
		std::shared_ptr<Material> CreateMaterial();
		std::shared_ptr<Skybox> CreateSkybox(PCWSTR path);
		// std::vector<std::shared_ptr<Mesh>> LoadWavefrontOBJ_deprecated(const wchar_t* path, bool importTexture);
	private:
		bool ReadObject(FILE* const objFile, long* pofpos, VertexPack& vp, Mesh& mesh, const size_t meshIndex);
		bool ReadFaces(FILE* const objFile, long* pnffpos, VERTEX_FORMAT_TYPE vft, const VertexPack& vp,
			IndexMapPack& imp, Mesh& mesh, size_t meshIndex, RawVector& tempVB, std::vector<uint32_t>& tempIB);
		/*
		std::shared_ptr<Texture2D> LoadTexture_deprecated(const wchar_t* path);
		bool ReadFaces_deprecated(FILE* const objFile, long* pnffpos, const VERTEX_FORMAT_TYPE vft, const VertexPack& vp,
			IndexMapPack& imp, Mesh& mesh, size_t meshIndex, RawVector& tempVB, std::vector<uint32_t>& tempIB, std::vector<DeferredMtlLinkingData>& dml,
			SubsetAttribute& subsetAttribute);
		bool ReadObject_deprecated(FILE* const objFile, long* pofpos, VertexPack& vp, Mesh& mesh, const size_t meshIndex,
			std::vector<DeferredMtlLinkingData>& dml);
		*/
	private:
	};
}
