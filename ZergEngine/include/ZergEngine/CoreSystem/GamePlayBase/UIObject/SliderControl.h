#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	enum class SliderControlType : uint8_t
	{
		Horizontal,
		Vertical
	};

	class SliderControl : public IUIObject
	{
		friend class Renderer;
	public:
		SliderControl(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name);
		virtual ~SliderControl() = default;

		virtual UIObjectType GetType() const override { return UIObjectType::SliderControl; }

		void SetSliderControlType(SliderControlType type) { m_sliderType = type; }
		SliderControlType GetSliderControlType() const { return m_sliderType; }

		// 전달된 참조 인자로 범위의 최소, 최대값이 전달됩니다.
		void GetRange(int32_t& min, int32_t& max) { min = m_rangeMin; max = m_rangeMax; }
		int32_t GetRangeMin() const { return m_rangeMin; }
		int32_t GetRangeMax() const { return m_rangeMax; }
		// max가 min보다 작을 경우 함수는 실패 후 false를 반환합니다.
		bool SetRange(int32_t min, int32_t max);
		// 설정된 범위 최대값보다 크거나 같은 값이 전달될 경우 함수는 실패 후 false를 반환합니다.
		bool SetRangeMin(int32_t min);
		// 설정된 범위 최솟값보다 작거나 같은 값이 전달될 경우 함수는 실패 후 false를 반환합니다.
		bool SetRangeMax(int32_t max);

		FLOAT GetTrackLength() const { return m_trackLength; }
		void SetTrackLength(FLOAT length) { m_trackLength = length; }

		FLOAT GetTrackThickness() const { return m_trackThickness; }
		void SetTrackThickness(FLOAT thickness) { m_trackThickness = thickness; }

		// Thumb의 현재 위치값을 반환합니다.
		int32_t GetThumbPos() const { return m_thumbPos; }
		// Thumb의 현재 위치값을 설정합니다. 현재 범위를 벗어난 경우 클램프된 값이 설정됩니다.
		void SetThumbPos(int32_t pos);

		const XMFLOAT2& GetThumbSize() const { return m_thumbSize.GetSize(); }
		FLOAT GetThumbSizeX() const { return m_thumbSize.GetSizeX(); }
		FLOAT GetThumbSizeY() const { return m_thumbSize.GetSizeY(); }
		FLOAT GetThumbWidth() const { return GetThumbSizeX(); }
		FLOAT GetThumbHeight() const { return GetThumbSizeY(); }
		FLOAT GetThumbHalfSizeX() const { return m_thumbSize.GetHalfSizeX(); }
		FLOAT GetThumbHalfSizeY() const { return m_thumbSize.GetHalfSizeY(); }
		FLOAT GetThumbHalfWidth() const { return GetThumbHalfSizeX(); }
		FLOAT GetThumbHalfHeight() const { return GetThumbHalfSizeY(); }
		void SetThumbSize(FLOAT width, FLOAT height) { m_thumbSize.SetSize(width, height); }
		void SetThumbSize(const XMFLOAT2& size) { m_thumbSize.SetSize(size); }
		void XM_CALLCONV SetThumbSize(FXMVECTOR size) { m_thumbSize.SetSize(size); }

		XMVECTOR GetTrackColorVector() const { return m_trackColor.GetColorVector(); }
		const XMFLOAT4& GetTrackColor() const { return m_trackColor.GetColor(); }
		void XM_CALLCONV SetTrackColor(FXMVECTOR color) { m_trackColor.SetColor(color); }
		void SetTrackColor(const XMFLOAT4& color) { m_trackColor.SetColor(color); }
		void SetTrackColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_trackColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetTrackColorRGB(FXMVECTOR rgb) { m_trackColor.SetColorRGB(rgb); }
		void SetTrackColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_trackColor.SetColorRGB(r, g, b); }
		void SetTrackColorA(FLOAT a) { m_trackColor.SetColorA(a); }

		XMVECTOR GetThumbColorVector() const { return m_thumbColor.GetColorVector(); }
		const XMFLOAT4& GetThumbColor() const { return m_thumbColor.GetColor(); }
		void XM_CALLCONV SetThumbColor(FXMVECTOR color) { m_thumbColor.SetColor(color); }
		void SetThumbColor(const XMFLOAT4& color) { m_thumbColor.SetColor(color); }
		void SetThumbColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) { m_thumbColor.SetColor(r, g, b, a); }
		void XM_CALLCONV SetThumbColorRGB(FXMVECTOR rgb) { m_thumbColor.SetColorRGB(rgb); }
		void SetThumbColorRGB(FLOAT r, FLOAT g, FLOAT b) { m_thumbColor.SetColorRGB(r, g, b); }
		void SetThumbColorA(FLOAT a) { m_thumbColor.SetColorA(a); }

		void SetHandlerOnPosChange(std::function<bool()> handler) { m_handlerOnPosChange = std::move(handler); }

		virtual bool HitTest(POINT pt) const override;

		// Thumb의 Min -> Max 방향으로의 물리 거리를 계산합니다.
		FLOAT ComputeDistFromMinToThumb() const;
		// Windows 화면좌표계에서의 Thumb 오프셋을 계산합니다.
		void ComputeWinCoordThumbOffset(SIZE* pOut) const;
		// 동차 클립 공간 좌표계에서의 Thumb의 오프셋을 계산합니다.
		void ComputeHCSCoordThumbOffset(XMFLOAT2* pOut) const;
	private:
		virtual void OnMouseMove(POINT pt) override;

		FLOAT GetTrackLogicalLength() const { return m_trackLogicalLength; }
		void UpdateTrackLogicalLength() { m_trackLogicalLength = static_cast<FLOAT>(m_rangeMax - m_rangeMin); }
	private:
		SliderControlType m_sliderType;
		int32_t m_rangeMin;
		int32_t m_rangeMax;
		FLOAT m_trackLogicalLength;	// m_rangeMax - m_rangeMin cache
		FLOAT m_trackLength;
		FLOAT m_trackThickness;
		int32_t m_thumbPos;
		UISize m_thumbSize;
		UIColor m_trackColor;
		UIColor m_thumbColor;

		std::function<bool()> m_handlerOnPosChange;
	};
}
