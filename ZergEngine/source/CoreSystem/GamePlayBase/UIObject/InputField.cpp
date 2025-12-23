#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

constexpr float INPUT_FIELD_DEFAULT_WIDTH = 200.0f;
constexpr float INPUT_FIELD_DEFAULT_HEIGHT = 40.0f;
constexpr float ROUNDED_INPUT_FIELD_DEFAULT_RADIUS = 15.0f;

InputField::InputField(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_size(INPUT_FIELD_DEFAULT_WIDTH, INPUT_FIELD_DEFAULT_HEIGHT)
	, m_bkColor(ColorsLinear::White)
	, m_text()
	, m_textColor(ColorsLinear::Black)
	, m_radius(ROUNDED_INPUT_FIELD_DEFAULT_RADIUS, ROUNDED_INPUT_FIELD_DEFAULT_RADIUS)
	, m_iff(IFF_SPACE)	// 공백 허용
	, m_shape(InputFieldShape::Rectangle)
{
	m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);					// 비가상함수이므로 생성자에서 호출해도 ok
	m_text.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);			// 비가상함수이므로 생성자에서 호출해도 ok
}

void InputField::AllowReturn(bool b)
{
	if (b)
		m_iff = m_iff | IFF_RETURN;
	else
		m_iff = m_iff & (~IFF_RETURN);
}

void InputField::SetPassword(bool b)
{
	if (b)
		m_iff = m_iff | IFF_PASSWORD;
	else
		m_iff = m_iff & (~IFF_PASSWORD);
}

void InputField::AllowSpace(bool b)
{
	if (b)
		m_iff = m_iff | IFF_SPACE;
	else
		m_iff = m_iff & (~IFF_SPACE);
}

void InputField::SetDigitOnly(bool b)
{
	if (b)
		m_iff = m_iff | IFF_DIGIT;
	else
		m_iff = m_iff & (~IFF_DIGIT);
}

bool InputField::HitTest(POINT pt) const
{
	POINT wcp;
	m_transform.GetWinCoordPosition(&wcp);

	return m_size.HitTest(pt, wcp);
}

void InputField::OnChar(TCHAR ch)
{
	constexpr uint32_t UNKNOWN = (std::numeric_limits<uint32_t>::max)();
	constexpr uint32_t RETURN = 0;
	constexpr uint32_t BACKSPACE = 1;
	constexpr uint32_t SPACE = 2;
	constexpr uint32_t ALPHA = 3;
	constexpr uint32_t DIGIT = 4;
	constexpr uint32_t PUNCT = 5;

	uint32_t ct = UNKNOWN;		// character type

	if (ch == VK_RETURN)
		ct = RETURN;
	else if (ch == VK_BACK)
		ct = BACKSPACE;
	else if (ch == VK_SPACE)
		ct = SPACE;
	else if (_istalpha(ch))
		ct = ALPHA;
	else if (_istdigit(ch))
		ct = DIGIT;
	else if (_istpunct(ch))
		ct = PUNCT;

	std::wstring& text = m_text.GetText();

	switch (ct)
	{
	case RETURN:
		if (this->IsReturnAllowed())
			text.push_back(ch);
		break;
	case BACKSPACE:
		if (text.length() > 0)
			text.pop_back();
		break;
	case SPACE:
		if (this->IsSpaceAllowed())
			text.push_back(ch);
		break;
	case ALPHA:
		if (!this->IsDigitOnly())
			text.push_back(ch);
		break;
	case DIGIT:
		text.push_back(ch);
		break;
	case PUNCT:
		text.push_back(ch);
		break;
	default:
		break;
	}
}

void InputField::OnLButtonClick(POINT pt)
{
	// 텍스트 입력을 받게 등록 요청
	UIObjectManager::GetInstance()->SetFocusedUI(this);
}

