#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pConstantBuffer = nullptr;

	_pTextureRV = nullptr;
	_pSamplerLinear = nullptr;

	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidState;

	CreateCameras();

	_Origin.x = 0, _Origin.y = 0, _Origin.z = 0;
	_planetPos.x = _Origin.x + 2, _planetPos.y = 0, _planetPos.z = _Origin.z + 2;
	_moonPos.x = _planetPos.x + 2, _moonPos.y = 0, _moonPos.z = _planetPos.z + 2;

	_rotationValue = 0.0f;
	_halfRotation = false;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	// Initialize the world matrix

    // Initialize the view matrix

	_camera->Update();
	_secondCamera->Update();
	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);
	
	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	//Set the sample description for our textures
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	CreateObjects();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);


	return hr;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 1280, 720};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
	if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;
	
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
		
    };

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;
	
    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();
	
	if (FAILED(hr))
		return hr;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, 0, &_depthStencilView);
    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;        

    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set up lighting
	_lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
	_diffuseMaterial = XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f);
	_diffuseLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	_ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	_ambientMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	_specularMtrl = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	_specularLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	//Specular Power
	_specularPower = 10.0f;

	//Eye Position set the camera position
	_eyePos = XMFLOAT3(0.0f, 4.0f, -10.0f);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);
	
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC)); //This will check if memory is cleared
	wfdesc.FillMode = D3D11_FILL_WIREFRAME; //this then sets the fillmode so that directX only renders our cubes in wireframe
	wfdesc.CullMode = D3D11_CULL_NONE; //finally the culling is set to none so we can see the backs of cubes as they spin
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	D3D11_RASTERIZER_DESC solidState;
	ZeroMemory(&solidState, sizeof(D3D11_RASTERIZER_DESC));
	solidState.FillMode = D3D11_FILL_SOLID;
	solidState.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&solidState, &_solidState);

	_pImmediateContext->RSSetState(_solidState);

	_objMeshData = OBJLoader::Load("sphere.obj", _pd3dDevice);
	_tankMeshData = OBJLoader::Load("Hercules.obj", _pd3dDevice);
	

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_solidState) _solidState->Release();
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

	//gTime shows how much time has elapsed from the last update
	_gTime = t;

    //
    // Animate the objects
    //

	UpdateObjects(t);
	UpdateCamera(t);
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);
    //
    // Update variables
    //
    ConstantBuffer cb;
	
	if (_camSelection == 0)
	{
		cb.mView = XMMatrixTranspose(_camera->GetViewMtrx());
		cb.mProjection = XMMatrixTranspose(_camera->GetProjectionMtrx());
		_eyePos = _camera->GetEye();
	}

	if (_camSelection == 1)
	{
		cb.mView = XMMatrixTranspose(_secondCamera->GetViewMtrx());
		cb.mProjection = XMMatrixTranspose(_secondCamera->GetProjectionMtrx());
		_eyePos = _secondCamera->GetEye();
	}
	//Set lighting for the constant buffer
	cb.DiffuseLight = _diffuseLight;
	cb.DiffuseMtrl = _diffuseMaterial;
	cb.LightVecW = _lightDirection;
	cb.AmbientLight = _ambientLight;
	cb.AmbientMaterial = _ambientMaterial;
	cb.SpecularLight = _specularLight;
	cb.SpecularMtrl = _specularMtrl;
	cb.SpecularPower = _specularPower;
	cb.EyePosW = _eyePos;

	//Set the vertex and pixel shader
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	RenderObjects(stride, offset, cb);
   _pSwapChain->Present(0, 0);
}

