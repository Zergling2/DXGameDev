// TransformEdit.cpp : implementation file
//

#include "TransformEdit.h"
#include "..\MainFrm.h"
#include "..\TransformInspectorFormView.h"
#include "..\CLVItem\CLVItemTransform.h"

// CTransformEdit

IMPLEMENT_DYNAMIC(CTransformEdit, CEdit)

CTransformEdit::CTransformEdit()
{
}

CTransformEdit::~CTransformEdit()
{
}


BEGIN_MESSAGE_MAP(CTransformEdit, CEdit)
	ON_WM_CHAR()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


// CTransformEdit message handlers

void CTransformEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
    bool pass = false;

    do
    {
        CString str;
        this->GetWindowText(str);

        const int len = str.GetLength();
        int selStart, selEnd;
        this->GetSel(selStart, selEnd);

        const TCHAR ch = static_cast<TCHAR>(nChar);

        // Backspace 키와 숫자는 항상 허용
        if (nChar == VK_BACK || _istdigit(ch))
        {
            pass = true;
            break;
        }

        // 부호 입력 (+, -)
        if (ch == _T('+') || ch == _T('-'))
        {
            // 커서 위치가 0이 아니거나 이미 부호가 있을 경우
            if (selStart != 0 || str.FindOneOf(_T("+-")) != -1)
                pass = false;
            else
                pass = true;

            break;
        }

        // 소수점 입력
        if (ch == _T('.'))
        {
            // 이미 .이 있으면 추가 입력 금지
            if (str.Find('.') != -1)
            {
                pass = false;
                break;
            }

            // 숫자가 없는 상태에서 .만 입력하려는 경우: 0. 으로 자동 대체
            if (len == 0 || (len == 1 && (str[0] == '+' || str[0] == '-')))
            {
                ReplaceSel(_T("0."));
                return;
            }

            pass = true;
            break;
        }
    } while (false);

    if (pass)
        CEdit::OnChar(nChar, nRepCnt, nFlags);
    else
        MessageBeep(MB_ICONWARNING);
}


void CTransformEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

    // TODO: Add your message handler code here

    // 소수점 표현 정규화
    TCHAR str[32];
    this->GetWindowTextW(str, _countof(str));
    float val = _tcstof(str, nullptr);

    // 에디트 컨트롤에 정규화된 수 설정
    CString result;
    result.Format(_T("%lf"), val);
    this->SetWindowTextW(result.GetString());
    
    // 값을 객체로 업데이트
    CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
    CFormView* pFormView = pMainFrame->GetComponentInspectorFormView();
    BOOL isInspectorValid = pFormView->IsKindOf(RUNTIME_CLASS(CTransformInspectorFormView));
    assert(isInspectorValid);
    if (isInspectorValid)
    {
        CTransformInspectorFormView* pInspector = static_cast<CTransformInspectorFormView*>(pFormView);

        // 1. 컨트롤 -> 변수로 업데이트
        pInspector->UpdateData(TRUE);

        // 2. 업데이트된 변수롤 Transform에 세팅
        float val;

        CLVItemTransform* pItem = pInspector->GetCLVItemToModify();
        ze::Transform* pTransform = pItem->GetTransform();

        XMFLOAT3A pos;
        pInspector->m_positionX.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        pos.x = val;
        pInspector->m_positionY.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        pos.y = val;
        pInspector->m_positionZ.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        pos.z = val;
        pTransform->SetPosition(pos);

        XMFLOAT3A rot;
        pInspector->m_rotationX.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        rot.x = XMConvertToRadians(val);
        pInspector->m_rotationY.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        rot.y = XMConvertToRadians(val);
        pInspector->m_rotationZ.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        rot.z = XMConvertToRadians(val);
        pTransform->SetRotation(rot);

        XMFLOAT3A scale;
        pInspector->m_scaleX.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        scale.x = val;
        pInspector->m_scaleY.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        scale.y = val;
        pInspector->m_scaleZ.GetWindowText(str, _countof(str));
        val = _tcstof(str, nullptr);
        scale.z = val;
        pTransform->SetScale(scale);
    }
}
