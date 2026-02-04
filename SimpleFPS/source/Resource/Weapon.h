#pragma once

#include <ZergEngine\ZergEngine.h>
#include <cstdint>
#include <cstddef>

class Weapon
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	Weapon();
	virtual ~Weapon() = default;
public:
	std::wstring m_name;
	ze::Texture2D m_previewImage;
	ze::Texture2D m_killLogImage;
};
