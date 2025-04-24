#include <ZergEngine\CoreSystem\GamePlayBase\Component\Camera.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\GraphicDevice.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>
#include <ZergEngine\Common\EngineConstants.h>

using namespace ze;

constexpr uint8_t CAMERA_FIELD_OF_VIEW_DEFAULT = 92;
constexpr int8_t CAMERA_DEPTH_DEFAULT = 0;
constexpr PROJECTION_METHOD CAMERA_PROJECTION_METHOD_DEFAULT = PROJECTION_METHOD::PERSPECTIVE;
constexpr CLEAR_FLAG CAMERA_CLEAR_FLAG_DEFAULT = CLEAR_FLAG::SOLID_COLOR;
const XMVECTORF32 CAMERA_BACKGROUND_COLOR_DEFAULT = Colors::Blue;

constexpr float CAMERA_CLIPPING_NEAR_PLANE_DEFAULT = 0.3f;
constexpr float CAMERA_CLIPPING_FAR_PLANE_DEFAULT = 1000.0f;
constexpr float CAMERA_VIEWPORT_X_DEFAULT = 0.0f;
constexpr float CAMERA_VIEWPORT_Y_DEFAULT = 0.0f;
constexpr float CAMERA_VIEWPORT_WIDTH_DEFAULT = 1.0f;
constexpr float CAMERA_VIEWPORT_HEIGHT_DEFAULT = 1.0f;

Camera::Camera() noexcept
	: IComponent(CameraManager.AssignUniqueId())
	, m_cpColorBufferRTV(nullptr)
	, m_cpColorBufferSRV(nullptr)
	, m_cpDepthStencilBufferDSV(nullptr)
	, m_backgroundColor(CAMERA_BACKGROUND_COLOR_DEFAULT)
	, m_fov(CAMERA_FIELD_OF_VIEW_DEFAULT)
	, m_depth(CAMERA_DEPTH_DEFAULT)
	, m_projMethod(CAMERA_PROJECTION_METHOD_DEFAULT)
	, m_clearFlag(CAMERA_CLEAR_FLAG_DEFAULT)
	, m_nearPlane(CAMERA_CLIPPING_NEAR_PLANE_DEFAULT)
	, m_farPlane(CAMERA_CLIPPING_FAR_PLANE_DEFAULT)
	, m_viewportRect(CAMERA_VIEWPORT_X_DEFAULT, CAMERA_VIEWPORT_Y_DEFAULT, CAMERA_VIEWPORT_WIDTH_DEFAULT, CAMERA_VIEWPORT_HEIGHT_DEFAULT)
	, m_viewMatrix()
	, m_projMatrix()
	, m_fullbufferViewport()
	, m_tessMinDist(50.0f)
	, m_tessMaxDist(400.0f)
	, m_minTessExponent(0.0f)
	, m_maxTessExponent(6.0f)
{
}

void Camera::SetFieldOfView(uint8_t degree)
{
	// FOV 변경
	// 백 버퍼는 그대로 사용 가능.
	// 투영 행렬만 업데이트하면 된다.

	Math::Clamp(degree, static_cast<uint8_t>(50), static_cast<uint8_t>(120));
	m_fov = degree;

	this->UpdateProjectionMatrix();
}

void Camera::SetClippingPlanes(float nearPlane, float farPlane)
{
	// Near plane, Far plane 변경
	// 백 버퍼는 그대로 사용 가능.
	// 투영 행렬만 업데이트하면 된다.

	if (nearPlane <= 0.0f || farPlane < nearPlane)
		return;

	m_nearPlane = nearPlane;
	m_farPlane = farPlane;

	this->UpdateProjectionMatrix();
}

