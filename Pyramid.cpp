#include "Pyramid.h"



Pyramid::Pyramid(ID3D11Device* device) : Primitives(device)
{
	SimpleVertex pyramidVertices[] =
	{
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,0.0f)},
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,1.0f)},
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f,0.0f)},
		{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.05f,0.05f)},
	};
	CreateVertexBuffer(pyramidVertices, _device);

	WORD pyramidIndices[] =
	{
		2,4,1,
		1,4,0,
		0,4,3,
		3,4,2,
		3,2,1,
		1,0,1
	};
	CreateIndexBuffer(pyramidIndices, _device, 18);
}


Pyramid::~Pyramid()
{
}
