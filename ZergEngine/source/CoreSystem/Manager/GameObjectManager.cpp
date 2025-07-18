#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

GameObjectManager* GameObjectManager::s_pInstance = nullptr;

GameObjectManager::GameObjectManager()
	: m_uniqueId(0)
	, m_lock()
	, m_activeGroup()
	, m_inactiveGroup()
	, m_handleTable(8192, nullptr)
	, m_destroyed()
{
	m_lock.Init();
}

GameObjectManager::~GameObjectManager()
{
}

void GameObjectManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new GameObjectManager();
}

void GameObjectManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void GameObjectManager::Init()
{
	m_uniqueId = 0;
}

void GameObjectManager::UnInit()
{
	// 楛顫歜縑憮 嘐葬 賅舒 п薯衛儷橫撿 ж嘎煎
	assert(m_activeGroup.size() == 0);
	assert(m_inactiveGroup.size() == 0);

	m_handleTable.clear();
}

void GameObjectManager::Deploy(GameObject* pGameObject)
{
	// 醞蹂 (Ы楚斜 薯剪)
	pGameObject->OffFlag(GAMEOBJECT_FLAG::PENDING);

	pGameObject->IsActive() ? this->AddToActiveGroup(pGameObject) : this->AddToInactiveGroup(pGameObject);
}

GameObjectHandle GameObjectManager::FindGameObject(PCWSTR name)
{
	GameObjectHandle hGameObject;	// null handle

	for (GameObject* pGameObject : m_activeGroup)
	{
		if (wcscmp(pGameObject->GetName(), name) == 0)
		{
			hGameObject = pGameObject->ToHandle();
			break;
		}
	}

	return hGameObject;
}

GameObjectHandle GameObjectManager::CreateObject(PCWSTR name)
{
	GAMEOBJECT_FLAG flag = GAMEOBJECT_FLAG::ACTIVE;

	GameObject* pNewGameObject = new GameObject(this->AssignUniqueId(), flag, name);

	GameObjectHandle hNewGameObject = this->RegisterToHandleTable(pNewGameObject);
	this->AddToActiveGroup(pNewGameObject);

	return hNewGameObject;
}

GameObjectHandle GameObjectManager::CreatePendingObject(GameObject** ppNewGameObject, PCWSTR name)
{
	GAMEOBJECT_FLAG flag =
		static_cast<GAMEOBJECT_FLAG>(static_cast<uint16_t>(GAMEOBJECT_FLAG::PENDING) | static_cast<uint16_t>(GAMEOBJECT_FLAG::ACTIVE));

	// PENDING GAME OBJECT
	GameObject* pNewGameObject = new GameObject(this->AssignUniqueId(), flag, name);
	*ppNewGameObject = pNewGameObject;
	return this->RegisterToHandleTable(pNewGameObject);
}

void GameObjectManager::RequestDestroy(GameObject* pGameObject)
{
	// 雖翱脹 螃粽薛お蒂 薯剪ж朝 唳辦朝 OnLoadScene縑憮 Destroy蒂 и棻朝 曖嘐檣等 檜匙擎 ъ辨ж雖 彊朝棻.
	if (pGameObject->IsPending())
		return;

	// 濠衝 螃粽薛お梱雖 Destroy
	for (Transform* pChildTransform : pGameObject->m_transform.m_children)
		pChildTransform->m_pGameObject->Destroy();

	// 模嶸ж堅 氈朝 賅萇 闡ん凱お 薯剪
	for (IComponent* pComponent : pGameObject->m_components)
		pComponent->Destroy();	// 頂睡縑憮 埠夥煎 GameObject <-> Component除 翱唸 莒戲賊 寰脾 (雖旎 牖�裔炴� 檜攪溯檜攪 蜂陛颶!) (雖翱餉薯)

	this->AddToDestroyQueue(pGameObject);
}

GameObject* GameObjectManager::ToPtr(uint32_t tableIndex, uint64_t id) const
{
	assert(tableIndex < GameObjectManager::GetInstance()->m_handleTable.size());

	GameObject* pGameObject = GameObjectManager::GetInstance()->m_handleTable[tableIndex];

	if (pGameObject == nullptr || pGameObject->GetId() != id)
		return nullptr;
	else
		return pGameObject;
}

GameObjectHandle THREADSAFE GameObjectManager::RegisterToHandleTable(GameObject* pGameObject)
{
	GameObjectHandle hGameObject;

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

	m_handleTable[emptyIndex] = pGameObject;
	pGameObject->m_tableIndex = emptyIndex;

	hGameObject = GameObjectHandle(emptyIndex, pGameObject->GetId());	// 嶸�褲� с菟 遽綠

	return hGameObject;
}

