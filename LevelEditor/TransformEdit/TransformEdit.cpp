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

        // Backspace Ű�� ���ڴ� �׻� ���
        if (nChar == VK_BACK || _istdigit(ch))
        {
            pass = true;
            break;
        }

        // ��ȣ �Է� (+, -)
        if (ch == _T('+') || ch == _T('-'))
        {
            // Ŀ�� ��ġ�� 0�� �ƴϰų� �̹� ��ȣ�� ���� ���
            if (selStart != 0 || str.FindOneOf(_T("+-")) != -1)
                pass = false;
            else
                pass = true;

            break;
        }

        // �Ҽ��� �Է�
        if (ch == _T('.'))
        {
            // �̹� .�� ������ �߰� �Է� ����
            if (str.Find('.') != -1)
            {
                pass = false;
                break;
            }

            // ���ڰ� ���� ���¿��� .�� �Է��Ϸ��� ���: 0. ���� �ڵ� ��ü
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

    // �Ҽ��� ǥ�� ����ȭ
    TCHAR str[32];
    this->GetWindowTextW(str, _countof(str));
    float val = _tcstof(str, nullptr);

    // ����Ʈ ��Ʈ�ѿ� ����ȭ�� �� ����
    CString result;
    result.Format(_T("%lf"), val);
    this->SetWindowTextW(result.GetString());
    
    // ���� ��ü�� ������Ʈ
    CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
    CFormView* pFormView = pMainFrame->GetComponentInspectorFormView();
    BOOL isInspectorValid = pFormView->IsKindOf(RUNTIME_CLASS(CTransformInspectorFormView));
    assert(isInspectorValid);
    if (isInspectorValid)
    {
        CTransformInspectorFormView* pInspector = static_cast<CTransformInspectorFormView*>(pFormView);

        // 1. ��Ʈ�� -> ������ ������Ʈ
        pInspector->UpdateData(TRUE);

        // 2. ������Ʈ�� ������ Transform�� ����
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
