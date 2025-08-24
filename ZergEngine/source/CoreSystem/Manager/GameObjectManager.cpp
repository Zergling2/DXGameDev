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
	// ·±Å¸ÀÓ¿¡¼­ ¹Ì¸® ¸ðµÎ ÇØÁ¦½ÃÄ×¾î¾ß ÇÏ¹Ç·Î
	assert(m_activeGroup.size() == 0);
	assert(m_inactiveGroup.size() == 0);

	m_handleTable.clear();
}

void GameObjectManager::Deploy(GameObject* pGameObject)
{
	// Áß¿ä (ÇÃ·¡±× Á¦°Å)
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
	// Áö¿¬µÈ ¿ÀºêÁ§Æ®¸¦ Á¦°ÅÇÏ´Â °æ¿ì´Â OnLoadScene¿¡¼­ Destroy¸¦ ÇÑ´Ù´Â ÀÇ¹ÌÀÎµ¥ ÀÌ°ÍÀº Çã¿ëÇÏÁö ¾Ê´Â´Ù.
	if (pGameObject->IsPending())
		return;

	// ÀÚ½Ä ¿ÀºêÁ§Æ®±îÁö Destroy
	for (Transform* pChildTransform : pGameObject->m_transform.m_children)
		pChildTransform->m_pGameObject->Destroy();

	// ¼ÒÀ¯ÇÏ°í ÀÖ´Â ¸ðµç ÄÄÆ÷³ÍÆ® Á¦°Å
	for (IComponent* pComponent : pGameObject->m_components)
		pComponent->Destroy();	// ³»ºÎ¿¡¼­ °ð¹Ù·Î GameObject <-> Component°£ ¿¬°á ²÷À¸¸é ¾ÈµÊ (Áö±Ý ¼øÈ¸ÇÏ´Â ÀÌÅÍ·¹ÀÌÅÍ ¸Á°¡Áü!) (Áö¿¬»èÁ¦)

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
	
	// Å×ÀÌºí¿¡ µî·Ï
	// ºó ÀÚ¸® °Ë»ö
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

	// ¸¸¾à ºó ÀÚ¸®¸¦ Ã£Áö ¸øÇßÀ» °æ¿ì
	if (!find)
	{
		emptyIndex = tableSize;
		m_handleTable.push_back(nullptr);	// Å×ÀÌºí °ø°£ È®º¸
	}

	m_handleTable[emptyIndex] = pGameObject;
	pGameObject->m_tableIndex = emptyIndex;

	hGameObject = GameObjectHandle(emptyIndex, pGameObject->GetId());	// À¯È¿ÇÑ ÇÚµé ÁØºñ

	return hGameObject;
}

void GameObjectManager::AddToDestroyQueue(GameObject* pGameObject)
{
	if (pGameObject->IsOnTheDestroyQueue())	// ¸Å¿ì Áß¿ä! (Áßº¹ »ðÀÔÀ¸·Î ÀÎÇÑ Áßº¹ delete Èü ¼Õ»ó ¹æÁö)
		return;

	m_destroyed.push_back(pGameObject);
	pGameObject->OnFlag(GAMEOBJECT_FLAG::ON_DESTROY_QUEUE);
}

void GameObjectManager::MoveToActiveGroup(GameObject* pGameObject)
{
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// 1. Inactive group¿¡¼­ Á¦°Å
	assert(!pGameObject->IsActive());	// Inactive »óÅÂ¿´¾î¾ß ÇÑ´Ù.
	this->RemoveFromGroup(pGameObject);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬


	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// 2. Active group¿¡ Ãß°¡
	this->AddToActiveGroup(pGameObject);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
}

void GameObjectManager::MoveToInactiveGroup(GameObject* pGameObject)
{
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// 1. Active group¿¡¼­ Á¦°Å
	assert(pGameObject->IsActive());	// Active »óÅÂ¿´¾î¾ß ÇÑ´Ù.
	this->RemoveFromGroup(pGameObject);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬


	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
	// 2. Inactive group¿¡ Ãß°¡
	this->AddToInactiveGroup(pGameObject);
	// ¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬¦¬
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
	assert(group[groupIndex] == pGameObject);	// Áß¿ä!

	// Áö¿ì·Á´Â ¿ÀºêÁ§Æ® Æ÷ÀÎÅÍ°¡ ¸Ç µÚ¿¡ ÀÖ´Â°ÍÀÌ ¾Æ´Ñ°æ¿ì
	// ¸¶Áö¸·¿¡ À§Ä¡ÇÑ Æ÷ÀÎÅÍ¿Í À§Ä¡¸¦ ¹Ù²Û´Ù.
	if (groupIndex != lastIndex)
	{
		std::swap(group[groupIndex], group[lastIndex]);
		group[groupIndex]->m_groupIndex = groupIndex;	// ¸¶Áö¸·¿¡ ÀÖ´ø ¿ÀºêÁ§Æ®ÀÇ groupIndex¸¦ ¿Ã¹Ù¸£°Ô ¾÷µ¥ÀÌÆ® ÇØÁÖ¾î¾ß ÇÔ.
	}

	// ¼Ò¼ÓµÇ¾î ÀÖ´ø vector¿¡¼­ Á¦°Å
	group.pop_back();
}