void Camera::SetViewportRect(float x, float y, float w, float h)
{
	// 해당 카메라 렌더링 결과가 위치할 스크린 영역의 위치 및 크기 업데이트
	// 백 버퍼를 필요한 크기만큼으로 재생성한다.
	// 백 버퍼 크기 변경 -> 종횡비가 변경되므로 투영 행렬 재생성이 필요하다.
	// 다시 버퍼 영역 전체를 나타내도록 D3D11_VIEWPORT 구조체도 업데이트 해야한다.
	
	if (x < 0.0f || x > 1.0f ||
		y < 0.0f || y > 1.0f ||
		w <= 0.0f || w > 1.0f ||
		h <= 0.0f || h > 1.0f)
		return;

	if (m_viewportRect.m_x != x || m_viewportRect.m_y != y || m_viewportRect.m_width != w || m_viewportRect.m_height != h)
	{
		// 컬러버퍼 및 뎁스스텐실 버퍼를 재생성하게 한다. (비싼 작업이므로 설정이 실질적으로 변경된 경우에만 버퍼 재생성)
		m_cpColorBufferRTV.Reset();
		m_cpColorBufferSRV.Reset();
		m_cpDepthStencilBufferDSV.Reset();

		m_viewportRect.m_x = x;
		m_viewportRect.m_y = y;
		m_viewportRect.m_width = w;
		m_viewportRect.m_height = h;

		this->UpdateProjectionMatrix();
		this->UpdateFullbufferViewport();
	}
}

void Camera::SetDepth(int8_t depth)
{
	m_depth = depth;

	assert(m_pGameObject != nullptr);

	// 지연된 오브젝트인 경우에는 비동기 씬에서 생성되었을 수 있으므로 함부로 CameraManager.Update()를 호출하면 안된다.
	if (!m_pGameObject->IsDeferred())
		CameraManager.Update();	// 카메라 깊이값으로 정렬 다시
}

bool Camera::SetMinimumTessellationExponent(float exponent)
{
	if (exponent < 0.0f || exponent > 6.0f)
		return false;

	m_minTessExponent = exponent;
	return true;
}

bool Camera::SetMaximumTessellationExponent(float exponent)
{
	if (exponent < 0.0f || exponent > 6.0f)
		return false;

	m_maxTessExponent = exponent;
	return true;
}

HRESULT Camera::CreateBufferAndView()
{
	HRESULT hr = S_OK;

	do
	{
		m_cpColorBufferRTV.Reset();
		m_cpColorBufferSRV.Reset();
		m_cpDepthStencilBufferDSV.Reset();

		ComPtr<ID3D11RenderTargetView> cpColorBufferRTV;
		ComPtr<ID3D11ShaderResourceView> cpColorBufferSRV;
		ComPtr<ID3D11DepthStencilView> cpDepthStencilBufferDSV;

		D3D11_TEXTURE2D_DESC descColorBuffer;
		ZeroMemory(&descColorBuffer, sizeof(descColorBuffer));
		// 컬러 버퍼 생성 및 컬러 버퍼에 대한 렌더 타겟 뷰 및 셰이더 리소스 뷰 생성
		descColorBuffer.Width = static_cast<UINT>(this->CalcBufferWidth());
		descColorBuffer.Height = static_cast<UINT>(this->CalcBufferHeight());
		descColorBuffer.MipLevels = 1;
		descColorBuffer.ArraySize = 1;
		descColorBuffer.Format = BACKBUFFER_FORMAT;
		descColorBuffer.SampleDesc = GraphicDevice.GetSwapChainDesc().SampleDesc;
		descColorBuffer.Usage = D3D11_USAGE_DEFAULT;
		descColorBuffer.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;		// 렌더링 + 카메라병합셰이더 리소스
		descColorBuffer.CPUAccessFlags = 0;
		descColorBuffer.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> colorBuffer;
		hr = GraphicDevice.GetDeviceComInterface()->CreateTexture2D(&descColorBuffer, nullptr, colorBuffer.GetAddressOf());
		if (FAILED(hr))
			break;

		hr = GraphicDevice.GetDeviceComInterface()->CreateRenderTargetView(colorBuffer.Get(), nullptr, cpColorBufferRTV.GetAddressOf());
		if (FAILED(hr))
			break;

		D3D11_SHADER_RESOURCE_VIEW_DESC descColorBufferSRV;
		ZeroMemory(&descColorBufferSRV, sizeof(descColorBufferSRV));
		descColorBufferSRV.Format = descColorBuffer.Format;
		descColorBufferSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		hr = GraphicDevice.GetDeviceComInterface()->CreateShaderResourceView(colorBuffer.Get(), &descColorBufferSRV, cpColorBufferSRV.GetAddressOf());
		if (FAILED(hr))
			break;

		D3D11_TEXTURE2D_DESC descDepthStencilBuffer;
		ZeroMemory(&descDepthStencilBuffer, sizeof(descDepthStencilBuffer));
		// 뎁스/스텐실 버퍼 생성 및 뎁스스텐실뷰 생성
		descDepthStencilBuffer.Width = descColorBuffer.Width;
		descDepthStencilBuffer.Height = descColorBuffer.Height;
		descDepthStencilBuffer.MipLevels = 1;
		descDepthStencilBuffer.ArraySize = 1;
		descDepthStencilBuffer.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepthStencilBuffer.SampleDesc = descColorBuffer.SampleDesc;
		descDepthStencilBuffer.Usage = D3D11_USAGE_DEFAULT;
		descDepthStencilBuffer.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepthStencilBuffer.CPUAccessFlags = 0;
		descDepthStencilBuffer.MiscFlags = 0;

		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		hr = GraphicDevice.GetDeviceComInterface()->CreateTexture2D(&descDepthStencilBuffer, nullptr, depthStencilBuffer.GetAddressOf());
		if (FAILED(hr))
			break;

		hr = GraphicDevice.GetDeviceComInterface()->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, cpDepthStencilBufferDSV.GetAddressOf());
		if (FAILED(hr))
			break;

		// 필요한 리소스 생성이 모두 성공한 경우에 멤버로 이동
		m_cpColorBufferRTV = std::move(cpColorBufferRTV);
		m_cpColorBufferSRV = std::move(cpColorBufferSRV);
		m_cpDepthStencilBufferDSV = std::move(cpDepthStencilBufferDSV);
	} while (false);

	return hr;
}

