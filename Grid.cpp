#include "Grid.h"



Grid::Grid(ID3D11Device* device) : Primitives(device)
{
	SimpleVertex gridVertices[] =
	{
		{XMFLOAT3(-10.0f, -1.0f, -10.0f),XMFLOAT3(-10.0f, -1.0f, -10.0f), XMFLOAT2(0.0f,1.0f)},
		{XMFLOAT3(-5.0f, -1.0f, -10.0f),XMFLOAT3(-5.0f, -1.0f, -10.0f) , XMFLOAT2(1.0f,1.0f)},
		{XMFLOAT3(0.0f, -1.0f, -10.0f), XMFLOAT3(0.0f, -1.0f, -10.0f), XMFLOAT2(0.0f,1.0f)},
		{XMFLOAT3(5.0f, -1.0f, -10.0f),XMFLOAT3(5.0f, -1.0f, -10.0f) , XMFLOAT2(1.0f,1.0f)},
		{XMFLOAT3(10.0f, -1.0f, -10.0f), XMFLOAT3(10.0f, -1.0f, -10.0f) , XMFLOAT2(0.0f,1.0f)},

		{XMFLOAT3(-10.0f, -1.0f, -5.0f), XMFLOAT3(-10.0f, -1.0f, -5.0f) , XMFLOAT2(0.0f,0.0f)},
		{XMFLOAT3(-5.0f, -1.0f, -5.0f), XMFLOAT3(-5.0f, -1.0f, -5.0f) , XMFLOAT2(1.0f,0.0f)},
		{XMFLOAT3(0.0f, -1.0f, -5.0f), XMFLOAT3(0.0f, -1.0f, -5.0f) , XMFLOAT2(0.0f,0.0f)},
		{XMFLOAT3(5.0f, -1.0f, -5.0f), XMFLOAT3(5.0f, -1.0f, -5.0f) , XMFLOAT2(1.0f,0.0f)},
		{XMFLOAT3(10.0f, -1.0f, -5.0f), XMFLOAT3(10.0f, -1.0f, -5.0f) , XMFLOAT2(0.0f,0.0f)},

		{XMFLOAT3(-10.0f, -1.0f, 0.0f), XMFLOAT3(-10.0f, -1.0f, 0.0f) , XMFLOAT2(0.0f,1.0f)},
		{XMFLOAT3(-5.0f, -1.0f, 0.0f), XMFLOAT3(-5.0f, -1.0f, 0.0f) , XMFLOAT2(1.0f,1.0f)},
		{XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) , XMFLOAT2(0.0f,1.0f)},
		{XMFLOAT3(5.0f, -1.0f, 0.0f), XMFLOAT3(5.0f, -1.0f, 0.0f) , XMFLOAT2(1.0f,1.0f)},
		{XMFLOAT3(10.0f, -1.0f, 0.0f), XMFLOAT3(10.0f, -1.0f, 0.0f) , XMFLOAT2(0.0f,1.0f)},

		{XMFLOAT3(-10.0f, -1.0f, 5.0f), XMFLOAT3(-10.0f, -1.0f, 5.0f) , XMFLOAT2(0.0f,0.0f)},
		{XMFLOAT3(-5.0f, -1.0f, 5.0f), XMFLOAT3(-5.0f, -1.0f, 5.0f) , XMFLOAT2(1.0f,0.0f)},
		{XMFLOAT3(0.0f, -1.0f, 5.0f), XMFLOAT3(0.0f, -1.0f, 5.0f) , XMFLOAT2(0.0f,0.0f)},
		{XMFLOAT3(5.0f, -1.0f, 5.0f), XMFLOAT3(5.0f, -1.0f, 5.0f) , XMFLOAT2(1.0f,0.0f)},
		{XMFLOAT3(10.0f, -1.0f, 5.0f), XMFLOAT3(10.0f, -1.0f, 5.0f) , XMFLOAT2(0.0f,0.0f)},

		{XMFLOAT3(-10.0f, -1.0f, 10.0f), XMFLOAT3(-10.0f, -1.0f, 10.0f) , XMFLOAT2(0.0f,1.0f)},
		{XMFLOAT3(-5.0f, -1.0f, 10.0f), XMFLOAT3(-5.0f, -1.0f, 10.0f) , XMFLOAT2(1.0f,1.0f)},
		{XMFLOAT3(0.0f, -1.0f, 10.0f), XMFLOAT3(0.0f, -1.0f, 10.0f) , XMFLOAT2(0.0f,1.0f)},
		{XMFLOAT3(5.0f, -1.0f, 10.0f), XMFLOAT3(5.0f, -1.0f, 10.0f) , XMFLOAT2(1.0f,1.0f)},
		{XMFLOAT3(10.0f, -1.0f, 10.0f), XMFLOAT3(10.0f, -1.0f, 10.0f) , XMFLOAT2(0.0f,1.0f)},
	};
	CreateVertexBuffer(gridVertices, _device);

	WORD gridIndices[] =
	{
		6,1,0, 5,6,0,
		7,2,1, 6,7,1,
		8,3,2, 7,8,2,
		9,4,3, 8,9,3,

		11,6,5, 10,11,5,
		12,7,6, 11,12,6,
		13,8,7, 12,13,7,
		14,9,8, 13,14,8,

		16,11,10, 15,16,10,
		17,12,11, 16,17,11,
		18,13,12, 17,18,12,
		19,14,13, 18,19,13,

		21,16,15, 20,21,15,
		22,17,16, 21,22,16,
		23,18,17, 22,23,17,
		24,19,18, 23,24,18,
	};
	CreateIndexBuffer(gridIndices, _device,96);
}


Grid::~Grid()
{
}
