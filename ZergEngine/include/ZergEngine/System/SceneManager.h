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
		// 씬 교체 시 해야할 일
		// 1. 현재 씬의 모든 GameObject들을 DestroyAllComponents를 호출해준 후 제거한다. (DontDestroyOnLoad 객체는 패스한다.)
		// 2. 남아있는 GameObject들을 새로운 씬으로 옮기면서 m_pScene 포인터도 교체해준다.
		// 3. 현재 씬에 남아있는 컴포넌트들을 새 씬으로 옮긴다.
		// 3. 스크립트들 실행한다. (Awake같은것들)
		// 4. 런타임 루프 반복.
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