void GameObjectManager::AddToDestroyQueue(GameObject* pGameObject)
{
	if (pGameObject->IsOnTheDestroyQueue())	// 衙辦 醞蹂! (醞犒 鳶殮戲煎 檣и 醞犒 delete �� 槳鼻 寞雖)
		return;

	m_destroyed.push_back(pGameObject);
	pGameObject->OnFlag(GAMEOBJECT_FLAG::ON_DESTROY_QUEUE);
}

void GameObjectManager::MoveToActiveGroup(GameObject* pGameObject)
{
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 1. Inactive group縑憮 薯剪
	assert(!pGameObject->IsActive());	// Inactive 鼻鷓艘橫撿 и棻.
	this->RemoveFromGroup(pGameObject);
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收


	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 2. Active group縑 蹺陛
	this->AddToActiveGroup(pGameObject);
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
}

void GameObjectManager::MoveToInactiveGroup(GameObject* pGameObject)
{
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 1. Active group縑憮 薯剪
	assert(pGameObject->IsActive());	// Active 鼻鷓艘橫撿 и棻.
	this->RemoveFromGroup(pGameObject);
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收


	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
	// 2. Inactive group縑 蹺陛
	this->AddToInactiveGroup(pGameObject);
	// 收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收收
}

void GameObjectManager::DeployToActiveGroup(GameObject* pGameObject)
{
	assert(pGameObject->IsActive());
	AddToActiveGroup(pGameObject);
}

void GameObjectManager::DeployToInactiveGroup(GameObject* pGameObject)
{
	assert(!pGameObject->IsActive());
	AddToInactiveGroup(pGameObject);
}

void GameObjectManager::AddToActiveGroup(GameObject* pGameObject)
{
	m_activeGroup.push_back(pGameObject);
	pGameObject->m_groupIndex = static_cast<uint32_t>(m_activeGroup.size() - 1);

	pGameObject->OnFlag(GAMEOBJECT_FLAG::ACTIVE);
}

void GameObjectManager::AddToInactiveGroup(GameObject* pGameObject)
{
	m_inactiveGroup.push_back(pGameObject);
	pGameObject->m_groupIndex = static_cast<uint32_t>(m_inactiveGroup.size() - 1);

	pGameObject->OffFlag(GAMEOBJECT_FLAG::ACTIVE);
}

