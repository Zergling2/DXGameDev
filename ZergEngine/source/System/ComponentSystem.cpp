#include <ZergEngine\System\ComponentSystem.h>
#include <ZergEngine\System\Component\Camera.h>
#include <ZergEngine\System\Component\IScript.h>
#include <ZergEngine\System\Component\Light.h>
#include <ZergEngine\System\Component\MeshRenderer.h>
#include <ZergEngine\System\Graphics.h>
#include <ZergEngine\System\Debug.h>
#include <ZergEngine\System\GameObject.h>

using namespace zergengine;

static LPCWSTR const THIS_FILE_NAME = L"ComponentSystem.cpp";

bool ComponentSystem::CameraManager::s_updateRequired = false;
std::list<std::shared_ptr<Camera>> ComponentSystem::CameraManager::s_cameras;
std::vector<std::shared_ptr<IScript>> ComponentSystem::ScriptManager::s_scripts;
std::list<std::shared_ptr<MeshRenderer>> ComponentSystem::MeshRendererManager::s_meshRenderers;
std::vector<std::shared_ptr<DirectionalLight>> ComponentSystem::LightManager::s_dl;
std::vector<std::shared_ptr<PointLight>> ComponentSystem::LightManager::s_pl;
std::vector<std::shared_ptr<SpotLight>> ComponentSystem::LightManager::s_sl;

void ComponentSystem::CameraManager::Update()
{
    // 1. �ı��� ī�޶� �޸� ���� (shared_ptr ������ ���۷��� ����)
    const auto end = ComponentSystem::CameraManager::s_cameras.cend();
    auto iter = ComponentSystem::CameraManager::s_cameras.begin();

    while (iter != end)
    {
        if (iter->get()->IsDestroyed())
            iter = ComponentSystem::CameraManager::s_cameras.erase(iter);
        else
            ++iter;
    }

    // 2. Depth���� ������
    ComponentSystem::CameraManager::s_cameras.sort(
        [](const std::shared_ptr<Camera>& lhs, const std::shared_ptr<Camera>& rhs)
        {
            return lhs->GetDepth() < rhs->GetDepth();
        }
    );

    ComponentSystem::CameraManager::s_updateRequired = false;
}

void ComponentSystem::CameraManager::OnBeforeResize()
{
    for (auto& camera : ComponentSystem::CameraManager::s_cameras)
    {
        camera->m_rtvColorBuffer.Reset();
        camera->m_srvColorBuffer.Reset();
        camera->m_dsvDepthStencilBuffer.Reset();
    }
}

void ComponentSystem::CameraManager::OnResize()
{
    for (auto& camera : ComponentSystem::CameraManager::s_cameras)
    {
        camera->UpdateProjectionMatrix();
        camera->UpdateViewportInfo();
    }
}

void ComponentSystem::ScriptManager::FixedUpdateScripts()
{
    auto destroyedBegin = std::remove_if(
        ComponentSystem::ScriptManager::s_scripts.begin(),
        ComponentSystem::ScriptManager::s_scripts.end(),
        [](const std::shared_ptr<IScript>& script)
        {
            if (!script->IsDestroyed() && script->IsEnabled())
                script->FixedUpdate();
            return script->IsDestroyed();
        }
    );

    ComponentSystem::ScriptManager::s_scripts.erase(destroyedBegin, ComponentSystem::ScriptManager::s_scripts.cend());
}

void ComponentSystem::ScriptManager::UpdateScripts()
{
    auto destroyedBegin = std::remove_if(
        ComponentSystem::ScriptManager::s_scripts.begin(),
        ComponentSystem::ScriptManager::s_scripts.end(),
        [](const std::shared_ptr<IScript>& script)
        {
            if (!script->IsDestroyed() && script->IsEnabled())
                script->Update();
            return script->IsDestroyed();
        }
    );

    ComponentSystem::ScriptManager::s_scripts.erase(destroyedBegin, ComponentSystem::ScriptManager::s_scripts.cend());
}

void ComponentSystem::ScriptManager::LateUpdateScripts()
{
    auto destroyedBegin = std::remove_if(
        ComponentSystem::ScriptManager::s_scripts.begin(),
        ComponentSystem::ScriptManager::s_scripts.end(),
        [](const std::shared_ptr<IScript>& script)
        {
            if (!script->IsDestroyed() && script->IsEnabled())
                script->LateUpdate();
            return script->IsDestroyed();
        }
    );

    ComponentSystem::ScriptManager::s_scripts.erase(destroyedBegin, ComponentSystem::ScriptManager::s_scripts.cend());
}

