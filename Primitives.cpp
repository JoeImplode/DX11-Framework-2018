#include "Primitives.h"
Primitives::Primitives(ID3D11Device* device)
{
	_device = device;
}

Primitives::~Primitives() 
{
}

void Primitives::CreateVertexBuffer(SimpleVertex Vertices[], ID3D11Device* device, int size)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = Vertices;
	device->CreateBuffer(&bd, &InitData, &_primVertexBuffer);
	
}

void Primitives::CreateIndexBuffer(WORD indices[], ID3D11Device* device,UINT indexLength)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * indexLength;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA IndexData;
	ZeroMemory(&IndexData, sizeof(IndexData));
	IndexData.pSysMem = indices;
	device->CreateBuffer(&bd, &IndexData, &_primIndexBuffer);
}

void Primitives::Render(ID3D11DeviceContext* context, ConstantBuffer cb, ID3D11Buffer* ConstantBuffer,UINT indexCount,UINT stride, UINT offset)
{
	XMMATRIX primMatrix = XMLoadFloat4x4(&_primMatrix);
	context->PSSetShaderResources(0, 1, &_primTexture);
	context->IASetVertexBuffers(0, 1, &_primVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(_primIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	cb.mWorld = XMMatrixTranspose(primMatrix);
	context->UpdateSubresource(ConstantBuffer, 0, nullptr, &cb, 0, 0);
	context->DrawIndexed(indexCount, 0, 0);
}


void Primitives::LoadTexture(ID3D11Device* device, const wchar_t* fileName)
{
	CreateDDSTextureFromFile(device, fileName, nullptr, &_primTexture);
}

void Primitives::Update(XMMATRIX transformation)
{
	XMStoreFloat4x4(&_primMatrix, transformation);
}
