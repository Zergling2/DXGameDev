#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

#define SLOT_NUMBER(ShaderResourceName, SlotNum) static constexpr uint32_t ShaderResourceName##_SLOTNUM = SlotNum;

namespace ze
{
	// ��������������������������������������������������������������������������������������������������
	// ��                TEXTURE SAMPLER                ��
	// ��������������������������������������������������������������������������������������������������
	SLOT_NUMBER(MESH_TEXTURE_SAMPLER, 0);
	SLOT_NUMBER(SKYBOX_SAMPLER, 1);
	SLOT_NUMBER(TERRAIN_TEXTURE_SAMPLER, 2);
	SLOT_NUMBER(HEIGHT_MAP_SAMPLER, 3);
}
