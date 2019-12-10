#pragma once
#include "Object.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>

class Ship : public Object
{
public:
	Ship(char* fileName, ID3D11Device* device, bool invertTextCoords);

	float velocity;
	float rotate;
	XMFLOAT3 direction;
	XMFLOAT3 position;
	XMMATRIX tempMatrix;
	void Move();
	XMFLOAT3 GetPosition();
	void SetPosition(XMFLOAT3 newPosition);

private:
protected:
};

