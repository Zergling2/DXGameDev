#include <ZergEngine\System\Component\Camera.h>
#include <ZergEngine\System\Graphics.h>
#include <ZergEngine\System\ComponentSystem.h>
#include <ZergEngine\Common\EngineMath.h>
#include <ZergEngine\System\GameObject.h>
#include <ZergEngine\System\Debug.h>

using namespace zergengine;

static LPCWSTR const THIS_FILE_NAME = L"Camera.cpp";

bool Camera::IsCreatable()
{
	if (ComponentSystem::CameraManager::s_cameras.size() < 4)
		return true;
	else
		return false;
}

Camera::Camera()
	: m_rtvColorBuffer(nullptr)
	, m_srvColorBuffer(nullptr)
	, m_dsvDepthStencilBuffer(nullptr)
	, m_fov(CAMERA_FIELD_OF_VIEW_DEFAULT)
	, m_depth(CAMERA_DEPTH_DEFAULT)
	, m_projMethod(CAMERA_PROJECTION_METHOD_DEFAULT)
	, m_clearFlag(CAMERA_CLEAR_FLAG_DEFAULT)
	, m_backgroundColor(CAMERA_BACKGROUND_COLOR_DEFAULT)
	, m_clippingPlanes(CAMERA_CLIPPING_NEAR_PLANE_DEFAULT, CAMERA_CLIPPING_FAR_PLANE_DEFAULT)
	, m_nzdvp(CAMERA_VIEWPORT_X_DEFAULT, CAMERA_VIEWPORT_Y_DEFAULT, CAMERA_VIEWPORT_WIDTH_DEFAULT, CAMERA_VIEWPORT_HEIGHT_DEFAULT)
	, m_viewMatrix()
	, m_projMatrix()
	, m_viewport()
	, m_tessMinDist(50.0f)
	, m_tessMaxDist(400.0f)
	, m_minTessExponent(0.0f)
	, m_maxTessExponent(6.0f)
{
	XMStoreFloat4x4A(&m_viewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4A(&m_projMatrix, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(static_cast<float>(m_fov)),
		static_cast<float>(Screen::GetWidth()) / static_cast<float>(Screen::GetHeight()),
		m_clippingPlanes.nearZ, m_clippingPlanes.farZ)
	);

	this->UpdateProjectionMatrix();
	this->UpdateViewportInfo();
}

void Camera::SetFieldOfView(uint8_t degree)
{
	Math::Clamp(degree, static_cast<uint8_t>(50), static_cast<uint8_t>(120));
	m_fov = degree;

	this->UpdateProjectionMatrix();
}

void Camera::SetClippingPlanes(float nearZ, float farZ)
{
	if (nearZ <= 0.0f || farZ < nearZ)
		return;

	m_clippingPlanes.nearZ = nearZ;
	m_clippingPlanes.farZ = farZ;

	this->UpdateProjectionMatrix();
}

void Camera::SetViewportRect(float x, float y, float w, float h)
{
	if (x < 0.0f || x > 1.0f ||
		y < 0.0f || y > 1.0f ||
		w <= 0.0f || w > 1.0f ||
		h <= 0.0f || h > 1.0f)
		return;

	if (m_nzdvp.x != x || m_nzdvp.y != y || m_nzdvp.w != w || m_nzdvp.h != h)
	{
		// 버퍼 재생성 유도 (비싼 작업이므로 설정이 실질적으로 변경된 경우에만 버퍼 재생성)
		m_rtvColorBuffer.Reset();
		m_srvColorBuffer.Reset();
		m_dsvDepthStencilBuffer.Reset();

		m_nzdvp.x = x;
		m_nzdvp.y = y;
		m_nzdvp.w = w;
		m_nzdvp.h = h;
	}

	this->UpdateProjectionMatrix();
	this->UpdateViewportInfo();
}

