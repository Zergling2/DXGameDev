#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\RectTransform.h>

using namespace ze;

UIObjectManager* UIObjectManager::s_pInstance = nullptr;

UIObjectManager::UIObjectManager()
	: m_uniqueId(0)
	, m_lock()
	, m_destroyed()
	, m_roots()
	, m_activeGroup()
	, m_inactiveGroup()
	, m_handleTable(256, nullptr)
	, m_pObjectPressedByLButton(nullptr)
	, m_pObjectPressedByRButton(nullptr)
	, m_pObjectPressedByMButton(nullptr)
{
	m_lock.Init();
}

UIObjectManager::~UIObjectManager()
{
}

void UIObjectManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new UIObjectManager();
}

void UIObjectManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void UIObjectManager::Init()
{
	m_uniqueId = 0;
}

void UIObjectManager::UnInit()
{
	// 楛顫歜 贗楚蝶縑憮 �側�/綠�側� 螃粽薛お菟擊 賅舒 п薯ц橫撿 ж嘎煎
	assert(m_activeGroup.size() == 0);
	assert(m_inactiveGroup.size() == 0);

	m_handleTable.clear();
	m_uniqueId = 0;
}

void UIObjectManager::Deploy(IUIObject* pUIObject)
{
	pUIObject->OffFlag(UIOBJECT_FLAG::PENDING);	// 醞蹂!

	if (pUIObject->m_transform.m_pParentTransform == nullptr)
		this->AddToRootArray(pUIObject);

	pUIObject->IsActive() ? this->DeployToActiveGroup(pUIObject) : this->DeployToInactiveGroup(pUIObject);
}

void UIObjectManager::RequestDestroy(IUIObject* pUIObject)
{
	// 雖翱脹 螃粽薛お蒂 薯剪ж朝 唳辦朝 OnLoadScene縑憮 Destroy蒂 и棻朝 曖嘐檣等 檜匙擎 ъ辨ж雖 彊朝棻.
	if (pUIObject->IsPending())
		return;

	// 濠衝 螃粽薛お梱雖 Destroy
	for (RectTransform* pChildTransform : pUIObject->m_transform.m_children)
		pChildTransform->m_pUIObject->Destroy();

	this->AddToDestroyQueue(pUIObject);
}

IUIObject* UIObjectManager::ToPtr(uint32_t tableIndex, uint64_t id) const
{
	assert(tableIndex < UIObjectManager::GetInstance()->m_handleTable.size());

	IUIObject* pUIObject = UIObjectManager::GetInstance()->m_handleTable[tableIndex];

	if (pUIObject == nullptr || pUIObject->GetId() != id)
		return nullptr;
	else
		return pUIObject;
}

UIObjectHandle THREADSAFE UIObjectManager::RegisterToHandleTable(IUIObject* pUIObject)
{
	UIObjectHandle hUIObject;

	// Auto Exclusive Lock
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	// 纔檜綰縑 蛔煙
	// 綴 濠葬 匐儀
	uint32_t emptyIndex;
	bool find = false;
	const uint32_t tableSize = static_cast<uint32_t>(m_handleTable.size());
	for (uint32_t i = 0; i < tableSize; ++i)
	{
		if (m_handleTable[i] == nullptr)
		{
			emptyIndex = i;
			find = true;
			break;
		}
	}

	// 虜擒 綴 濠葬蒂 瓊雖 跤ц擊 唳辦
	if (!find)
	{
		emptyIndex = tableSize;
		m_handleTable.push_back(nullptr);	// 纔檜綰 奢除 �捏�
	}

	m_handleTable[emptyIndex] = pUIObject;
	pUIObject->m_tableIndex = emptyIndex;

	hUIObject = UIObjectHandle(emptyIndex, pUIObject->GetId());	// 嶸�褲� с菟 遽綠

	return hUIObject;
}

void UIObjectManager::AddToDestroyQueue(IUIObject* pUIObject)
{
	if (pUIObject->IsOnTheDestroyQueue())
		return;

	pUIObject->OnFlag(UIOBJECT_FLAG::ON_DESTROY_QUEUE);		// 衙辦 醞蹂! (漱攪縑 醞犒 鳶殮戲煎 檣и 醞犒 delete �� 槳鼻 寞雖)
	m_destroyed.push_back(pUIObject);
}

