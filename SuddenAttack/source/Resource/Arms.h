#pragma once

#include <ZergEngine\ZergEngine.h>

class ArmsViewInfo
{
public:
	ArmsViewInfo(std::shared_ptr<ze::SkinnedMesh> spMesh, std::shared_ptr<ze::Armature> spArmature, std::vector<std::shared_ptr<ze::Material>> materials)
		: m_spMesh(std::move(spMesh))
		, m_spArmature(std::move(spArmature))
		, m_materials(std::move(materials))
	{
	}

	std::shared_ptr<ze::SkinnedMesh> GetMesh() const { return m_spMesh; }
	std::shared_ptr<ze::Armature> GetArmature() const { return m_spArmature; }
	const std::vector<std::shared_ptr<ze::Material>>& GetMaterials() const { return m_materials; }
private:
	std::shared_ptr<ze::SkinnedMesh> m_spMesh;
	std::shared_ptr<ze::Armature> m_spArmature;
	std::vector<std::shared_ptr<ze::Material>> m_materials;
};