void GameObjectManager::RemoveDestroyedGameObjects()
{
	// ÆÄ±«µÈ °ÔÀÓ¿ÀºêÁ§Æ® Á¦°Å µµÁß ºñµ¿±â ¾À ·Îµå °úÁ¤¿¡¼­ÀÇ °ÔÀÓ¿ÀºêÁ§Æ® »ý¼ºÀ¸·Î ÀÎÇØ m_tableÀÇ ÀçÇÒ´çÀÌ ÀÏ¾î³¯ ¼ö ÀÖÀ¸¹Ç·Î
	// ¶ô È¹µæÀÌ ¹Ýµå½Ã ÇÊ¿äÇÏ´Ù.
	AutoAcquireSlimRWLockExclusive autolock(m_lock);

	for (GameObject* pGameObject : m_destroyed)
	{
		/* °ËÁõ */
		assert(pGameObject->IsPending() == false);			// ·Îµù ¾À ¼Ò¼ÓÀÇ ¿ÀºêÁ§Æ®´Â ÆÄ±«µÉ ¼ö ¾ø´Ù.
		assert(pGameObject->IsOnTheDestroyQueue() == true);	// ÆÄ±« Å¥¿¡ µé¾î¿Â °æ¿ì¿¡´Â ÀÌ ON_DESTROY_QUEUE ÇÃ·¡±×°¡ ÄÑÁ® ÀÖ¾î¾ß¸¸ ÇÑ´Ù.

		// Step 1. Transform ÀÚ½Ä ºÎ¸ð ¿¬°á Á¦°Å
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
			assert(pChildTransform->m_pParentTransform == pTransform);	// ÀÚ½Å°úÀÇ ¿¬°á È®ÀÎ


			// Áß¿ä Æ÷ÀÎÆ®
			pChildTransform->m_pParentTransform = nullptr;	// ¹Ø¿¡¼­ °ð deleteµÇ´Â ÀÚ½ÅÀ» Á¢±Ù(Step 1¿¡¼­ Á¢±ÙÇÔ)ÇÏ´Â °ÍÀ» ¹æÁö (ÀáÀçÀû ´ó±Û¸µ Æ÷ÀÎÅÍ Á¦°Å)
			// ÀÌ·Î ÀÎÇØ¼­ °´Ã¼ ÆÄ±« ½Ã¿¡´Â ·çÆ® ¿ÀºêÁ§Æ®¿´´ÂÁö ÆÇº°(RootGroup¿¡¼­ Æ÷ÀÎÅÍ Á¦°ÅÇØ¾ß ÇÔ)À» ÇÒ ¶§ ºÎ¸ð Transform Æ÷ÀÎÅÍ·Î
			// ÆÇ´ÜÇÒ ¼ö ¾ø´Ù. µû¶ó¼­ ÁøÂ¥ ·çÆ® ³ëµå¿´´ÂÁö¸¦ Ã¼Å©ÇØ³õ±â À§ÇØ UIOBJECT_FLAG¿­°ÅÇüÀ¸·Î REAL_ROOT¶ó´Â ÇÃ·¡±×¸¦ »ç¿ëÇÑ´Ù.
		}
		// pTransform->m_children.clear();	// °´Ã¼ delete½Ã ÀÚµ¿ ¼Ò¸ê

		this->RemoveFromGroup(pGameObject);

		// ÇÚµé Å×ÀÌºí¿¡¼­ Á¦°Å
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
	// ÀÌ¹Ì ÇØ´ç È°¼º »óÅÂ°¡ ¼³Á¤µÇ¾î ÀÖ´Â °æ¿ì ÇÔ¼ö ¸®ÅÏ
	if (pGameObject->IsActive() == active)
		return;
	
	if (active)
	{
		for (IComponent* pComponent : pGameObject->m_components)
			pComponent->Enable();

		// PENDING »óÅÂ°¡ ¾Æ´Ñ °æ¿ì¿¡¸¸ Æ÷ÀÎÅÍ ÀÌµ¿ (PENDING »óÅÂ¿¡¼­´Â Active/Inactive º¤ÅÍ¿¡ Æ÷ÀÎÅÍ°¡ Á¸ÀçÇÏÁö ¾Ê´Â´Ù.)
		if (!pGameObject->IsPending())
			this->MoveToActiveGroup(pGameObject);
	}
	else
	{
		for (IComponent* pComponent : pGameObject->m_components)
			pComponent->Disable();

		// PENDING »óÅÂ°¡ ¾Æ´Ñ °æ¿ì¿¡¸¸ Æ÷ÀÎÅÍ ÀÌµ¿ (PENDING »óÅÂ¿¡¼­´Â Active/Inactive º¤ÅÍ¿¡ Æ÷ÀÎÅÍ°¡ Á¸ÀçÇÏÁö ¾Ê´Â´Ù.)
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

	// ÀÚ±â ÀÚ½ÅÀ» ºÎ¸ð·Î ¼³Á¤ÇÏ·Á°í ÇÏ°Å³ª ÀÌ¹Ì ¼³Á¤ÇÏ·Á´Â ºÎ¸ð°¡ ÀÌ¹Ì ºÎ¸ðÀÎ °æ¿ì¿¡´Â ½ÇÆÐ
	if (pTransform == pNewParentTransform || pOldParentTransform == pNewParentTransform)
		return false;

	// »çÀÌÅ¬ °Ë»ç
	// pTransformÀÌ ÀÌ¹Ì ³ª¸¦ Á¶»óÀ¸·Î ÇÏ°í ÀÖ´Â °æ¿ì or ÆÄ±« ¿¹Á¤ÀÎ ¿ÀºêÁ§Æ®¸¦ ºÎ¸ð·Î ¼³Á¤ÇÏ·Á´Â °æ¿ì
	// ¿¹¿ÜÃ³¸® ÇØÁÖÁö ¾ÊÀ¸¸é °´Ã¼ÀÇ Destroy ÇÔ¼ö¿¡¼­ ÀÚ½Ä ¿ÀºêÁ§Æ®µé±îÁö ¸ðµÎ Destroy Å¥¿¡ ³Ö¾îÁÖ´Â Á¤Ã¥°ú ÀÏ°ü¼ºÀÌ ¸ÂÁö ¾Ê´Â´Ù.
	// (ºÎ¸ð´Â ÆÄ±«µÇ´Âµ¥ ÀÚ½ÄÀº ÆÄ±«µÇÁö ¾Ê´Â ¸ð¼ø ¹ß»ý)
	if (pNewParentTransform != nullptr)
		if (pNewParentTransform->IsDescendantOf(pTransform) || pNewParentTransform->m_pGameObject->IsOnTheDestroyQueue())
			return false;

	// ·çÆ® ³ëµå Ã³¸® °æ¿ìÀÇ ¼ö
	// 1. pOldParentTransform != nullptr && pNewParentTransform == nullptr (±âÁ¸¿¡ ·çÆ® ³ëµå°¡ ¾Æ´Ï¾ú°í ÀÌÁ¦ ·çÆ® ³ëµå°¡ µÇ·Á´Â °æ¿ì)
	// 2. pOldParentTransform != nullptr && pNewParentTransform != nullptr (±âÁ¸¿¡ ·çÆ® ³ëµå°¡ ¾Æ´Ï¾ú°í Áö±Ýµµ ·çÆ® ³ëµå°¡ µÇ´Â °ÍÀº ¾Æ´Ñ °æ¿ì)
	// 3. pOldParentTransform == nullptr && pNewParentTransform == nullptr (±âÁ¸¿¡ ·çÆ® ³ëµå¿´°í Áö±Ýµµ ·çÆ® ³ëµå°¡ µÇ·Á´Â °æ¿ì) (ÀÌ °æ¿ì´Â ÇÔ¼ö ÁøÀÔ Á÷ÈÄ ¿¹¿ÜÃ³¸®µÊ)
	// 4. pOldParentTransform == nullptr && pNewParentTransform != nullptr (±âÁ¸¿¡ ·çÆ® ³ëµå¿´°í ÀÌÁ¦ ·çÆ® ³ëµå°¡ ¾Æ´Ï°Ô µÇ´Â °æ¿ì)

	// ºÎ¸ð°¡ nullptrÀÌ ¾Æ´Ï¾ú´ø °æ¿ì¿¡´Â ±âÁ¸ ºÎ¸ð¿¡¼­ ÀÚ½Ä Æ÷ÀÎÅÍ¸¦ Ã£¾Æ¼­ Á¦°Å
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
		assert(find == true);	// ÀÚ½ÄÀ¸·Î Á¸ÀçÇß¾ú¾î¾ß ÇÔ
	}

	// ºÎ¸ðÀÇ ÀÚ½Ä ¸ñ·ÏÀ» ¾÷µ¥ÀÌÆ®
	if (pNewParentTransform != nullptr)
		pNewParentTransform->m_children.push_back(pTransform);

	// ºÎ¸ð Æ÷ÀÎÅÍ¸¦ »õ·Î¿î ºÎ¸ð·Î ¾÷µ¥ÀÌÆ®
	pTransform->m_pParentTransform = pNewParentTransform;

	return true;
}