UIObjectHandle UIObjectManager::FindUIObject(PCWSTR name)
{
	UIObjectHandle hUIObject;	// null handle

	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (wcscmp(pUIObject->GetName(), name) == 0)
		{
			hUIObject = pUIObject->ToHandleBase();
			break;
		}
	}

	return hUIObject;
}

void UIObjectManager::MoveToActiveGroup(IUIObject* pUIObject)
{
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 1. Inactive group縑憮 薯剪
	assert(!pUIObject->IsActive());	// Inactive 鼻鷓艘橫撿 и棻.
	this->RemoveFromGroup(pUIObject);
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收


	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 2. Active group縑 蹺陛
	this->AddToActiveGroup(pUIObject);
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
}

void UIObjectManager::MoveToInactiveGroup(IUIObject* pUIObject)
{
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 1. Active group縑憮 薯剪
	assert(pUIObject->IsActive());	// Active 鼻鷓艘橫撿 и棻.
	this->RemoveFromGroup(pUIObject);
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收


	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 2. Inactive group縑 蹺陛
	this->AddToInactiveGroup(pUIObject);
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
}

void UIObjectManager::AddToRootArray(IUIObject* pUIObject)
{
	assert(pUIObject->m_transform.m_pParentTransform == nullptr);

	m_roots.push_back(pUIObject);
	pUIObject->OnFlag(UIOBJECT_FLAG::REAL_ROOT);	// 癱殮 衛 REAL_ROOT Ы楚斜蒂 黨棻.
}

void UIObjectManager::DeployToActiveGroup(IUIObject* pUIObject)
{
	assert(pUIObject->IsActive()); AddToActiveGroup(pUIObject);
}

void UIObjectManager::DeployToInactiveGroup(IUIObject* pUIObject)
{
	assert(!pUIObject->IsActive()); AddToInactiveGroup(pUIObject);
}

void UIObjectManager::AddToActiveGroup(IUIObject* pUIObject)
{
	assert(pUIObject->IsActive());	// 檜 л熱朝 壁縑憮 煎萄脹 螃粽薛お陛 瞪羲 婦葬濠煎 癱殮腆 陽 ��轎腎嘎煎 ACTIVE Ы楚斜陛 麵螳氈橫撿 и棻.

	m_activeGroup.push_back(pUIObject);
	pUIObject->m_groupIndex = static_cast<uint32_t>(m_activeGroup.size() - 1);

	pUIObject->OnFlag(UIOBJECT_FLAG::ACTIVE);
}

void UIObjectManager::AddToInactiveGroup(IUIObject* pUIObject)
{
	assert(!pUIObject->IsActive());	// 檜 л熱朝 壁縑憮 煎萄脹 螃粽薛お陛 瞪羲 婦葬濠煎 癱殮腆 陽 ��轎腎嘎煎 ACTIVE Ы楚斜陛 疏螳氈橫撿 и棻.

	m_inactiveGroup.push_back(pUIObject);
	pUIObject->m_groupIndex = static_cast<uint32_t>(m_inactiveGroup.size() - 1);

	pUIObject->OffFlag(UIOBJECT_FLAG::ACTIVE);
}

void UIObjectManager::RemoveFromRootArray(IUIObject* pUIObject)
{
	/*
	渠睡碟 UI 掘撩 衛 瑞お 螃粽薛お曖 偃熱朝 瞳戲嘎煎 瑞お ん檣攪菟擊 摹⑽ 掘褻煎 盪濰 塽 匐儀и棻.
	*/

	assert(m_roots.size() > 0);

#if defined (DEBUG) || (_DEBUG)
	bool find = false;
#endif
	auto end = m_roots.cend();
	auto iter = m_roots.cbegin();
	while (iter != end)
	{
		if (*iter == pUIObject)
		{
#if defined (DEBUG) || (_DEBUG)
			find = true;
#endif
			m_roots.erase(iter);
			break;
		}

		++iter;
	}

	assert(find == true);

	pUIObject->OffFlag(UIOBJECT_FLAG::REAL_ROOT);
}

