#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Checkbox.h>

using namespace ze;

constexpr float DEFAULT_CHECKBOX_SIZE = 14.0f;
constexpr float DEFAULT_TEXTBOX_WIDTH = 80.0f;
constexpr float DEFAULT_TEXTBOX_HEIGHT = 14.0f;

Checkbox::Checkbox(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_isChecked(false)
	, m_leftText(false)	// 텍스트박스의 기본 위치는 오른쪽
	, m_checkboxSize(DEFAULT_CHECKBOX_SIZE, DEFAULT_CHECKBOX_SIZE)
	, m_boxColor(ColorsLinear::OliveDrab)
	, m_checkColor(ColorsLinear::Gold)
	, m_textboxSize(DEFAULT_TEXTBOX_WIDTH, DEFAULT_TEXTBOX_HEIGHT)
	, m_textColor(ColorsLinear::WhiteSmoke)
	, m_text()
	, m_handlerOnClick()
{
	m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_text.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void Checkbox::SetCheck(bool b)
{
	// 이미 요청된 체크 상태가 설정되어 있는경우
	if (b == this->GetCheck())
		return;

	// 상태 업데이트
	m_isChecked = b;

	// 지연 객체(예시: OnLoadScene)인 경우 콜백 호출 방지
	if (this->IsPending())
		return;

	// UI Event Callback
	if (m_handlerOnClick)
	{
		bool success = m_handlerOnClick();
		if (!success)	// 객체가 파괴된 경우
			m_handlerOnClick = nullptr;
	}
}

void Checkbox::SetLeftText(bool b)
{
	if (m_leftText == b)
		return;

	m_leftText = b;

	if (m_leftText)
		m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	else
		m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
}

bool Checkbox::HitTest(POINT pt) const
{
	POINT wcp;
	m_transform.GetWinCoordPosition(&wcp);

	return m_checkboxSize.HitTest(pt, wcp);
}

void Checkbox::OnLButtonClick(POINT pt)
{
	this->SetCheck(!this->GetCheck());
}
