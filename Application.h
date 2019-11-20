#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <iostream>
#include "DDSTextureLoader.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "Camera.h"
#include "Object.h"
#include "Primitives.h"
#include "Cube.h"
#include "Pyramid.h"
#include "Grid.h"

using namespace std;
using namespace DirectX;

class Object;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;

	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;

	ID3D11ShaderResourceView* _pTextureRV;
	ID3D11ShaderResourceView* _TankTexture;

	ID3D11SamplerState*		_pSamplerLinear;
	ID3D11Buffer*           _pConstantBuffer;

	//Objects

	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;

	XMFLOAT3				_Origin, _planetPos, _moonPos;
	
	//Lighting
	XMFLOAT3				_lightDirection;
	XMFLOAT4				_diffuseMaterial;
	XMFLOAT4				_diffuseLight;
	XMFLOAT4				_ambientLight;
	XMFLOAT4				_ambientMaterial;
	XMFLOAT4				_specularMtrl;
	XMFLOAT4				_specularLight;
	float					_specularPower;
	float					_gTime;

	//Objects
	MeshData				_objMeshData;
	MeshData				_tankMeshData;

	//Cameras
	Camera*					_camera;
	Camera*					_secondCamera;

	Object*					_testObject;

	int currentPath = 0;

	//Camera pos, at up and to
	//-----------------------------------------------------
	XMFLOAT3 _eyePos = XMFLOAT3(0.0f, 6.0f, -15.0f);
	XMFLOAT3 _at = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 _up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 _to = XMFLOAT3(0.0f, 0.0f, 5.0f);

	XMFLOAT3 _secondEyePos = XMFLOAT3(2.0f, 20.0f, -2.0f);
	XMFLOAT3 _secondAt = XMFLOAT3(0.0f, 0.0f, -2.0f);
	XMFLOAT3 _secondUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 _secondTo = XMFLOAT3(10.0f, 60.0f, 340.0f);
	//-----------------------------------------------------

	//Depth stencil items
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidState;

	//-----------------------------------------------------
	float _rotationValue;
	bool _halfRotation;
	const float _rotationMax = 5.0f;
	const float _rotationMin = 0.0f;

	bool _wireFrameCheck = false;
	int _camSelection = 0;
	//-----------------------------------------------------

	Cube* _cube;
	Pyramid* _pyramid;
	Grid* _grid;
	void UpdateCamera(static float t);
	void UpdateObjects(static float t);
	void RenderObjects(UINT stride, UINT offset, ConstantBuffer cb);
	void CreateObjects();
private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();

	UINT _WindowHeight;
	UINT _WindowWidth;
public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	
	void Draw();
};

