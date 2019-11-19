#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

using namespace DirectX;

class Camera
{

private:
	//Private attributes for camera position and view volume
	XMFLOAT3 _eye;
	XMFLOAT3 _at;
	XMFLOAT3 _up;
	XMFLOAT3 _to;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	//Attributes to hold the view and projection matrices which will be passed to the shader
	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

public:
	bool atSelection = true;
	Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up,XMFLOAT3 to, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	void Update();

	void SetPosition(XMFLOAT3 eye);
	void SetLookat(XMFLOAT3 at);
	void SetUp(XMFLOAT3 up);
	void SetTo(XMFLOAT3 to);

	XMFLOAT3 GetAt();
	XMFLOAT3 GetTo();
	XMFLOAT3 GetEye();
	XMMATRIX GetViewMtrx();
	XMMATRIX GetProjectionMtrx();
	XMMATRIX GetCombinedMatrix();

	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	~Camera();
};