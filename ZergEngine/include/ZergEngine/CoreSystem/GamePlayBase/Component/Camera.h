#pragma once

#include <ZergEngine\CoreSystem\EngineConstants.h>
#include <ZergEngine\CoreSystem\DataStructure\ViewportRect.h>
#include <ZergEngine\CoreSystem\DataStructure\Ray.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	// �񵿱� �� �ε� �� ī�޶� ������Ʈ ���� �� �����ؾ� �� ����
	// ���� �����忡�� â ũ�� �����ϴ� ������ �� ���� ���� �� �� ������ ũ�� ������ ���ͼ�
	// Depth/Stencil ���۳� ���� Ÿ���� ����� ��쿡 ���� â�� ���� ũ�� ����ġ ����, �Ǵ� �� ���� Resize���� �������� ���� ũ����
	// ���� ������ ���� �� �ִ�. ���� �� ��� SRWLock ����ȭ�� �Ұ����ҵ�.
	// => �ϴ� �ذ�) ���� �� �並 �����ϴ� CreateBufferAndView() �Լ��� ���� ���� ���� �����忡�� ����ǰ� �Ѵ�.
	// ������ ���ҽ� ������ ���� �������� �Ͼ�Ƿ� ���۰� �������� ���� �����ӿ����� ������ ������ �߻�. ������ �񵿱� �ε��� ����/�� ������ ����ϸ�
	// ������ ����ȭ�� �ʿ������µ� ���� ���� ���� �����忡���� �׻� ������ ����ȭ�� �ʿ������Ƿ� �̰� �� ���Ƽ ����̴�. (���� �Ͼ�� ���� ������)
	class Camera : public IComponent
	{
		friend class CameraManager;
		friend class Renderer;
	public:
		static constexpr ComponentType TYPE = ComponentType::Camera;
		static constexpr bool IsCreatable() { return true; }

		Camera() noexcept;
		virtual ~Camera() = default;

		virtual ComponentType GetType() const override { return ComponentType::Camera; }
		const XMFLOAT4A& GetBackgroundColor() const { return m_backgroundColor; }
		void SetBackgroundColor(const XMFLOAT4A color) { m_backgroundColor = color; }
		void XM_CALLCONV SetBackgroundColor(FXMVECTOR color) { XMStoreFloat4A(&m_backgroundColor, color); }
		ProjectionMethod GetProjectionMethod() const { return m_projMethod; }
		void SetProjectionMethod(ProjectionMethod method);
		uint32_t GetFieldOfView() const { return m_fov; }
		void SetFieldOfView(uint8_t degree);
		float GetNearPlane() const { return m_nearPlane; }
		float GetFarPlane() const { return m_farPlane; }
		void SetClippingPlanes(float nearPlane, float farPlane);

		// x, y, w, h must be normalized value.
		void SetViewportRect(float x, float y, float width, float height);
		const ViewportRect& GetViewportRect() const { return m_viewportRect; }

		int8_t GetDepth() const { return m_depth; }
		void SetDepth(int8_t depth);

		void SetMinimumDistanceStartTessellation(float distance) { m_tessMinDist = distance; }
		void SetMaximumDistanceEndTessellation(float distance) { m_tessMaxDist = distance; }

		// Only the range of 0.0 to 6.0 is allowed. Other ranges are ignored.
		bool SetMinimumTessellationExponent(float exponent);

		// Only the range of 0.0 to 6.0 is allowed. Other ranges are ignored.
		bool SetMaximumTessellationExponent(float exponent);

		float GetMinDistanceTessellationToStart() const { return m_tessMinDist; }
		float GetMaxDistanceTessellationToStart() const { return m_tessMaxDist; }
		float GetMinimumTessellationExponent() const { return m_minTessExponent; }
		float GetMaximumTessellationExponent() const { return m_maxTessExponent; }

		ID3D11RenderTargetView* GetColorBufferRTVComInterface() const { return m_cpColorBufferRTV.Get(); }
		ID3D11ShaderResourceView* GetColorBufferSRVComInterface() const { return m_cpColorBufferSRV.Get(); }
		ID3D11DepthStencilView* GetDepthStencilBufferDSVComInterface() const { return m_cpDepthStencilBufferDSV.Get(); }

		XMMATRIX XM_CALLCONV GetViewMatrix() const { return XMLoadFloat4x4A(&m_viewMatrix); }
		XMMATRIX XM_CALLCONV GetProjMatrix() const { return XMLoadFloat4x4A(&m_projMatrix); }
		XMMATRIX XM_CALLCONV GetViewportMatrix();

		POINT ScreenPointToCameraPoint(POINT pt) const;
		Ray ScreenPointToRay(POINT pt);	// ���� ���� ���� Ray�� ��ȯ�մϴ�.
	private:
		virtual IComponentManager* GetComponentManager() const override;

		const D3D11_VIEWPORT& GetEntireBufferViewport() const { return m_entireBufferViewport; }

		// ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
		// �Ű������� ���޵Ǵ� width, height �Է��� ���� ü���� ���� ũ��. ���� ī�޶��� ����ȭ����Ʈ ũ�⸦ �������� ���� ���� ũ�⸦ ����ؾ� ��.
		HRESULT CreateBuffer(uint32_t width, uint32_t height);			// �÷� ����, ���� Ÿ�� ��, ���̴� ���ҽ� ��, ���� ���ٽ� ����, ���� ���ٽ� �� ����
		void UpdateViewMatrix();
		void UpdateProjectionMatrix(uint32_t width, uint32_t height);			// FoV, Ŭ���� ���(near, far plane)�Ǵ� â ũ�� ����Ǿ��� ��� ȣ�� �ʿ�
		void UpdateEntireBufferViewport(uint32_t width, uint32_t height);
		// ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
	private:
		ComPtr<ID3D11RenderTargetView> m_cpColorBufferRTV;
		ComPtr<ID3D11ShaderResourceView> m_cpColorBufferSRV;		// ������ ��� ���ս� �ʿ�
		ComPtr<ID3D11DepthStencilView> m_cpDepthStencilBufferDSV;
		XMFLOAT4A m_backgroundColor;
		uint8_t m_fov;
		int8_t m_depth;
		ProjectionMethod m_projMethod;
		ClearFlag m_clearFlag;
		float m_nearPlane;
		float m_farPlane;
		ViewportRect m_viewportRect;
		XMFLOAT4X4A m_viewMatrix;
		XMFLOAT4X4A m_projMatrix;
		D3D11_VIEWPORT m_entireBufferViewport;
		float m_tessMinDist;		// �ִ������� �׼����̼� �Ǵ� �ּ� �Ÿ� ���� ��
									// (�� ������ �Ÿ������� m_minTessFactor�� m_maxTessFactor�� ������ �� ���̷� ������ Factor�� ���ȴ�.
		float m_tessMaxDist;		// �ּ������� �׼����̼� �Ǵ� �ִ� �Ÿ� ���� ��
		float m_minTessExponent;
		float m_maxTessExponent;
	};
}
