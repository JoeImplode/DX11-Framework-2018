#include "Object.h"

Object::Object(char* fileName, ID3D11Device* device, bool invertTextCoords)
{
	_objectTexture = nullptr;
	_objectMesh = OBJLoader::Load(fileName, device, invertTextCoords);
}

Object::~Object()
{

}

void Object::LoadTexture(ID3D11Device* device, const wchar_t* fileName)
{
	CreateDDSTextureFromFile(device, fileName, nullptr, &_objectTexture);
}

void Object::Render(ID3D11DeviceContext* context,ConstantBuffer cb, ID3D11Buffer* ConstantBuffer,UINT stride, UINT offset)
{
	XMMATRIX _objMatrix = XMLoadFloat4x4(&_objectMatrix);

	context->PSSetShaderResources(0, 1, &_objectTexture);
	context->IASetVertexBuffers(0, 1, &_objectMesh.VertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(_objectMesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	cb.mWorld = XMMatrixTranspose(_objMatrix);
	context->UpdateSubresource(ConstantBuffer, 0, nullptr, &cb, 0, 0);
	context->DrawIndexed(_objectMesh.IndexCount, 0, 0);
}

void Object::Update(XMMATRIX objectTransformation)
{
	XMStoreFloat4x4(&_objectMatrix, objectTransformation);
}
XMFLOAT4X4 Object::GetObjMatrix()
{
	return this->_objectMatrix;
}

XMFLOAT3 Object::GetMtrxLookTo()
{
	return XMFLOAT3(this->_objectMatrix._13, this->_objectMatrix._23, this->_objectMatrix._33);
}

XMFLOAT3 Object::GetMtrxUp()
{
	return XMFLOAT3(this->_objectMatrix._12, this->_objectMatrix._22, this->_objectMatrix._32);
}

XMFLOAT3 Object::GetMtrxRight()
{
	return XMFLOAT3(this->_objectMatrix._11, this->_objectMatrix._21, this->_objectMatrix._31);
}

XMFLOAT3 Object::GetMtrxPosition()
{
	return XMFLOAT3(this->_objectMatrix._14, this->_objectMatrix._24, this->_objectMatrix._34);
}