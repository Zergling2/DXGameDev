// TransformEdit.cpp : implementation file
//

#include "FloatEdit.h"

// CFloatEdit

IMPLEMENT_DYNAMIC(CFloatEdit, CEdit)

CFloatEdit::CFloatEdit()
{
}

CFloatEdit::~CFloatEdit()
{
}


BEGIN_MESSAGE_MAP(CFloatEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()


// CFloatEdit message handlers

void CFloatEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
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
