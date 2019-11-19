#include "Cube.h"

Cube::Cube(ID3D11Device* device) : Primitives(device)
{
	SimpleVertex newSimpleVertex[] =
	{
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, //top front right
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f , 0.0f) }, //top front left
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),  XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f,1.0f) }, //bottom front left
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f,1.0f) }, //bottom front right

		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f,1.0f) }, //bottom back right
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f,0.0f) }, //back top right
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f,0.0f) }, //back top left
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f,1.0f) }, //bottom back left
	};
	CreateVertexBuffer(newSimpleVertex, _device);
	WORD tempIndices[] =
	{
		0, 1, 2,
			2, 3, 0,
			0, 3, 4,
			4, 5, 0,
			0, 5, 6,
			6, 1, 0,
			1, 6, 7,
			7, 2, 1,
			7, 4, 3,
			3, 2, 7,
			4, 7, 6,
			6, 5, 4
			};
	CreateIndexBuffer(tempIndices, _device, 36);
}

Cube::~Cube()
{

}