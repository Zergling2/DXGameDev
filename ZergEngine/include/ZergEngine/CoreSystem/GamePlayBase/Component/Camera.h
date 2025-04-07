#pragma once

#include <ZergEngine\Common\EngineHelper.h>
#include <ZergEngine\Common\EngineConstants.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{

	// 비동기 씬 로딩에서 카메라 컴포넌트 생성 시 주의해야 할 사항
	// 메인 스레드에서 창 크기 변경하는 시점에 백 버퍼 접근 및 백 버퍼의 크기 정보를 얻어와서
	// Depth/Stencil 버퍼나 렌더 타겟을 만드는 경우에 크기 불일치 문제, 또는 백 버퍼 Resize도중 접근으로 인한 크래시
	// 등의 문제가 생길 수 있다. 따라서 이 경우 SRWLock 동기화가 불가피할듯.
	// => 일단 해결) 버퍼 및 뷰를 생성하는 CreateBufferAndView() 함수를 메인 엔진 루프 스레드에서 실행되게 한다.
	// 단점은 리소스 생성이 엔진 루프에서 일어나므로 버퍼가 생성되지 않은 프레임에서는 렌더링 지연이 발생. 하지만 비동기 로딩중 버퍼/뷰 생성을 허용하면
	// 스레드 동기화가 필요해지는데 엔진 메인 루프 스레드에서도 항상 스레드 동기화가 필요해지므로 이게 더 페널티 요소이다. (자주 일어나는 일을 빠르게)
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

		// 컬러 버퍼, 렌더 타겟 뷰, 셰이더 리소스 뷰, 뎁스 스텐실 버퍼, 뎁스 스텐실 뷰 생성
		HRESULT CreateBufferAndView();
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();				// FoV, 클리핑 평면(near, far plane)또는 창 크기 변경되었을 경우 호출 필요
		void UpdateFullbufferViewport();			// 창 크기 변경되었을 경우 호출 필요
	private:
		ComPtr<ID3D11RenderTargetView> m_cpColorBufferRTV;
		ComPtr<ID3D11ShaderResourceView> m_cpColorBufferSRV;		// 렌더링 결과 병합시 필요
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
		float m_tessMinDist;		// 최대한으로 테셀레이션 되는 최소 거리 정의 ┓
									// (이 사이의 거리에서는 m_minTessFactor와 m_maxTessFactor에 설정한 값 사이로 보간된 Factor가 사용된다.
		float m_tessMaxDist;		// 최소한으로 테셀레이션 되는 최대 거리 정의 ┛
		float m_minTessExponent;
		float m_maxTessExponent;
	};
}
