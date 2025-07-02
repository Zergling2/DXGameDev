#pragma once

#include <ZergEngine\CoreSystem\Effect\EffectInterface.h>
#include <ZergEngine\CoreSystem\ShaderResource\ConstantBuffer.h>

namespace ze
{
	class MSCameraMergeEffect : public IEffect
	{
	private:
		enum DIRTY_FLAG : DWORD
		{
			PRIMITIVE_TOPOLOGY					= 1 << 0,
			INPUT_LAYOUT						= 1 << 1,
			SHADER								= 1 << 2,
			CONSTANTBUFFER_PER_MSCAMERA_MERGE	= 1 << 3,

			COUNT,

			ALL = ((COUNT - 1) << 1) - 1
		};
	public:
		MSCameraMergeEffect() noexcept
			: m_dirtyFlag(ALL)
			, m_pInputLayout(nullptr)
			, m_pVertexShader(nullptr)
			, m_pPixelShader(nullptr)
			, m_cbPerMSCameraMerge()
			, m_cbPerMSCameraMergeCache()
			, m_pTextureSRVArray{ nullptr }
		{
		}
		virtual ~MSCameraMergeEffect() = default;

		virtual void Init() override;
		virtual void Release() override;

		void SetMergeParameters(float width, float height, float topLeftX, float topLeftY) noexcept;
		void SetMergeTexture(ID3D11ShaderResourceView* pMergeTextureSRV);
	private:
		virtual void ApplyImpl(ID3D11DeviceContext* pDeviceContext) noexcept override;
		virtual void KickedOutOfDeviceContext() noexcept override;

		void ApplyShader(ID3D11DeviceContext* pDeviceContext) noexcept;
		void ApplyPerMSCameraMergeConstantBuffer(ID3D11DeviceContext* pDeviceContext) noexcept;

		void ClearTextureSRVArray();
	private:
		DWORD m_dirtyFlag;

		ID3D11InputLayout* m_pInputLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		ConstantBuffer<CbPerMSCameraMerge> m_cbPerMSCameraMerge;
		CbPerMSCameraMerge m_cbPerMSCameraMergeCache;

		// [0] RENDER_RESULT
		ID3D11ShaderResourceView* m_pTextureSRVArray[1];
	};
}
