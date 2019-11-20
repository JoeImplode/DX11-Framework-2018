#pragma once
#include "OBJLoader.h"
#include "DDSTextureLoader.h"
#include <windows.h>
#include "resource.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <iostream>
using namespace DirectX;
class Primitives
{
public:
	Primitives(ID3D11Device* device);
	~Primitives();
	void CreateVertexBuffer(SimpleVertex Vertices[], ID3D11Device* device, int size);
	void CreateIndexBuffer(WORD indices[],ID3D11Device* device, UINT indexLength);
	void Update(XMMATRIX transformation);
	void Render(ID3D11DeviceContext* context, ConstantBuffer cb, ID3D11Buffer* ConstantBuffer,UINT indexCount, UINT stride, UINT offset);
	void LoadTexture(ID3D11Device* device, const wchar_t* fileName);

	ID3D11Buffer* _primVertexBuffer;
	ID3D11Buffer* _primIndexBuffer;
private:

protected:
	ID3D11ShaderResourceView* _primTexture;
	XMFLOAT4X4 _primMatrix;
	ID3D11Device* _device;

};

