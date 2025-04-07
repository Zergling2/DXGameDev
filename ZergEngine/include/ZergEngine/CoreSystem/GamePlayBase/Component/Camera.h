#pragma once

#include <ZergEngine\Common\EngineHelper.h>
#include <ZergEngine\Common\EngineConstants.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{

	// �񵿱� �� �ε����� ī�޶� ������Ʈ ���� �� �����ؾ� �� ����
	// ���� �����忡�� â ũ�� �����ϴ� ������ �� ���� ���� �� �� ������ ũ�� ������ ���ͼ�
	// Depth/Stencil ���۳� ���� Ÿ���� ����� ��쿡 ũ�� ����ġ ����, �Ǵ� �� ���� Resize���� �������� ���� ũ����
	// ���� ������ ���� �� �ִ�. ���� �� ��� SRWLock ����ȭ�� �Ұ����ҵ�.
	// => �ϴ� �ذ�) ���� �� �並 �����ϴ� CreateBufferAndView() �Լ��� ���� ���� ���� �����忡�� ����ǰ� �Ѵ�.
	// ������ ���ҽ� ������ ���� �������� �Ͼ�Ƿ� ���۰� �������� ���� �����ӿ����� ������ ������ �߻�. ������ �񵿱� �ε��� ����/�� ������ ����ϸ�
	// ������ ����ȭ�� �ʿ������µ� ���� ���� ���� �����忡���� �׻� ������ ����ȭ�� �ʿ������Ƿ� �̰� �� ���Ƽ ����̴�. (���� �Ͼ�� ���� ������)
	class Camera : public IComponent
	{
		friend class CameraManagerImpl;
		friend class RendererImpl;
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::CAMERA;
		Camera();
		virtual ~Camera() = default;

		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::CAMERA; }
		inline const XMFLOAT4& GetBackgroundColor() const { return m_backgroundColor; }
		inline void SetBackgroundColor(const XMFLOAT4A color) { m_backgroundColor = color; }
		inline void SetBackgroundColor(FXMVECTOR color) { XMStoreFloat4A(&m_backgroundColor, color); }
		inline PROJECTION_METHOD GetProjectionMethod() const { return m_projMethod; }
		void SetProjectionMethod(PROJECTION_METHOD method);
		inline uint32_t GetFieldOfView() const { return m_fov; }
		void SetFieldOfView(uint8_t degree);
		inline const ClippingPlanes& GetClippingPlanes() const { return m_clippingPlanes; }
		void SetClippingPlanes(float nearZ, float farZ);

		// x, y, w, h must be normalized value.
		void SetViewportRect(float x, float y, float w, float h);
		inline const NormalizedViewportRect& GetViewportRect() const { return m_nzdVp; }

		inline int8_t GetDepth() const { return m_depth; }
		void SetDepth(int8_t depth);

		inline void SetMinimumDistanceStartTessellation(float distance) { m_tessMinDist = distance; }
		inline void SetMaximumDistanceEndTessellation(float distance) { m_tessMaxDist = distance; }

		// Only the range of 0.0 to 6.0 is allowed. Other ranges are ignored.
		bool SetMinimumTessellationExponent(float exponent);

		// Only the range of 0.0 to 6.0 is allowed. Other ranges are ignored.
		bool SetMaximumTessellationExponent(float exponent);

		inline float GetMinimumDistanceForTessellationToStart() const { return m_tessMinDist; }
		inline float GetMaximumDistanceForTessellationToStart() const { return m_tessMaxDist; }
		inline float GetMinimumTessellationExponent() const { return m_minTessExponent; }
		inline float GetMaximumTessellationExponent() const { return m_maxTessExponent; }

		inline ID3D11RenderTargetView* GetColorBufferRTVComInterface() const { return m_cpColorBufferRTV.Get(); }
		inline ID3D11ShaderResourceView* GetColorBufferSRVComInterface() const { return m_cpColorBufferSRV.Get(); }
		inline ID3D11DepthStencilView* GetDepthStencilBufferDSVComInterface() const { return m_cpDepthStencilBufferDSV.Get(); }

		inline const XMFLOAT4X4A& GetViewMatrix() const { return m_viewMatrix; }
		inline const XMFLOAT4X4A& GetProjMatrix() const { return m_projMatrix; }
	private:
		virtual IComponentManager* GetComponentManager() const override;

		float CalcBufferWidth();
		float CalcBufferHeight();

		// �÷� ����, ���� Ÿ�� ��, ���̴� ���ҽ� ��, ���� ���ٽ� ����, ���� ���ٽ� �� ����
		HRESULT CreateBufferAndView();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();				// FoV, Ŭ���� ���(near, far plane)�Ǵ� â ũ�� ����Ǿ��� ��� ȣ�� �ʿ�
		void UpdateFullbufferViewport();			// â ũ�� ����Ǿ��� ��� ȣ�� �ʿ�
	private:
		ComPtr<ID3D11RenderTargetView> m_cpColorBufferRTV;
		ComPtr<ID3D11ShaderResourceView> m_cpColorBufferSRV;		// ������ ��� ���ս� �ʿ�
		ComPtr<ID3D11DepthStencilView> m_cpDepthStencilBufferDSV;
		XMFLOAT4A m_backgroundColor;
		uint8_t m_fov;
		int8_t m_depth;
		PROJECTION_METHOD m_projMethod;
		CLEAR_FLAG m_clearFlag;
		ClippingPlanes m_clippingPlanes;
		NormalizedViewportRect m_nzdVp;
		XMFLOAT4X4A m_viewMatrix;
		XMFLOAT4X4A m_projMatrix;
		D3D11_VIEWPORT m_fullbufferViewport;
		float m_tessMinDist;		// �ִ������� �׼����̼� �Ǵ� �ּ� �Ÿ� ���� ��
									// (�� ������ �Ÿ������� m_minTessFactor�� m_maxTessFactor�� ������ �� ���̷� ������ Factor�� ���ȴ�.
		float m_tessMaxDist;		// �ּ������� �׼����̼� �Ǵ� �ִ� �Ÿ� ���� ��
		float m_minTessExponent;
		float m_maxTessExponent;
	};
}
