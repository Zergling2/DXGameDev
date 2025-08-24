#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class Texture2D
	{
	public:
		Texture2D() noexcept
			: m_cpTex2D(nullptr)
			, m_cpSRV(nullptr)
		{
		}
		Texture2D(ComPtr<ID3D11Texture2D> cpTex2D, ComPtr<ID3D11ShaderResourceView> cpSRV) noexcept
			: m_cpTex2D(std::move(cpTex2D))
			, m_cpSRV(std::move(cpSRV))
		{
		}
		Texture2D(const Texture2D& other) noexcept
			: m_cpTex2D(other.m_cpTex2D)
			, m_cpSRV(other.m_cpSRV)
		{
		}
		Texture2D(Texture2D&& other) noexcept
			: m_cpTex2D(std::move(other.m_cpTex2D))
			, m_cpSRV(std::move(other.m_cpSRV))
		{
		}
		Texture2D& operator=(const Texture2D& other) noexcept
		{
			Texture2D temp(other);

			*this = std::move(temp);

			return *this;
		}
		Texture2D& operator=(Texture2D&& other) noexcept
		{
			m_cpTex2D = std::move(other.m_cpTex2D);
			m_cpSRV = std::move(other.m_cpSRV);

			return *this;
		}
		bool operator==(const Texture2D& other) noexcept
		{
			return (m_cpTex2D == other.m_cpTex2D) && (m_cpSRV == other.m_cpSRV);
		}
		virtual ~Texture2D() = default;
		operator bool() const
		{
			return m_cpTex2D != nullptr && m_cpSRV != nullptr;
		}

		virtual void Reset()
		{
			m_cpTex2D.Reset();
			m_cpSRV.Reset();
		}

		ID3D11Texture2D* GetTex2DComInterface() const { return m_cpTex2D.Get(); }
		ID3D11ShaderResourceView* GetSRVComInterface() const { return m_cpSRV.Get(); }

		void GetTexture2DDesc(D3D11_TEXTURE2D_DESC* pDesc) { m_cpTex2D->GetDesc(pDesc); }
		void GetShaderResourceViewDesc(D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc) { m_cpSRV->GetDesc(pDesc); }
		SIZE GetResolution() const;
	public:
		ComPtr<ID3D11Texture2D> m_cpTex2D;
		ComPtr<ID3D11ShaderResourceView> m_cpSRV;
	};

	class RenderTargetTexture : public Texture2D
	{
	public:
		RenderTargetTexture() noexcept
			: m_cpRTV(nullptr)
		{
		}
		RenderTargetTexture(ComPtr<ID3D11RenderTargetView> cpRTV) noexcept
			: m_cpRTV(std::move(cpRTV))
		{
		}
		RenderTargetTexture(const RenderTargetTexture& other) noexcept
			: m_cpRTV(other.m_cpRTV)
		{
		}
		RenderTargetTexture(RenderTargetTexture&& other) noexcept
			: m_cpRTV(std::move(other.m_cpRTV))
		{
		}
		RenderTargetTexture& operator=(const RenderTargetTexture& other) noexcept
		{
			RenderTargetTexture temp(other);

			*this = std::move(temp);

			return *this;
		}
		RenderTargetTexture& operator=(RenderTargetTexture&& other) noexcept
		{
			m_cpRTV = std::move(other.m_cpRTV);

			return *this;
		}
		virtual ~RenderTargetTexture() = default;
		operator bool() const
		{
			return m_cpRTV != nullptr && Texture2D::operator bool();
		}

		virtual void Reset() override
		{
			m_cpRTV.Reset();

			Texture2D::Reset();
		}

		ID3D11RenderTargetView* GetRTVComInterface() const { return m_cpRTV.Get(); }
		void GetRenderTargetViewDesc(D3D11_RENDER_TARGET_VIEW_DESC* pDesc) { m_cpRTV->GetDesc(pDesc); }
	private:
		ComPtr<ID3D11RenderTargetView> m_cpRTV;
	};
}
