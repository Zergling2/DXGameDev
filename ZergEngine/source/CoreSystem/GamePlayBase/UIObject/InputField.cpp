#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

constexpr float ROUNDED_INPUT_FIELD_DEFAULT_RADIUS = 15.0f;

InputField::InputField(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: Text(id, flag, name)
	, m_iff(IFF_SPACE)	// 공백 허용
	, m_shape(INPUT_FIELD_SHAPE::RECTANGLE)
	, m_radius(ROUNDED_INPUT_FIELD_DEFAULT_RADIUS, ROUNDED_INPUT_FIELD_DEFAULT_RADIUS)
	, m_bkColor(Colors::White)
{
	const uint32_t textSize = GetTextFormat().GetSize();

	this->SetSize(XMFLOAT2(200.0f, static_cast<float>(textSize) + 12.0f));	// 비가상함수이므로 생성자에서 호출해도 ok
	this->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);					// 비가상함수이므로 생성자에서 호출해도 ok
	this->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);			// 비가상함수이므로 생성자에서 호출해도 ok
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

void InputField::OnDetachedFromUIInteraction()
{
	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	if (pUIObjectManager->GetActiveInputField() == this)
		pUIObjectManager->SetActiveInputField(nullptr);

	Text::OnDetachedFromUIInteraction();
}

void InputField::OnLButtonDown()
{
	Text::OnLButtonDown();

	UIObjectManager* pUIObjectManager = UIObjectManager::GetInstance();

	pUIObjectManager->SetActiveInputField(nullptr);
}

void InputField::OnLButtonClick()
{
	// 글자 입력을 받게 등록 요청
	UIObjectManager::GetInstance()->SetActiveInputField(this);
}

void InputField::OnChar(TCHAR ch)
{
	constexpr uint32_t UNKNOWN		= 0xFFFFFFFF;
	constexpr uint32_t RETURN		= 0;
	constexpr uint32_t BACKSPACE	= 1;
	constexpr uint32_t SPACE		= 2;
	constexpr uint32_t ALPHA		= 3;
	constexpr uint32_t DIGIT		= 4;
	constexpr uint32_t PUNCT		= 5;

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
	else if (ch == _istpunct(ch))
		ct = PUNCT;


	switch (ct)
	{
	case RETURN:
		if (this->IsReturnAllowed())
			m_text.push_back(ch);
		break;
	case BACKSPACE:
		if (m_text.length() > 0)
			m_text.pop_back();
		break;
	case SPACE:
		if (this->IsSpaceAllowed())
			m_text.push_back(ch);
		break;
	case ALPHA:
		if (!this->IsDigitOnly())
			m_text.push_back(ch);
		break;
	case DIGIT:
		m_text.push_back(ch);
		break;
	case PUNCT:
		m_text.push_back(ch);
		break;
	default:
		break;
	}
}
