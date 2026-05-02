#include <ZergEngine\CoreSystem\BulletDXMath.h>

using namespace ze;

XMMATRIX BtToDX::ConvertMatrix(const btTransform& t)
{
	XMFLOAT3 trans = BtToDX::ConvertVector(t.getOrigin());
	XMFLOAT4 rot = BtToDX::ConvertQuaternion(t.getRotation());

	XMMATRIX m = XMMatrixRotationQuaternion(XMLoadFloat4(&rot));
	m.r[3] = XMLoadFloat3(&trans);

	return m;
}

btTransform DXToBt::ConvertMatrix(const XMFLOAT4X4& m)
{
	XMMATRIX matrix = XMLoadFloat4x4(&m);

	return DXToBt::ConvertMatrix(matrix);
}

btTransform DXToBt::ConvertMatrix(FXMMATRIX m)
{
	XMVECTOR vScale;
	XMVECTOR vRot;
	XMVECTOR vTrans;
	XMMatrixDecompose(&vScale, &vRot, &vTrans, m);

	// XMFLOAT3A scale;
	XMFLOAT4A rot;
	XMFLOAT3A trans;
	XMStoreFloat4A(&rot, vRot);
	XMStoreFloat3A(&trans, vTrans);

	btTransform t;
	t.setOrigin(DXToBt::ConvertVector(trans));
	t.setRotation(DXToBt::ConvertQuaternion(rot));

	return t;
}
