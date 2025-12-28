#pragma once

#include <ZergEngine\CoreSystem\Platform.h>

namespace ze
{
	class IEffect;

	// Effect 상태 관리 기능을 포함해서 DeviceContext 래핑
	class EffectDeviceContext
	{
	public:
		EffectDeviceContext() noexcept;
		EffectDeviceContext(ID3D11DeviceContext* pDeviceContext) noexcept;
		~EffectDeviceContext() = default;

		void AttachDeviceContext(ID3D11DeviceContext* pDeviceContext) noexcept;
		ID3D11DeviceContext* GetDeviceContextComInterface() const { return m_pDeviceContext; }

		void Apply(IEffect* pEffect) noexcept;

		void ClearState() noexcept;
		void ClearDepthStencilView(
			/* [annotation] */
			_In_  ID3D11DepthStencilView* pDepthStencilView,
			/* [annotation] */
			_In_  UINT clearFlags,
			/* [annotation] */
			_In_  FLOAT depth,
			/* [annotation] */
			_In_  UINT8 stencil)
		{
			m_pDeviceContext->ClearDepthStencilView(pDepthStencilView, clearFlags, depth, stencil);
		}
		void ClearRenderTargetView(
			/* [annotation] */
			_In_  ID3D11RenderTargetView* pRenderTargetView,
			/* [annotation] */
			_In_  const FLOAT colorRGBA[4])
		{
			m_pDeviceContext->ClearRenderTargetView(pRenderTargetView, colorRGBA);
		}
		void ClearUnorderedAccessViewFloat(
			/* [annotation] */
			_In_  ID3D11UnorderedAccessView* pUnorderedAccessView,
			/* [annotation] */
			_In_  const FLOAT values[4])
		{
			m_pDeviceContext->ClearUnorderedAccessViewFloat(pUnorderedAccessView, values);
		}
		void ClearUnorderedAccessViewUint(
			/* [annotation] */
			_In_  ID3D11UnorderedAccessView* pUnorderedAccessView,
			/* [annotation] */
			_In_  const UINT values[4])
		{
			m_pDeviceContext->ClearUnorderedAccessViewUint(pUnorderedAccessView, values);
		}

		void IASetVertexBuffers(
			/* [annotation] */
			_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1) UINT startSlot,
			/* [annotation] */
			_In_range_(0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - startSlot) UINT numBuffers,
			/* [annotation] */
			_In_reads_opt_(numBuffers) ID3D11Buffer* const* ppVertexBuffers,
			/* [annotation] */
			_In_reads_opt_(numBuffers) const UINT* pStrides,
			/* [annotation] */
			_In_reads_opt_(numBuffers) const UINT* pOffsets)
		{
			m_pDeviceContext->IASetVertexBuffers(startSlot, numBuffers, ppVertexBuffers, pStrides, pOffsets);
		}
		void IASetIndexBuffer(
			/* [annotation] */
			_In_opt_ ID3D11Buffer* pIndexBuffer,
			/* [annotation] */
			_In_ DXGI_FORMAT format,
			/* [annotation] */
			_In_ UINT offset)
		{
			m_pDeviceContext->IASetIndexBuffer(pIndexBuffer, format, offset);
		}
		void Draw(
			/* [annotation] */
			_In_  UINT vertexCount,
			/* [annotation] */
			_In_  UINT startVertexLocation)
		{
			m_pDeviceContext->Draw(vertexCount, startVertexLocation);
		}
		void DrawIndexed(
			/* [annotation] */
			_In_  UINT indexCount,
			/* [annotation] */
			_In_  UINT startIndexLocation,
			/* [annotation] */
			_In_  INT baseVertexLocation
		)
		{
			m_pDeviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
		}
	private:
		ID3D11DeviceContext* m_pDeviceContext;
		IEffect* m_pCurrentEffect;
	};
}