void Camera::SetDepth(int8_t depth)
{
	m_depth = depth;

	ComponentSystem::CameraManager::RequestUpdate();		// 카메라 깊이값으로 정렬 다시
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
	printf("재생성!\n");
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC descColorBuffer;
#if defined(DEBUG) || (_DEBUG)
	ZeroMemory(&descColorBuffer, sizeof(descColorBuffer));
#endif
	// 컬러 버퍼 생성 및 컬러 버퍼에 대한 렌더 타겟 뷰 및 셰이더 리소스 뷰 생성
	descColorBuffer.Width = static_cast<UINT>(static_cast<float>(Screen::GetWidth()) * m_nzdvp.w);
	descColorBuffer.Height = static_cast<UINT>(static_cast<float>(Screen::GetHeight()) * m_nzdvp.h);
	descColorBuffer.MipLevels = 1;
	descColorBuffer.ArraySize = 1;
	descColorBuffer.Format = BACKBUFFER_FORMAT;
	descColorBuffer.SampleDesc = Graphics::GetSwapChainDescriptor().SampleDesc;
	descColorBuffer.Usage = D3D11_USAGE_DEFAULT;
	descColorBuffer.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descColorBuffer.CPUAccessFlags = 0;
	descColorBuffer.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> colorBuffer;

	hr = Graphics::GetDevice()->CreateTexture2D(&descColorBuffer, nullptr, colorBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return hr;
	}

	assert(m_rtvColorBuffer.Get() == nullptr);
	hr = Graphics::GetDevice()->CreateRenderTargetView(colorBuffer.Get(), nullptr, m_rtvColorBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC descColorBufferSRV;
#if defined(DEBUG) || (_DEBUG)
	ZeroMemory(&descColorBufferSRV, sizeof(descColorBufferSRV));
#endif
	descColorBufferSRV.Format = descColorBuffer.Format;
	descColorBufferSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	assert(m_srvColorBuffer.Get() == nullptr);
	hr = Graphics::GetDevice()->CreateShaderResourceView(colorBuffer.Get(), &descColorBufferSRV, m_srvColorBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return hr;
	}

	D3D11_TEXTURE2D_DESC descDepthStencilBuffer;
#if defined(DEBUG) || (_DEBUG)
	ZeroMemory(&descDepthStencilBuffer, sizeof(descDepthStencilBuffer));
#endif
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

	hr = Graphics::GetDevice()->CreateTexture2D(&descDepthStencilBuffer, nullptr, depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return hr;
	}

	assert(m_dsvDepthStencilBuffer.Get() == nullptr);
	hr = Graphics::GetDevice()->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, m_dsvDepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return hr;
	}

	return hr;
}

void Camera::UpdateViewMatrix()
{
	const GameObject* pCameraOwner = GetGameObject();
	assert(pCameraOwner != nullptr);

	const XMVECTOR cameraRotation = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat4A(&pCameraOwner->m_transform.m_rotation));
	const XMVECTOR forward = XMVector3Rotate(CAMERA_FORWARD_VECTOR, cameraRotation);
	const XMVECTOR up = XMVector3Rotate(CAMERA_UP_VECTOR, cameraRotation);
	const XMVECTOR eye = XMLoadFloat3A(&pCameraOwner->m_transform.m_position);
	const XMVECTOR at = eye + forward;
	XMStoreFloat4x4A(&m_viewMatrix, XMMatrixLookAtLH(eye, at, up));
}

void Camera::UpdateProjectionMatrix()
{
	const float colorBufferWidth = static_cast<float>(Screen::GetWidth()) * m_nzdvp.w;
	const float colorBufferHeight = static_cast<float>(Screen::GetHeight()) * m_nzdvp.h;
	const float aspectRatio = colorBufferWidth / colorBufferHeight;

	XMStoreFloat4x4A(&m_projMatrix, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(static_cast<float>(m_fov)),
		aspectRatio,
		m_clippingPlanes.nearZ,
		m_clippingPlanes.farZ)
	);
}

void Camera::UpdateViewportInfo()
{
	const float colorBufferWidth = static_cast<float>(Screen::GetWidth()) * m_nzdvp.w;
	const float colorBufferHeight = static_cast<float>(Screen::GetHeight()) * m_nzdvp.h;

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = colorBufferWidth;
	m_viewport.Height = colorBufferHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}

void Camera::SystemJobOnEnabled()
{
}

void Camera::SystemJobOnDisabled()
{
}
