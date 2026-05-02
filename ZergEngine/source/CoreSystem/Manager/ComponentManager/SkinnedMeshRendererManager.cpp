#include <ZergEngine\CoreSystem\Manager\ComponentManager\SkinnedMeshRendererManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\SkinnedMeshRenderer.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Resource\Animation.h>
#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <cassert>

using namespace ze;

SkinnedMeshRendererManager* SkinnedMeshRendererManager::s_pInstance = nullptr;

void SkinnedMeshRendererManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new SkinnedMeshRendererManager();
}

void SkinnedMeshRendererManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void SkinnedMeshRendererManager::UpdateAnimationTime()
{
    for (IComponent* pComponent : m_directAccessGroup)
    {
        if (!pComponent->IsEnabled() || !pComponent->m_pGameObject->IsActiveInHierarchy())
            continue;

        SkinnedMeshRenderer* pSkinnedMeshRenderer = static_cast<SkinnedMeshRenderer*>(pComponent);

        auto iter = pSkinnedMeshRenderer->m_currAnims.begin();
        while (iter != pSkinnedMeshRenderer->m_currAnims.cend())
        {
            const std::string& boneGroupName = iter->first;
            PlayingAnimation& pa = iter->second;
            assert(pa.m_pAnim);

            const float animDuration = pa.m_pAnim->GetDuration();
            float newTimeCursor = pa.m_timeCursor + Time::GetInstance()->GetDeltaTime() * pa.m_playbackSpeed;

            if (pa.m_loop)
            {
                newTimeCursor = Math::WrapFloat(newTimeCursor, animDuration);
                pa.m_timeCursor = newTimeCursor;

                ++iter;
            }
            else
            {
                if (0.0f <= newTimeCursor && newTimeCursor <= animDuration)
                {
                    pa.m_timeCursor = newTimeCursor;
                    ++iter;
                }
                else
                {
                    iter = pSkinnedMeshRenderer->m_currAnims.erase(iter);
                }
            }
        }
    }
}