void UIObjectManager::RemoveFromGroup(IUIObject* pUIObject)
{
	auto& group = pUIObject->IsActive() ? m_activeGroup : m_inactiveGroup;
	
	uint32_t groupSize = static_cast<uint32_t>(group.size());
	assert(groupSize > 0);

	const uint32_t groupIndex = pUIObject->m_groupIndex;
	const uint32_t lastIndex = groupSize - 1;

	assert(groupIndex < groupSize);
	// assert(groupIndex != std::numeric_limits<uint32_t>::max());
	assert(group[groupIndex] == pUIObject);	// 醞蹂!

	// 雖辦溥朝 螃粽薛お ん檣攪陛 裔 菴縑 氈朝匙檜 嬴棋唳辦
	// 葆雖虞縑 嬪纂и ん檣攪諦 嬪纂蒂 夥羞棻.
	if (groupIndex != lastIndex)
	{
		std::swap(group[groupIndex], group[lastIndex]);
		group[groupIndex]->m_groupIndex = groupIndex;	// 葆雖虞縑 氈湍 螃粽薛お曖 groupIndex蒂 螢夥腦啪 機等檜お п輿橫撿 л.
	}

	// 模樓腎橫 氈湍 vector縑憮 薯剪
	group.pop_back();
}

void UIObjectManager::RemoveDestroyedUIObjects()
{
	// だ惚脹 啪歜螃粽薛お 薯剪 紫醞 綠翕晦 壁 煎萄 婁薑縑憮曖 啪歜螃粽薛お 儅撩戲煎 檣п m_table曖 營й渡檜 橾橫陳 熱 氈戲嘎煎
	// 塊 �僱磍� 奩萄衛 в蹂ж棻.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (IUIObject* pUIObject : m_destroyed)
	{
		/* 匐隸 */
		assert(pUIObject->IsPending() == false);	// 煎註 壁 模樓曖 螃粽薛お朝 だ惚腆 熱 橈棻.
		assert(pUIObject->IsOnTheDestroyQueue() == true);	// だ惚 聽縑 菟橫螞 唳辦縑朝 檜 ON_DESTROY_QUEUE Ы楚斜陛 麵螳 氈橫撿虜 и棻.

		if (pUIObject == m_pObjectPressedByLButton)
			m_pObjectPressedByLButton = nullptr;

		// Step 1. Transform 濠衝 睡賅 翱唸 薯剪
		RectTransform* pTransform = &pUIObject->m_transform;
		RectTransform* pParentTransform = pTransform->m_pParentTransform;
		if (pParentTransform != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			bool find = false;
#endif
			std::vector<RectTransform*>::const_iterator end = pParentTransform->m_children.cend();
			std::vector<RectTransform*>::const_iterator iter = pParentTransform->m_children.cbegin();
			while (iter != end)
			{
				if (*iter == pTransform)
				{
#if defined(DEBUG) || defined(_DEBUG)
					find = true;
#endif
					pParentTransform->m_children.erase(iter);
					break;
				}
				++iter;
			}
			assert(find == true);
		}

		for (RectTransform* pChildTransform : pTransform->m_children)
		{
			assert(pChildTransform->m_pParentTransform == pTransform);	// 濠褐婁曖 翱唸 �挫�


			// 醞蹂 ん檣お
			pChildTransform->m_pParentTransform = nullptr;	// 壽縑憮 埠 delete腎朝 濠褐擊 蕾斬(Step 1縑憮 蕾斬л)ж朝 匙擊 寞雖 (濡營瞳 湮旋葭 ん檣攪 薯剪)
			// 檜煎 檣п憮 偌羹 だ惚 衛縑朝 瑞お 螃粽薛お艘朝雖 っ滌(RootGroup縑憮 ん檣攪 薯剪п撿 л)擊 й 陽 睡賅 Transform ん檣攪煎
			// っ欽й 熱 橈棻. 評塭憮 霞瞼 瑞お 喻萄艘朝雖蒂 羹觼п場晦 嬪п UIOBJECT_FLAG翮剪⑽戲煎 REAL_ROOT塭朝 Ы楚斜蒂 餌辨и棻.
		}
		// pTransform->m_children.clear();	// 偌羹 delete衛 濠翕 模資

		if (pUIObject->IsRoot())
			this->RemoveFromRootArray(pUIObject);

		this->RemoveFromGroup(pUIObject);

		// с菟 纔檜綰縑憮 薯剪
		assert(m_handleTable[pUIObject->m_tableIndex] == pUIObject);
		m_handleTable[pUIObject->m_tableIndex] = nullptr;

		// for debugging...
		pUIObject->m_groupIndex = std::numeric_limits<uint32_t>::max();
		pUIObject->m_tableIndex = std::numeric_limits<uint32_t>::max();

		delete pUIObject;
	}

	m_destroyed.clear();
}

