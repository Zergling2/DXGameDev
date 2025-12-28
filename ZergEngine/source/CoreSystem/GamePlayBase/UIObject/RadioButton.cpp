#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\RadioButton.h>

using namespace ze;

constexpr float DEFAULT_RADIOBUTTON_RADIUS = 7.5f;
constexpr float DEFAULT_TEXTBOX_WIDTH = 80.0f;
constexpr float DEFAULT_TEXTBOX_HEIGHT = 14.0f;

RadioButton::RadioButton(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_isChecked(false)
	, m_leftText(false)
	, m_radius(DEFAULT_RADIOBUTTON_RADIUS)
	, m_buttonColor(ColorsLinear::DarkOliveGreen)
	, m_dotColor(ColorsLinear::Gold)
	, m_textboxSize(DEFAULT_TEXTBOX_WIDTH, DEFAULT_TEXTBOX_HEIGHT)
	, m_textColor(ColorsLinear::WhiteSmoke)
	, m_text(L"Radio Button")
	, m_groupIndex(-1)
	, m_spGroup()
	, m_handlerOnClick()
{
	m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_text.SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void RadioButton::Join(RadioButton* pOther)
{
	// 예외 사항들 처리
	// pOther가 this인 경우 반환.
	// pOther가 이미 동일한 그룹에 속해있는 경우 반환.

	// pOther가 동일한 그룹 내에 있지 않은 경우 기존 pOther는 기존 그룹에서 제거되어 this 버튼의 그룹에 편입.

	// 자기 자신을 추가하는 경우 예외 처리
	if (this == pOther)
		return;

	// 이미 같은 그룹이었던 경우 (둘 다 nullptr이었을 수 있으므로)
	if (this->m_spGroup == pOther->m_spGroup)
	{
		if (this->m_spGroup != nullptr)
			return;
		else // 둘 다 nullptr이었던 경우 this에 그룹 생성 및 this를 그룹에 추가
		{
			this->m_spGroup = std::make_shared<RadioButtonGroup>();

			this->m_isChecked = false;
			this->m_spGroup->m_buttons.push_back(this);
			this->m_groupIndex = static_cast<int32_t>(this->m_spGroup->m_buttons.size() - 1);
		}
	}

	std::shared_ptr<RadioButtonGroup> spGroup;
	RadioButton* pAdd;
	if (this->m_spGroup != nullptr && pOther->m_spGroup == nullptr)
	{
		// pOther를 this의 그룹에 편입
		spGroup = this->m_spGroup;
		pAdd = pOther;
	}
	else if (this->m_spGroup == nullptr && pOther->m_spGroup != nullptr)
	{
		// this를 pOther의 그룹에 편입
		spGroup = pOther->m_spGroup;
		pAdd = this;
	}
	else // this->m_spGroup != nullptr && pOther->m_spGroup != nullptr
	{
		// pOther가 그룹을 나가서 this의 그룹에 참여
		pOther->LeaveGroup();
		spGroup = this->m_spGroup;
		pAdd = pOther;
	}

	assert(pAdd->m_spGroup == nullptr);
	assert(pAdd->m_spGroup != spGroup);

	pAdd->m_isChecked = false;
	spGroup->m_buttons.push_back(pAdd);
	pAdd->m_spGroup = spGroup;
	pAdd->m_groupIndex = static_cast<int32_t>(spGroup->m_buttons.size() - 1);
}

void RadioButton::SetCheck()
{
	const bool isChecked = this->GetCheck();
	if (isChecked)	// 이미 체크되어 있는 경우
		return;

	if (m_spGroup)
	{
		// 1. 일단 모두 false로 변경
		for (RadioButton* pItem : m_spGroup->m_buttons)
			pItem->m_isChecked = false;
	}

	assert(this->GetCheck() == false);	// 위의 동작으로 인해
	// 2. 마지막으로 this 체크 (상태 업데이트)
	m_isChecked = true;

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

void RadioButton::SetLeftText(bool b)
{
	if (m_leftText == b)
		return;

	m_leftText = b;

	if (m_leftText)
		m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	else
		m_text.SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
}

bool RadioButton::HitTest(POINT pt) const
{
	POINT wcp;
	m_transform.GetWinCoordPosition(&wcp);

	POINT ptL;	// pt in local space
	ptL.x = pt.x - wcp.x;
	ptL.y = pt.y - wcp.y;

	const LONG radius = static_cast<LONG>(this->GetRadius());
	
	return ptL.x * ptL.x + ptL.y * ptL.y <= radius * radius;
}

void RadioButton::OnLButtonClick(POINT pt)
{
	this->SetCheck();
}

void RadioButton::OnDestroy()
{
	// 그룹에서 자신의 포인터를 제거 및 그룹 참조 해제
	this->LeaveGroup();
}

void RadioButton::LeaveGroup()
{
	if (!m_spGroup)
		return;

	auto& groupItems = m_spGroup->m_buttons;
	// 그룹 내에서 자신을 찾아 제거
	int32_t index = 0;
	std::vector<RadioButton*>::const_iterator iter = groupItems.cbegin();

#if defined(DEBUG) || defined(_DEBUG)
	bool found = false;
#endif

	while (iter != groupItems.cend())
	{
		RadioButton* pCurrRadioButton = *iter;
		pCurrRadioButton->m_groupIndex = index;	// 그룹 인덱스 업데이트

		if (pCurrRadioButton == this)
		{
#if defined(DEBUG) || defined(_DEBUG)
			found = true;
#endif
			pCurrRadioButton->m_groupIndex = -1;
			pCurrRadioButton->m_isChecked = false;

			iter = groupItems.erase(iter);
			index -= 1;		// 제거로 인한 인덱스 한칸 이동 고려
		}
		else
		{
			++iter;
		}

		++index;
	}

	assert(found == true);

	
	// 그룹 참조 해제
	m_spGroup.reset();
}

RadioButton* RadioButtonGroup::GetButton(size_t index) const
{
	const size_t groupSize = this->GetGroupSize();
	if (index < groupSize)
		return m_buttons[index];
	else
		return nullptr;
}
