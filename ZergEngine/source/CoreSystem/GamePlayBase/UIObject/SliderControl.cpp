#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SliderControl.h>
#include <ZergEngine\CoreSystem\Math.h>

using namespace ze;

constexpr FLOAT THUMB_DEFAULT_WIDTH = 8.0;
constexpr FLOAT THUMB_DEFAULT_HEIGHT = 16.0;

SliderControl::SliderControl(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: IUIObject(id, flag, name)
	, m_sliderType(SliderControlType::Horizontal)
	, m_rangeMin(0)
	, m_rangeMax(100)
	, m_trackLength(100.0f)
	, m_trackThickness(4.0f)
	, m_thumbPos(m_rangeMin)
	, m_thumbSize(THUMB_DEFAULT_WIDTH, THUMB_DEFAULT_HEIGHT)
	, m_trackColor(ColorsLinear::DimGray)
	, m_thumbColor(ColorsLinear::DarkOliveGreen)
	, m_handlerOnPosChange()
{
	this->UpdateTrackLogicalLength();

	// const int32_t newThumbPos = Math::Clamp(this->GetPos(), m_rangeMin, m_rangeMax);
	// this->SetPos(newThumbPos);
}

bool SliderControl::SetRange(int32_t min, int32_t max)
{
	if (max <= min)
		return false;

	m_rangeMin = min;
	m_rangeMax = max;
	UpdateTrackLogicalLength();
	
	const int32_t newPos = Math::Clamp(this->GetThumbPos(), m_rangeMin, m_rangeMax);
	this->SetThumbPos(newPos);

	return true;
}

bool SliderControl::SetRangeMin(int32_t min)
{
	if (m_rangeMax <= min)
		return false;

	m_rangeMin = min;
	UpdateTrackLogicalLength();

	const int32_t newPos = Math::Clamp(this->GetThumbPos(), m_rangeMin, m_rangeMax);
	this->SetThumbPos(newPos);

	return true;
}

bool SliderControl::SetRangeMax(int32_t max)
{
	if (max <= m_rangeMin)
		return false;

	m_rangeMax = max;
	UpdateTrackLogicalLength();

	const int32_t newPos = Math::Clamp(this->GetThumbPos(), m_rangeMin, m_rangeMax);
	this->SetThumbPos(newPos);

	return true;
}

void SliderControl::SetThumbPos(int32_t pos)
{
	const int32_t oldThumbPos = this->GetThumbPos();
	const int32_t newThumbPos = Math::Clamp(pos, m_rangeMin, m_rangeMax);

	if (oldThumbPos == newThumbPos)
		return;	// OnUpdate 등의 함수 호출 차단

	m_thumbPos = newThumbPos;

	// 지연 객체(예시: OnLoadScene)인 경우 콜백 호출 방지
	if (this->IsPending())
		return;

	// UI Event Callback
	if (m_handlerOnPosChange)
	{
		bool success = m_handlerOnPosChange();
		if (!success)	// 객체가 파괴된 경우
			m_handlerOnPosChange = nullptr;
	}
}

bool SliderControl::HitTest(POINT pt) const
{
	POINT wcp;	// Windows coord pos
	m_transform.GetWinCoordPosition(&wcp);

	SIZE thumbOffset;
	this->ComputeWinCoordThumbOffset(&thumbOffset);

	POINT thumbWcp;
	thumbWcp.x = wcp.x + thumbOffset.cx;
	thumbWcp.y = wcp.y + thumbOffset.cy;

	return m_thumbSize.HitTest(pt, thumbWcp);
}

FLOAT SliderControl::ComputeDistFromMinToThumb() const
{
	const FLOAT distPerTick = this->GetTrackLength() / this->GetTrackLogicalLength();
	const int32_t ticks = this->GetThumbPos() - this->GetRangeMin();

	return static_cast<FLOAT>(ticks) * distPerTick;
}

void SliderControl::ComputeWinCoordThumbOffset(SIZE* pOut) const
{
	// Windows 화면좌표계에서의 Thumb 오프셋을 계산합니다.

	const FLOAT dist = ComputeDistFromMinToThumb();
	const FLOAT halfLength = this->GetTrackLength() * 0.5f;

	if (this->GetSliderControlType() == SliderControlType::Horizontal)
	{
		pOut->cx = static_cast<LONG>(-halfLength + dist);
		pOut->cy = 0;
	}
	else
	{
		// 수직 스크롤바의 경우 아래쪽이 Min, 위쪽이 Max에 해당.
		pOut->cx = 0;
		pOut->cy = static_cast<LONG>(+halfLength - dist);
	}
}

void SliderControl::ComputeHCSCoordThumbOffset(XMFLOAT2* pOut) const
{
	// Min에서 Max방향으로의 이동거리
	const FLOAT dist = ComputeDistFromMinToThumb();
	const FLOAT halfLength = this->GetTrackLength() * 0.5f;

	if (this->GetSliderControlType() == SliderControlType::Horizontal)
	{
		pOut->x = -halfLength + dist;
		pOut->y = 0.0f;
	}
	else
	{
		// 수직 스크롤바의 경우 아래쪽이 Min, 위쪽이 Max에 해당.
		pOut->x = 0.0f;
		pOut->y = -halfLength + dist;
	}
}

void SliderControl::OnMouseMove(POINT pt)
{
	const FLOAT ticksPerDist = this->GetTrackLogicalLength() / this->GetTrackLength();

	POINT wcp;
	m_transform.GetWinCoordPosition(&wcp);

	POINT ptL;
	ptL.x = pt.x - wcp.x;
	ptL.y = pt.y - wcp.y;

	FLOAT distFromSliderStart;

	if (m_sliderType == SliderControlType::Horizontal)
	{
		const FLOAT distFromSliderLeft = static_cast<FLOAT>(ptL.x) + this->GetTrackLength() * 0.5f;
		distFromSliderStart = distFromSliderLeft;
	}
	else
	{
		const FLOAT distFromSliderBottom = this->GetTrackLength() * 0.5f - static_cast<FLOAT>(ptL.y);
		distFromSliderStart = distFromSliderBottom;
	}

	const int32_t newThumbPos = this->GetRangeMin() + static_cast<int32_t>(std::lround(distFromSliderStart * ticksPerDist));
	this->SetThumbPos(newThumbPos);
}
