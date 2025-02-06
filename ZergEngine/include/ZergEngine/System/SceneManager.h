#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class IScene;

	class AsyncOperation
	{
	private:
		AsyncOperation(IScene* pScene)
			: m_thread()
			, m_pScene(pScene)
		{
		}
		~AsyncOperation();
		void AllowSceneActivation();
	private:
		void SetProgress(float p);
		void SetDone();
	private:
		std::thread m_thread;
		IScene* m_pScene;
		float m_progress;
		bool m_done;
		bool m_allowSceneActivation;
	};

	class SceneManager
	{
		friend class Engine;
	private:
		static sysresult SysStartup(const EngineSystemInitDescriptor& desc);
		static void SysCleanup();
	public:
		// �� ��ü �� �ؾ��� ��
		// 1. ���� ���� ��� GameObject���� DestroyAllComponents�� ȣ������ �� �����Ѵ�. (DontDestroyOnLoad ��ü�� �н��Ѵ�.)
		// 2. �����ִ� GameObject���� ���ο� ������ �ű�鼭 m_pScene �����͵� ��ü���ش�.
		// 3. ���� ���� �����ִ� ������Ʈ���� �� ������ �ű��.
		// 3. ��ũ��Ʈ�� �����Ѵ�. (Awake�����͵�)
		// 4. ��Ÿ�� ���� �ݺ�.
		static sceneId GetCurrentSceneId();
		static IScene* GetCurrentScene() { return SceneManager::s_pCurrentScene; }
		static bool LoadScene(const wchar_t* sceneName);
		static std::shared_ptr<AsyncOperation> LoadSceneAsync(const wchar_t* sceneName);
	private:
		static bool HandleSceneChange();
		static IScene* CreateScene(const wchar_t* sceneName);
	private:
		static const SceneTableMap* s_pStm;
		static sceneId s_sceneIdForIssue;
		static IScene* s_pCurrentScene;
		static IScene* s_pNextScene;
		static SRWLOCK s_lock;
	};
}
