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
