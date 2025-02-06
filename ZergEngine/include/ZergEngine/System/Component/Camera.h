#pragma once

#include <ZergEngine\System\Component\IComponent.h>
#include <ZergEngine\System\ComponentSystem.h>

namespace zergengine
{
	// 비동기 씬 로딩에서 카메라 컴포넌트 생성 시 주의해야 할 사항
	// 메인 스레드에서 창 크기 변경하는 시점에 백 버퍼 접근 및 백 버퍼의 크기 정보를 얻어와서
	// Depth/Stencil 버퍼나 렌더 타겟을 만드는 경우에 크기 불일치 문제, 또는 백 버퍼 Resize도중 접근으로 인한 크래시
	// 등의 문제가 생길 수 있다. 따라서 이 경우 SRWLock 동기화가 불가피할듯.

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
		void UpdateProjectionMatrix();		// FoV, 클리핑 평면(near, far plane)또는 창 크기 변경되었을 경우 호출 필요
		void UpdateViewportInfo();			// 창 크기 변경되었을 경우 호출 필요
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
		float m_tessMinDist;		// 최대한으로 테셀레이션 되는 최소 거리 정의 ┓
									// (이 사이의 거리에서는 m_minTessFactor와 m_maxTessFactor에 설정한 값 사이로 보간된 Factor가 사용된다.
		float m_tessMaxDist;		// 최소한으로 테셀레이션 되는 최대 거리 정의 ┛
		float m_minTessExponent;
		float m_maxTessExponent;
	};
}
