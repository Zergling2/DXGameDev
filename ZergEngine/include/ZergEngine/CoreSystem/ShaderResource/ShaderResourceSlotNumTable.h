#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

#define SLOT_NUMBER(ShaderResourceName, SlotNum) static constexpr uint32_t ShaderResourceName##_SLOTNUM = SlotNum;

namespace ze
{
	// 旨收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旬
	// 早                TEXTURE SAMPLER                早
	// 曲收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收旭
	SLOT_NUMBER(MESH_TEXTURE_SAMPLER, 0);
	SLOT_NUMBER(SKYBOX_SAMPLER, 1);
	SLOT_NUMBER(TERRAIN_TEXTURE_SAMPLER, 2);
	SLOT_NUMBER(HEIGHT_MAP_SAMPLER, 3);
}
