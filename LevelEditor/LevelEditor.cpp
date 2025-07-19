// LevelEditor.cpp : Defines the class behaviors for the application.
//

#include "afxwinappex.h"
#include "afxdialogex.h"
#include "LevelEditor.h"
#include "MainFrm.h"

#include "LevelEditorDoc.h"
#include "LevelEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLevelEditorApp

// The one and only CLevelEditorApp object

CLevelEditorApp theApp;


BEGIN_MESSAGE_MAP(CLevelEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CLevelEditorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CLevelEditorApp construction

CLevelEditorApp::CLevelEditorApp() noexcept
	: m_hEditorCameraObject()
{

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("LevelEditor.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// CLevelEditorApp initialization

BOOL CLevelEditorApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	try
	{
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CLevelEditorDoc),
			RUNTIME_CLASS(CMainFrame),       // main SDI frame window
			RUNTIME_CLASS(CLevelEditorView)
		);
	}
	catch (const std::bad_alloc& e)
	{
		UNREFERENCED_PARAMETER(e);
		return FALSE;
	}
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();




	ze::Runtime::CreateInstance();
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	ze::Runtime::GetInstance()->InitEditor(
		AfxGetApp()->m_hInstance,
		pMainFrame->m_hWnd,
		pMainFrame->GetLevelEditorView()->m_hWnd,
		800, 600
	);
	
	// 메인 카메라용 게임오브젝트 생성
	m_hEditorCameraObject = ze::Runtime::GetInstance()->CreateGameObject();
	ze::GameObject* pEditorCameraObject = m_hEditorCameraObject.ToPtr();
	ze::ComponentHandle<ze::Camera> hCamera = pEditorCameraObject->AddComponent<ze::Camera>();
	ze::Camera* pCamera = hCamera.ToPtr();
	pCamera->SetBackgroundColor(DirectX::Colors::ForestGreen);

	ze::Texture2D skyboxCubeMap = ze::ResourceLoader::GetInstance()->LoadCubeMapTexture(L"Resource\\Skybox\\cloudy_puresky.dds");
	ze::Environment::GetInstance()->SetSkyboxCubeMap(skyboxCubeMap);

	return TRUE;
}

int CLevelEditorApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	ze::Runtime::GetInstance()->DestroyAllObject();
	ze::Runtime::GetInstance()->RemoveDestroyedComponentsAndObjects();

	ze::Runtime::GetInstance()->UnInit();
	ze::Runtime::GetInstance()->DestroyInstance();



	return CWinApp::ExitInstance();
}

// App command to run the dialog
void CLevelEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CLevelEditorApp::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_pMainWnd)
		return TRUE;

	if (m_pMainWnd->IsIconic())
		return TRUE;

	ze::Runtime::GetInstance()->OnIdle();

	return TRUE;
}
