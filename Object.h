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

class Object
{
public:
	Object(char* fileName, ID3D11Device* device, bool invertTextCoords);
	~Object();
	void LoadTexture(ID3D11Device* device, const wchar_t* fileName);
	void Update(XMMATRIX objectTransformation);
	void Render(ID3D11DeviceContext* context, ConstantBuffer cb, ID3D11Buffer* ConstantBuffer, UINT stride, UINT offset);
	XMFLOAT4X4 GetObjMatrix();

	XMFLOAT3 GetMtrxLookTo();
	XMFLOAT3 GetMtrxUp();
	XMFLOAT3 GetMtrxRight();
	XMFLOAT3 GetMtrxPosition();

private:
	ID3D11ShaderResourceView* _objectTexture;
	MeshData _objectMesh;
protected:
	XMFLOAT4X4 _objectMatrix;
};