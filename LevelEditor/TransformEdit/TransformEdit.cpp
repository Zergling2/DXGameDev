// TransformEdit.cpp : implementation file
//

#include "TransformEdit.h"

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
    CString str;
    this->GetWindowText(str);
    float val = _tcstof(str.GetString(), nullptr);

    CString result;
    result.Format(_T("%lf"), val);

    this->SetWindowTextW(result.GetString());
}