void GameObjectManager::RemoveFromGroup(GameObject* pGameObject)
{
	auto& group = pGameObject->IsActive() ? m_activeGroup : m_inactiveGroup;

	uint32_t groupSize = static_cast<uint32_t>(group.size());
	assert(groupSize > 0);

	const uint32_t groupIndex = pGameObject->m_groupIndex;
	const uint32_t lastIndex = groupSize - 1;

	assert(groupIndex < groupSize);
	// assert(groupIndex != std::numeric_limits<uint32_t>::max());
	assert(group[groupIndex] == pGameObject);	// 醞蹂!

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

void GameObjectManager::RemoveDestroyedGameObjects()
{
	// だ惚脹 啪歜螃粽薛お 薯剪 紫醞 綠翕晦 壁 煎萄 婁薑縑憮曖 啪歜螃粽薛お 儅撩戲煎 檣п m_table曖 營й渡檜 橾橫陳 熱 氈戲嘎煎
	// 塊 �僱磍� 奩萄衛 в蹂ж棻.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (GameObject* pGameObject : m_destroyed)
	{
		/* 匐隸 */
		assert(pGameObject->IsPending() == false);			// 煎註 壁 模樓曖 螃粽薛お朝 だ惚腆 熱 橈棻.
		assert(pGameObject->IsOnTheDestroyQueue() == true);	// だ惚 聽縑 菟橫螞 唳辦縑朝 檜 ON_DESTROY_QUEUE Ы楚斜陛 麵螳 氈橫撿虜 и棻.

		// Step 1. Transform 濠衝 睡賅 翱唸 薯剪
		Transform* pTransform = &pGameObject->m_transform;
		Transform* pParentTransform = pTransform->m_pParentTransform;
		if (pParentTransform != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			bool find = false;
#endif
			std::vector<Transform*>::const_iterator end = pParentTransform->m_children.cend();
			std::vector<Transform*>::const_iterator iter = pParentTransform->m_children.cbegin();
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

		for (Transform* pChildTransform : pTransform->m_children)
		{
			assert(pChildTransform->m_pParentTransform == pTransform);	// 濠褐婁曖 翱唸 �挫�


			// 醞蹂 ん檣お
			pChildTransform->m_pParentTransform = nullptr;	// 壽縑憮 埠 delete腎朝 濠褐擊 蕾斬(Step 1縑憮 蕾斬л)ж朝 匙擊 寞雖 (濡營瞳 湮旋葭 ん檣攪 薯剪)
			// 檜煎 檣п憮 偌羹 だ惚 衛縑朝 瑞お 螃粽薛お艘朝雖 っ滌(RootGroup縑憮 ん檣攪 薯剪п撿 л)擊 й 陽 睡賅 Transform ん檣攪煎
			// っ欽й 熱 橈棻. 評塭憮 霞瞼 瑞お 喻萄艘朝雖蒂 羹觼п場晦 嬪п UIOBJECT_FLAG翮剪⑽戲煎 REAL_ROOT塭朝 Ы楚斜蒂 餌辨и棻.
		}
		// pTransform->m_children.clear();	// 偌羹 delete衛 濠翕 模資

		this->RemoveFromGroup(pGameObject);

		// с菟 纔檜綰縑憮 薯剪
		assert(m_handleTable[pGameObject->m_tableIndex] == pGameObject);
		m_handleTable[pGameObject->m_tableIndex] = nullptr;

		// for debugging...
		pGameObject->m_groupIndex = std::numeric_limits<uint32_t>::max();
		pGameObject->m_tableIndex = std::numeric_limits<uint32_t>::max();

		delete pGameObject;
	}

	m_destroyed.clear();
}

void GameObjectManager::SetActive(GameObject* pGameObject, bool active)
{
	// 檜嘐 п渡 �側� 鼻鷓陛 撲薑腎橫 氈朝 唳辦 л熱 葬欐
	if (pGameObject->IsActive() == active)
		return;
	
	if (active)
	{
		for (IComponent* pComponent : pGameObject->m_components)
			pComponent->Enable();

		// PENDING 鼻鷓陛 嬴棋 唳辦縑虜 ん檣攪 檜翕 (PENDING 鼻鷓縑憮朝 Active/Inactive 漱攪縑 ん檣攪陛 襄營ж雖 彊朝棻.)
		if (!pGameObject->IsPending())
			this->MoveToActiveGroup(pGameObject);
	}
	else
	{
		for (IComponent* pComponent : pGameObject->m_components)
			pComponent->Disable();

		// PENDING 鼻鷓陛 嬴棋 唳辦縑虜 ん檣攪 檜翕 (PENDING 鼻鷓縑憮朝 Active/Inactive 漱攪縑 ん檣攪陛 襄營ж雖 彊朝棻.)
		if (!pGameObject->IsPending())
			this->MoveToInactiveGroup(pGameObject);
	}
}

bool GameObjectManager::SetParent(Transform* pTransform, Transform* pNewParentTransform)
{
	GameObject* pGameObject = pTransform->m_pGameObject;
	Transform* pOldParentTransform = pTransform->m_pParentTransform;

	if (pGameObject->IsOnTheDestroyQueue())
		return false;

	// 濠晦 濠褐擊 睡賅煎 撲薑ж溥堅 ж剪釭 檜嘐 撲薑ж溥朝 睡賅陛 檜嘐 睡賅檣 唳辦縑朝 褒ぬ
	if (pTransform == pNewParentTransform || pOldParentTransform == pNewParentTransform)
		return false;

	// 餌檜贗 匐餌
	// pTransform檜 檜嘐 釭蒂 褻鼻戲煎 ж堅 氈朝 唳辦 or だ惚 蕨薑檣 螃粽薛お蒂 睡賅煎 撲薑ж溥朝 唳辦
	// 蕨諼籀葬 п輿雖 彊戲賊 偌羹曖 Destroy л熱縑憮 濠衝 螃粽薛お菟梱雖 賅舒 Destroy 聽縑 厥橫輿朝 薑疇婁 橾婦撩檜 蜃雖 彊朝棻.
	// (睡賅朝 だ惚腎朝等 濠衝擎 だ惚腎雖 彊朝 賅牖 嫦儅)
	if (pNewParentTransform != nullptr)
		if (pNewParentTransform->IsDescendantOf(pTransform) || pNewParentTransform->m_pGameObject->IsOnTheDestroyQueue())
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
		std::vector<Transform*>::const_iterator end = pOldParentTransform->m_children.cend();
		std::vector<Transform*>::const_iterator iter = pOldParentTransform->m_children.cbegin();
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

	return true;
}
