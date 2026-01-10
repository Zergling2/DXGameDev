#include "ATVItemMesh.h"
#include "..\MainFrm.h"
#include "..\MeshInspectorFormView.h"
#include <ZergEngine\CoreSystem\Resource\StaticMesh.h>
#include <ZergEngine\CoreSystem\InputLayout.h>
#include <ZergEngine\CoreSystem\Debug.h>

void ATVItemStaticMesh::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. 인스펙터 폼 뷰 교체
	CMeshInspectorFormView* pInspector =
		static_cast<CMeshInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CMeshInspectorFormView)));


	pInspector->SetATVItemToModify(this);		// 수정 시 데이터 반영을 위하여

	// 2. 변경된 인스펙터 폼 뷰에 Mesh 정보 보여주기
	ID3D11Buffer* pVB = m_spMesh->GetVertexBuffer();
	if (!pVB)
		return;

	D3D11_BUFFER_DESC vbDesc;
	pVB->GetDesc(&vbDesc);

	pInspector->m_staticMeshSubsetCount = static_cast<int>(m_spMesh->m_subsets.size());

	pInspector->m_staticMeshName = m_spMesh->GetName();

	pInspector->m_staticMeshVertexCount = vbDesc.ByteWidth / ze::InputLayoutHelper::GetStructureByteStride(ze::VertexFormatType::PositionNormalTangentTexCoord);

	pInspector->m_staticMeshVertexFormatType = ze::Debug::VertexFormatToString(ze::VertexFormatType::PositionNormalTangentTexCoord);

	// 변수 -> 컨트롤로 업데이트
	pInspector->UpdateData(FALSE);
}
