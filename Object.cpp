#include "Object.h"

Object::Object(char* fileName, ID3D11Device* device, bool invertTextCoords)
{
	_objectTexture = nullptr;

	//Load an OBJ file and store it as the device
	_objectMesh = OBJLoader::Load(fileName, device, invertTextCoords);
}

Object::~Object()
{

}

void Object::LoadTexture(ID3D11Device* device, const wchar_t* fileName)
{
	//Load a texture and set it to objectTexture
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