IComponentManager* Camera::GetComponentManager() const
{
	return &CameraManager;
}

float Camera::CalcBufferWidth()
{
	return static_cast<float>(Window.GetWidth()) * m_viewportRect.m_width;
}

float Camera::CalcBufferHeight()
{
	return static_cast<float>(Window.GetHeight()) * m_viewportRect.m_height;
}

void Camera::UpdateViewMatrix()
{
	const GameObject* pCameraOwner = m_pGameObject;
	assert(pCameraOwner != nullptr);

	const XMMATRIX w = pCameraOwner->CalcWorldTransformMatrix();
	XMVECTOR scale;
	XMVECTOR rotation;		// Camera rotation quaternion
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotation, &translation, w);

	const XMVECTOR forward = XMVector3Rotate(LOCAL_FORWARD, rotation);
	const XMVECTOR up = XMVector3Rotate(LOCAL_UP, rotation);
	const XMVECTOR eye = translation;
	const XMVECTOR at = eye + forward;

	XMStoreFloat4x4A(&m_viewMatrix, XMMatrixLookAtLH(eye, at, up));
}

void Camera::UpdateProjectionMatrix()
{
	const float colorBufferWidth = this->CalcBufferWidth();
	const float colorBufferHeight = this->CalcBufferHeight();
	const float aspectRatio = colorBufferWidth / colorBufferHeight;

	XMStoreFloat4x4A(&m_projMatrix, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(static_cast<float>(m_fov)),
		aspectRatio,
		m_nearPlane,
		m_farPlane)
	);
}

void Camera::UpdateFullbufferViewport()
{
	const float colorBufferWidth = this->CalcBufferWidth();
	const float colorBufferHeight = this->CalcBufferHeight();

	m_fullbufferViewport.TopLeftX = 0;
	m_fullbufferViewport.TopLeftY = 0;
	m_fullbufferViewport.Width = colorBufferWidth;
	m_fullbufferViewport.Height = colorBufferHeight;
	m_fullbufferViewport.MinDepth = 0.0f;
	m_fullbufferViewport.MaxDepth = 1.0f;
}