void UIObjectManager::SetActive(IUIObject* pUIObject, bool active)
{
	// 檜嘐 п渡 �側� 鼻鷓陛 撲薑腎橫 氈朝 唳辦 л熱 葬欐
	if (pUIObject->IsActive() == active)
		return;

	if (active)
	{
		// PENDING 鼻鷓陛 嬴棋 唳辦縑虜 ん檣攪 檜翕 (PENDING 鼻鷓縑憮朝 Active/Inactive 漱攪縑 ん檣攪陛 襄營ж雖 彊朝棻.)
		if (!pUIObject->IsPending())
			this->MoveToActiveGroup(pUIObject);
	}
	else
	{
		// PENDING 鼻鷓陛 嬴棋 唳辦縑虜 ん檣攪 檜翕 (PENDING 鼻鷓縑憮朝 Active/Inactive 漱攪縑 ん檣攪陛 襄營ж雖 彊朝棻.)
		if (!pUIObject->IsPending())
			this->MoveToInactiveGroup(pUIObject);
	}
}

bool UIObjectManager::SetParent(RectTransform* pTransform, RectTransform* pNewParentTransform)
{
	IUIObject* pUIObject = pTransform->m_pUIObject;
	RectTransform* pOldParentTransform = pTransform->m_pParentTransform;

	if (pUIObject->IsOnTheDestroyQueue())
		return false;

	// 濠晦 濠褐擊 睡賅煎 撲薑ж溥堅 ж剪釭 檜嘐 撲薑ж溥朝 睡賅陛 檜嘐 睡賅檣 唳辦縑朝 褒ぬ
	if (pTransform == pNewParentTransform || pOldParentTransform == pNewParentTransform)
		return false;

	// 餌檜贗 匐餌
	// pTransform檜 檜嘐 釭蒂 褻鼻戲煎 ж堅 氈朝 唳辦 or だ惚 蕨薑檣 螃粽薛お蒂 睡賅煎 撲薑ж溥朝 唳辦
	// 蕨諼籀葬 п輿雖 彊戲賊 濠衝 螃粽薛お菟梱雖 賅舒 Destroy 聽縑 厥橫輿朝 薑疇婁 橾婦撩檜 蜃雖 彊朝棻.
	// (睡賅朝 だ惚腎朝等 濠衝擎 だ惚腎雖 彊朝 賅牖 嫦儅)
	if (pNewParentTransform != nullptr)
		if (pNewParentTransform->IsDescendantOf(pTransform) || pNewParentTransform->m_pUIObject->IsOnTheDestroyQueue())
			return false;

	// 瑞お 喻萄 籀葬 唳辦曖 熱
	// 1. pOldParentTransform != nullptr && pNewParentTransform == nullptr (晦襄縑 瑞お 喻萄陛 嬴棲歷堅 檜薯 瑞お 喻萄陛 腎溥朝 唳辦)
	// 2. pOldParentTransform != nullptr && pNewParentTransform != nullptr (晦襄縑 瑞お 喻萄陛 嬴棲歷堅 雖旎紫 瑞お 喻萄陛 腎朝 匙擎 嬴棋 唳辦)
	// 3. pOldParentTransform == nullptr && pNewParentTransform == nullptr (晦襄縑 瑞お 喻萄艘堅 雖旎紫 瑞お 喻萄陛 腎溥朝 唳辦) (檜 唳辦朝 л熱 霞殮 霜�� 蕨諼籀葬脾)
	// 4. pOldParentTransform == nullptr && pNewParentTransform != nullptr (晦襄縑 瑞お 喻萄艘堅 檜薯 瑞お 喻萄陛 嬴棲啪 腎朝 唳辦)

	// 睡賅陛 nullptr檜 嬴棲歷湍 唳辦縑朝 晦襄 睡賅縑憮 濠衝 ん檣攪蒂 瓊嬴憮 薯剪
	if (pOldParentTransform != nullptr)
	{
#if defined(DEBUG) || defined(_DEBUG)
		bool find = false;
#endif
		std::vector<RectTransform*>::const_iterator end = pOldParentTransform->m_children.cend();
		std::vector<RectTransform*>::const_iterator iter = pOldParentTransform->m_children.cbegin();
		while (iter != end)
		{
			if (*iter == pTransform)
			{
				pOldParentTransform->m_children.erase(iter);
#if defined(DEBUG) || defined(_DEBUG)
				find = true;
#endif
				break;
			}
			++iter;
		}
		assert(find == true);	// 濠衝戲煎 襄營ц歷橫撿 л
	}

	// 睡賅曖 濠衝 跡煙擊 機等檜お
	if (pNewParentTransform != nullptr)
		pNewParentTransform->m_children.push_back(pTransform);

	// 睡賅 ん檣攪蒂 億煎遴 睡賅煎 機等檜お
	pTransform->m_pParentTransform = pNewParentTransform;


	// 煎註 醞檜 嬴棋 螃粽薛お朝 Root 斜瑜縑憮 襄營й雖 罹睡蒂 機等檜おп撿л.
	if (!pUIObject->IsPending())
	{
		if (pOldParentTransform == nullptr)
		{
			if (pNewParentTransform != nullptr)
			{
				this->RemoveFromRootArray(pUIObject);
			}
		}
		else
		{
			if (pNewParentTransform == nullptr)
			{
				this->AddToRootArray(pUIObject);
			}
		}
	}

	return true;
}