void Application::UpdateCamera(static float t)
{
	if (GetAsyncKeyState(VK_DOWN))
	{
		_pImmediateContext->RSSetState(_solidState);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		_pImmediateContext->RSSetState(_wireFrame);
	}

	if (GetAsyncKeyState(0x31))
	{
		_camSelection = 0;
	}

	if (GetAsyncKeyState(0x32))
	{
		_camSelection = 1;
	}
	if (GetAsyncKeyState(0x33))
	{
		_camSelection = 2;
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		_camera->atSelection = false;
		_secondCamera->atSelection = false;
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		_camera->atSelection = true;
		_secondCamera->atSelection = true;
	}
	if (_camera->atSelection == false)
	{
		XMFLOAT3 lookToVect = _camera->GetTo(); //Firstly get our get to vector
		XMFLOAT3 camPos = _camera->GetEye(); //Secondly get our cameras position
		//Get the distance between the two vectors by finding the difference of each coord
		XMFLOAT3 distance;
		distance.x = lookToVect.x - camPos.x;
		distance.y = lookToVect.y - camPos.y;
		distance.z = lookToVect.z - camPos.z;

		//Normalise the vector to get the direction when it equals 1
		XMVECTOR dirVector = XMLoadFloat3(&distance);
		XMVECTOR XMVector3Normalize(dirVector);
		XMStoreFloat3(&distance, dirVector);

		float strafe = _camera->GetEye().x;

		if (GetAsyncKeyState(0x57))
		{
			//Create a new position that is equal to the current position plus the direction multiplied by a small value
			XMFLOAT3 distMoved = XMFLOAT3(camPos.x + distance.x * 0.001f, camPos.y + distance.y * 0.001f, camPos.z + distance.z * 0.001f);
			_camera->SetPosition(distMoved);
		}
		else if (GetAsyncKeyState(0x53))
		{
			//For going backwards, simply minus the position rather than plusing
			XMFLOAT3 distMoved = XMFLOAT3(camPos.x - distance.x * 0.001f, camPos.y - distance.y * 0.001f, camPos.z - distance.z * 0.001f);
			_camera->SetPosition(distMoved);
		}
		else if (GetAsyncKeyState(0x41))
		{
			//Move camera position left
			_camera->SetPosition(XMFLOAT3(strafe -= 0.01f, camPos.y, camPos.z));
			_camera->SetTo(XMFLOAT3(lookToVect.x -= 0.01f, lookToVect.y, lookToVect.z));
		}
		else if (GetAsyncKeyState(0x44))
		{
			//move camera position right
			_camera->SetPosition(XMFLOAT3(strafe += 0.01f, camPos.y, camPos.z));
			_camera->SetTo(XMFLOAT3(lookToVect.x += 0.01f, lookToVect.y, lookToVect.z));
		}
	}
	_camera->Update();
	_secondCamera->Update();
}

void Application::UpdateObjects(static float t)
{
	if ((_rotationValue != _rotationMax) && _halfRotation == false)
	{
		_rotationValue += 0.5;
	}
	else if (_rotationValue == _rotationMax)
	{
		_halfRotation = true;
	}
	else if ((_halfRotation == true) && _rotationValue != _rotationMin)
	{
		_rotationValue -= 0.5;
	}
	else if ((_rotationValue == _rotationMin) && _halfRotation == true)
	{
		_halfRotation = false;
	}
	
	_cube->Update(XMMatrixRotationZ(t) * XMMatrixTranslation(_planetPos.x + _rotationValue, _planetPos.y, _planetPos.z) * XMMatrixRotationY(t));
	_pyramid->Update(XMMatrixTranslation(5, 3, 1));
	_grid->Update(XMMatrixScaling(5, 5, 5) * XMMatrixTranslation(-3, -5, 0));
	_testObject->Update(XMMatrixRotationY(t) * XMMatrixTranslation(0, 0, 3));
	_ship->Update(XMMatrixScaling(0.2, 0.2, 0.2)* XMMatrixTranslation(2, -5.5, 2));
}

void Application::RenderObjects(UINT stride,UINT offset, ConstantBuffer cb)
{
	_cube->Render(_pImmediateContext, cb, _pConstantBuffer, 36, stride, offset);
	_testObject->Render(_pImmediateContext, cb, _pConstantBuffer, stride, offset);
	_ship->Render(_pImmediateContext, cb, _pConstantBuffer, stride, offset);
	_pyramid->Render(_pImmediateContext, cb, _pConstantBuffer, 18, stride, offset);
	_grid->Render(_pImmediateContext, cb, _pConstantBuffer, 96, stride, offset);
}

void Application::CreateObjects()
{
	_testObject = new Object("Hercules.obj", _pd3dDevice, false);
	_testObject->LoadTexture(_pd3dDevice, L"Hercules_COLOR.dds");

	_cube = new Cube(_pd3dDevice);
	_cube->LoadTexture(_pd3dDevice, L"Crate_COLOR.dds");

	_pyramid = new Pyramid(_pd3dDevice);
	_pyramid->LoadTexture(_pd3dDevice, L"Crate_COLOR.dds");

	_grid = new Grid(_pd3dDevice);
	_grid->LoadTexture(_pd3dDevice, L"Crate_COLOR.dds");

	_ship = new Object("ship2.obj", _pd3dDevice, true);
	_ship->LoadTexture(_pd3dDevice, L"Hercules_COLOR.dds");

}

void Application::CreateCameras()
{
	XMFLOAT3 _pos = XMFLOAT3(0.0f, 6.0f, -15.0f);
	XMFLOAT3 _at = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 _up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 _to = XMFLOAT3(0.0f, 0.0f, 5.0f);

	XMFLOAT3 _secondEyePos = XMFLOAT3(2.0f, 20.0f, -2.0f);
	XMFLOAT3 _secondAt = XMFLOAT3(0.0f, 0.0f, -2.0f);
	XMFLOAT3 _secondUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 _secondTo = XMFLOAT3(10.0f, 60.0f, 340.0f);
	_camera = new Camera(_pos, _at, _up, _to, _WindowWidth, _WindowHeight, 0.01f, 100.0f);
	_secondCamera = new Camera(_secondEyePos, _secondAt, _secondUp, _secondTo, _WindowWidth, _WindowHeight, 0.01f, 100.0f);
}