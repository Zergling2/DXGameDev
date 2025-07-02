#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	class ISizeUIObject : public IUIObject
	{
		friend class Renderer;
	public:
		ISizeUIObject(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name, UIOBJECT_TYPE type);
		virtual ~ISizeUIObject() = default;

		XMVECTOR XM_CALLCONV GetSize() const { return XMLoadFloat2(&m_size); }
		void XM_CALLCONV SetSize(FXMVECTOR size);
		void SetSize(const XMFLOAT2& size) { this->SetSize(XMLoadFloat2(&size)); }
		void SetSize(const XMFLOAT2A& size) { this->SetSize(XMLoadFloat2A(&size)); }
	private:
		virtual bool XM_CALLCONV HitTest(FXMVECTOR mousePosition) const override;
	protected:
		XMFLOAT2 m_size;
		XMFLOAT2 m_halfSize;
	};
}
