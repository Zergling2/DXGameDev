#pragma once

#include <ZergEngine\System\Component\IComponent.h>
#include <ZergEngine\System\ComponentSystem.h>

namespace zergengine
{
	// �񵿱� �� �ε����� ī�޶� ������Ʈ ���� �� �����ؾ� �� ����
	// ���� �����忡�� â ũ�� �����ϴ� ������ �� ���� ���� �� �� ������ ũ�� ������ ���ͼ�
	// Depth/Stencil ���۳� ���� Ÿ���� ����� ��쿡 ũ�� ����ġ ����, �Ǵ� �� ���� Resize���� �������� ���� ũ����
	// ���� ������ ���� �� �ִ�. ���� �� ��� SRWLock ����ȭ�� �Ұ����ҵ�.

	class Camera : public IComponent
	{
		friend class Graphics;
		friend class ComponentSystem::CameraManager;
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::CAMERA;
		static bool IsCreatable();
		Camera();
		virtual ~Camera()
		{
		}
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
		inline const NormalizedViewportRect& GetViewportRect() const { return m_nzdvp; }

		// x, y, w, h must be normalized value.
		void SetViewportRect(float x, float y, float w, float h);
		inline int8_t GetDepth() const { return m_depth; }
		void SetDepth(int8_t depth);

		inline void SetMinimumDistanceStartTessellation(float distance) { m_tessMinDist = distance; }
		inline void SetMaximumDistanceEndTessellation(float distance) { m_tessMaxDist = distance; }

		// 0.0 ~ 6.0
		bool SetMinimumTessellationExponent(float exponent);

		// 0.0 ~ 6.0
		bool SetMaximumTessellationExponent(float exponent);

		inline float GetMinimumDistanceForTessellationToStart() const { return m_tessMinDist; }
		inline float GetMaximumDistanceForTessellationToStart() const { return m_tessMaxDist; }
		inline float GetMinimumTessellationExponent() const { return m_minTessExponent; }
		inline float GetMaximumTessellationExponent() const { return m_maxTessExponent; }
	private:
		HRESULT CreateBufferAndView();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();		// FoV, Ŭ���� ���(near, far plane)�Ǵ� â ũ�� ����Ǿ��� ��� ȣ�� �ʿ�
		void UpdateViewportInfo();			// â ũ�� ����Ǿ��� ��� ȣ�� �ʿ�
		virtual void SystemJobOnEnabled() override;
		virtual void SystemJobOnDisabled() override;
	private:
		ComPtr<ID3D11RenderTargetView> m_rtvColorBuffer;
		ComPtr<ID3D11ShaderResourceView> m_srvColorBuffer;
		ComPtr<ID3D11DepthStencilView> m_dsvDepthStencilBuffer;
		uint8_t m_fov;
		int8_t m_depth;
		PROJECTION_METHOD m_projMethod;
		CLEAR_FLAG m_clearFlag;
		XMFLOAT4A m_backgroundColor;
		ClippingPlanes m_clippingPlanes;
		NormalizedViewportRect m_nzdvp;
		XMFLOAT4X4A m_viewMatrix;
		XMFLOAT4X4A m_projMatrix;
		D3D11_VIEWPORT m_viewport;
		float m_tessMinDist;		// �ִ������� �׼����̼� �Ǵ� �ּ� �Ÿ� ���� ��
									// (�� ������ �Ÿ������� m_minTessFactor�� m_maxTessFactor�� ������ �� ���̷� ������ Factor�� ���ȴ�.
		float m_tessMaxDist;		// �ּ������� �׼����̼� �Ǵ� �ִ� �Ÿ� ���� ��
		float m_minTessExponent;
		float m_maxTessExponent;
	};
}