void ComponentSystem::AddComponentToSystem(std::shared_ptr<IComponent> component)
{
    assert(component->IsDestroyed() == false);

    COMPONENT_TYPE ct = component->GetType();

    switch (ct)
    {
    case COMPONENT_TYPE::CAMERA:
        ComponentSystem::CameraManager::s_cameras.push_back(std::static_pointer_cast<Camera>(component));
        ComponentSystem::CameraManager::RequestUpdate();	// ���� �߰��Ǿ����Ƿ�
        break;
    case COMPONENT_TYPE::MESH_RENDERER:
        ComponentSystem::MeshRendererManager::s_meshRenderers.push_back(std::static_pointer_cast<MeshRenderer>(component));
        break;        
    case COMPONENT_TYPE::DIRECTIONAL_LIGHT:
        ComponentSystem::LightManager::s_dl.push_back(std::static_pointer_cast<DirectionalLight>(component));
        break;
    case COMPONENT_TYPE::POINT_LIGHT:
        ComponentSystem::LightManager::s_pl.push_back(std::static_pointer_cast<PointLight>(component));
        break;
    case COMPONENT_TYPE::SPOT_LIGHT:
        ComponentSystem::LightManager::s_sl.push_back(std::static_pointer_cast<SpotLight>(component));
        break;
    case COMPONENT_TYPE::SCRIPT:
        ComponentSystem::ScriptManager::s_scripts.push_back(std::static_pointer_cast<IScript>(component));
        break;
    case COMPONENT_TYPE::RIGID_BODY:
        break;
    case COMPONENT_TYPE::BOX_COLLIDER:
        break;
    case COMPONENT_TYPE::SPHERE_COLLIDER:
        break;
    case COMPONENT_TYPE::MESH_COLLIDER:
        break;
    default:
        break;
    }
}

void ComponentSystem::RemoveComponentFromSystemAndComponentList(std::shared_ptr<IComponent>& component)
{
    if (component->IsDestroyed())
        return;

    GameObject* pComponentOwner = component->GetGameObject();
    if (pComponentOwner)
    {
        auto& componentList = pComponentOwner->m_components;
        const auto end = componentList.cend();
        auto iter = componentList.cbegin();

        while (iter != end)
        {
            if (*iter == component)
            {
                componentList.erase(iter);
                return;
            }

            ++iter;
        }
    }

    component->m_pGameObject = nullptr;	// Unlink
    RemoveComponentFromSystemCommon(component);
}

void ComponentSystem::RemoveComponentFromSystemOnly(std::shared_ptr<IComponent>& component)
{
    // GameObject�� �ı��� �� ��� ������Ʈ���� ������ �� ���Ǵ� �Լ�
    // ���������� ����Ʈ�� clear�ϹǷ� ���� ����Ʈ�� ��ȸ�ؼ� ������Ʈ shared_ptr�� ������ �ʿ䰡 ����.

    if (component->IsDestroyed())
        return;

    component->m_pGameObject = nullptr;	// Unlink
    RemoveComponentFromSystemCommon(component);
}

void ComponentSystem::RemoveComponentFromSystemCommon(std::shared_ptr<IComponent>& component)
{
    COMPONENT_TYPE ct = component->GetType();

    switch (ct)
    {
    case COMPONENT_TYPE::CAMERA:
        // Deferred destroy!
        ComponentSystem::CameraManager::RequestUpdate();    // �������� ī�޶� ������Ʈ�� ������ ��ȸ�� �� ���������� ����.
        break;
    case COMPONENT_TYPE::MESH_RENDERER:
        // Deferred destroy!
        // �� ������ �������� ���� ���� �ڵ����� ����
        break;
    case COMPONENT_TYPE::DIRECTIONAL_LIGHT:
    {
        auto iter = ComponentSystem::LightManager::s_dl.cbegin();
        auto end = ComponentSystem::LightManager::s_dl.cend();
        while (iter != end)
        {
            if (iter->get() == component.get())
            {
                ComponentSystem::LightManager::s_dl.erase(iter);
                break;
            }
            ++iter;
        }
    }
        break;
    case COMPONENT_TYPE::POINT_LIGHT:
    {
        auto iter = ComponentSystem::LightManager::s_pl.cbegin();
        auto end = ComponentSystem::LightManager::s_pl.cend();
        while (iter != end)
        {
            if (iter->get() == component.get())
            {
                ComponentSystem::LightManager::s_pl.erase(iter);
                break;
            }
            ++iter;
        }
    }
        break;
    case COMPONENT_TYPE::SPOT_LIGHT:
    {
        auto iter = ComponentSystem::LightManager::s_sl.cbegin();
        auto end = ComponentSystem::LightManager::s_sl.cend();
        while (iter != end)
        {
            if (iter->get() == component.get())
            {
                ComponentSystem::LightManager::s_sl.erase(iter);
                break;
            }
            ++iter;
        }
    }
        break;
    case COMPONENT_TYPE::SCRIPT:
        // Deferred destroy!
        // �� ������ ������Ʈ ���� ���� �ڵ����� ����
        break;
    case COMPONENT_TYPE::RIGID_BODY:
        break;
    case COMPONENT_TYPE::BOX_COLLIDER:
        break;
    case COMPONENT_TYPE::SPHERE_COLLIDER:
        break;
    case COMPONENT_TYPE::MESH_COLLIDER:
        break;
    default:
        break;
    }
}
