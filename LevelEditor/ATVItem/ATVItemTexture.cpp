#include "ATVItemTexture.h"
#include "..\MainFrm.h"
#include "..\TextureInspectorFormView.h"

void ATVItemTexture::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. �ν����� �� �� ��ü
	CTextureInspectorFormView* pInspector =
		static_cast<CTextureInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CTextureInspectorFormView)));


	pInspector->SetATVItemToModify(this);		// ���� �� ������ �ݿ��� ���Ͽ�

	// 2. ����� �ν����� �� �信 Texture ���� �����ֱ�
	ID3D11Texture2D* pTexture = m_texture.GetTex2DComInterface();
	ID3D11ShaderResourceView* pSRV = m_texture.GetSRVComInterface();

	if (!pTexture || !pSRV)
		return;

	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	pTexture->GetDesc(&textureDesc);
	pSRV->GetDesc(&srvDesc);

	// 1.
	pInspector->m_staticTextureArraySize = textureDesc.ArraySize;
	// 2.
	pInspector->m_staticTextureMipLevels = textureDesc.MipLevels;
	// 3.
	pInspector->m_staticTextureType = ze::Debug::SRVDimensionToString(srvDesc.ViewDimension);
	// 4.
	pInspector->m_staticTextureFormat = ze::Debug::DXGIFormatToString(textureDesc.Format);
	// 5.
	TCHAR str[32];
	StringCbPrintf(str, sizeof(str), _T("%d x %d"), textureDesc.Width, textureDesc.Height);
	pInspector->m_staticTextureSize = str;

	// ���� -> ��Ʈ�ѷ� ������Ʈ
	pInspector->UpdateData(FALSE);
}