void UIObjectManager::OnLButtonDown(POINT pt)
{
	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pUIObject->OnLButtonDown();
			m_pObjectPressedByLButton = pUIObject;
			break;
		}
	}
}

void UIObjectManager::OnLButtonUp(POINT pt)
{
	// 揚溥氈湍 UI螃粽薛お陛 橈湍 唳辦縑朝 橫雯 籀葬紫 п還 в蹂陛 橈棻.
	if (m_pObjectPressedByLButton == nullptr)
		return;

	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	IUIObject* pLBtnUpObject = nullptr;
	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pLBtnUpObject = pUIObject;
			break;
		}
	}

	m_pObjectPressedByLButton->OnLButtonUp();

	if (m_pObjectPressedByLButton == pLBtnUpObject)
		m_pObjectPressedByLButton->OnLClick();

	m_pObjectPressedByLButton = nullptr;
}

void UIObjectManager::OnRButtonDown(POINT pt)
{
	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pUIObject->OnRButtonDown();
			m_pObjectPressedByRButton = pUIObject;
			break;
		}
	}
}

void UIObjectManager::OnRButtonUp(POINT pt)
{
	// 揚溥氈湍 UI螃粽薛お陛 橈湍 唳辦縑朝 橫雯 籀葬紫 п還 в蹂陛 橈棻.
	if (m_pObjectPressedByRButton == nullptr)
		return;

	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	IUIObject* pRBtnUpObject = nullptr;
	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pRBtnUpObject = pUIObject;
			break;
		}
	}

	m_pObjectPressedByRButton->OnRButtonUp();

	if (m_pObjectPressedByRButton == pRBtnUpObject)
		m_pObjectPressedByRButton->OnRClick();

	m_pObjectPressedByRButton = nullptr;
}

void UIObjectManager::OnMButtonDown(POINT pt)
{
	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pUIObject->OnMButtonDown();
			m_pObjectPressedByMButton = pUIObject;
			break;
		}
	}
}

void UIObjectManager::OnMButtonUp(POINT pt)
{
	// 揚溥氈湍 UI螃粽薛お陛 橈湍 唳辦縑朝 橫雯 籀葬紫 п還 в蹂陛 橈棻.
	if (m_pObjectPressedByMButton == nullptr)
		return;

	XMVECTOR mousePosition = XMVectorSet(static_cast<FLOAT>(pt.x), static_cast<FLOAT>(pt.y), 0.0f, 0.0f);

	IUIObject* pMBtnUpObject = nullptr;
	for (IUIObject* pUIObject : m_activeGroup)
	{
		if (pUIObject->HitTest(mousePosition))
		{
			pMBtnUpObject = pUIObject;
			break;
		}
	}

	m_pObjectPressedByMButton->OnMButtonUp();

	if (m_pObjectPressedByMButton == pMBtnUpObject)
		m_pObjectPressedByMButton->OnMClick();

	m_pObjectPressedByMButton = nullptr;
}
