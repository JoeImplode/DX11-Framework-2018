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
	_cubeIndexBuffer = nullptr;
	_cubeVertexBuffer = nullptr;
	_pConstantBuffer = nullptr;

	_pyramidIndexBuffer = nullptr;
	_pyramidVertexBuffer = nullptr;

	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _solidState;
	
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
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 4.0f, -2.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));
    // Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 100.0f));
	
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
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
   SimpleVertex vertices[] =
    {
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
    };

   SimpleVertex pyramidVertices[] =
   {
	   { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
	   { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
	   { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
	   { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
	   { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
   };

   SimpleVertex gridVertices[] =
   {
	   {XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(2.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(3.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(4.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},

	   {XMFLOAT3(0.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(2.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(3.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(4.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},

	   {XMFLOAT3(0.0f, -1.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(1.0f, -1.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(2.0f, -1.0f, 2.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(3.0f, -1.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(4.0f, -1.0f, 2.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},

	   {XMFLOAT3(0.0f, -1.0f, 3.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(1.0f, -1.0f, 3.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(2.0f, -1.0f, 3.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(3.0f, -1.0f, 3.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(4.0f, -1.0f, 3.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},

	   {XMFLOAT3(0.0f, -1.0f, 4.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(1.0f, -1.0f, 4.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(2.0f, -1.0f, 4.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(3.0f, -1.0f, 4.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   {XMFLOAT3(4.0f, -1.0f, 4.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
	   
   };
    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 100;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
	
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_cubeVertexBuffer);

	D3D11_SUBRESOURCE_DATA NewInitData;
	ZeroMemory(&NewInitData, sizeof(NewInitData));
	NewInitData.pSysMem = pyramidVertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &NewInitData, &_pyramidVertexBuffer);
	
	D3D11_SUBRESOURCE_DATA gridData;
	ZeroMemory(&gridData, sizeof(gridData));
	gridData.pSysMem = gridVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &gridData, &_gridVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
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

	WORD pyramidIndices[] =
	{
		2,4,1,
		1,4,0,
		0,4,3,
		3,4,2,
		3,2,1,
		1,0,1
	};

	WORD gridIndices[] =
	{
		6,1,0, 5,6,0,
		7,2,1, 6,7,1,
		2,3,8, 2,8,7,
		3,4,9, 3,9,8,

		5,6,11, 5,11,10,
		6,7,12, 6,12,11,
		7,8,13, 7,13,12,
		8,9,14, 8,14,13,

		10,11,16, 10,16,15,
		11,12,17, 11,17,16,
		12,13,18, 12,18,17,
		13,14,19, 13,19,18,

		15,16,21, 15,21,20,
		16,17,22, 16,22,21,
		17,18,23, 17,23,22,
		18,19,24, 18,24,23,
	};
	//buffer description cube
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_cubeIndexBuffer);
	//---------------------------------------------------------
	D3D11_BUFFER_DESC pyramidBd;
	ZeroMemory(&pyramidBd, sizeof(pyramidBd));
	pyramidBd.Usage = D3D11_USAGE_DEFAULT;
	pyramidBd.ByteWidth = sizeof(WORD) * 18;
	pyramidBd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	pyramidBd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA NewInitData;
	ZeroMemory(&NewInitData, sizeof(NewInitData));
	NewInitData.pSysMem = pyramidIndices;
    hr = _pd3dDevice->CreateBuffer(&pyramidBd, &NewInitData, &_pyramidIndexBuffer);
	//---------------------------------------------------------
	D3D11_BUFFER_DESC gridBd;
	ZeroMemory(&gridBd, sizeof(gridBd));
	gridBd.Usage = D3D11_USAGE_DEFAULT;
	gridBd.ByteWidth = sizeof(WORD) * 96;
	gridBd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	gridBd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA gridIndexData;
	ZeroMemory(&gridIndexData, sizeof(gridIndexData));
	gridIndexData.pSysMem = gridIndices;
	hr = _pd3dDevice->CreateBuffer(&gridBd, &gridIndexData, &_gridIndexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
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
    RECT rc = {0, 0, 640, 480};
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
		D3D_DRIVER_TYPE_UNKNOWN,
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
		D3D_DRIVER_TYPE_NULL,
		D3D_DRIVER_TYPE_SOFTWARE,
		D3D_DRIVER_TYPE_WARP
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

	//PROBABLY THROWING THE ERROR
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }
	//THROWING ERROR HERE ^^^^^
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

	InitVertexBuffer();

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    _pImmediateContext->IASetVertexBuffers(0, 1, &_cubeVertexBuffer, &stride, &offset);
	//_pImmediateContext->IASetVertexBuffers(0, 1, &_pyramidVertexBuffer, &stride, &offset);

	InitIndexBuffer();

    // Set index buffer
    _pImmediateContext->IASetIndexBuffer(_cubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	//_pImmediateContext->IASetIndexBuffer(_pyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_cubeVertexBuffer) _cubeVertexBuffer->Release();
    if (_cubeIndexBuffer) _cubeIndexBuffer->Release();
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
	if ((_rotationValue != _rotationMax)&& _halfRotation == false)
	{
		_rotationValue += 0.5;
	}
	else if (_rotationValue == _rotationMax)
	{
		_halfRotation = true;
	}
	else if ((_halfRotation == true) && _rotationValue!=_rotationMin)
	{
		_rotationValue -= 0.5;
	}
	else if ((_rotationValue == _rotationMin) && _halfRotation == true)
	{
		_halfRotation = false;
	}
	
	if (GetAsyncKeyState(VK_DOWN))
	{
		_pImmediateContext->RSSetState(_solidState);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		_pImmediateContext->RSSetState(_wireFrame);
	}

    //
    // Animate the cube
    //
	XMStoreFloat4x4(&_world, XMMatrixRotationY(t) * XMMatrixTranslation(0, 0, 3));
	XMStoreFloat4x4(&_world2, XMMatrixRotationZ(t) * XMMatrixTranslation(_planetPos.x +_rotationValue,_planetPos.y,_planetPos.z) * XMMatrixRotationY(t));
	XMStoreFloat4x4(&_world3, XMMatrixRotationZ(t) * XMMatrixTranslation(_moonPos.x,_moonPos.y,_moonPos.z) * XMMatrixRotationY(t));
	XMStoreFloat4x4(&_world4,XMMatrixTranslation(0,0,0));

}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);

	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX world2 = XMLoadFloat4x4(&_world2);
	XMMATRIX world3 = XMLoadFloat4x4(&_world3);
	XMMATRIX world4 = XMLoadFloat4x4(&_world4);
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);
    //
    // Update variables
    //
    ConstantBuffer cb;
	world = XMLoadFloat4x4(&_world);
	world2 = XMLoadFloat4x4(&_world2);
	world3 = XMLoadFloat4x4(&_world3);
	world4 = XMLoadFloat4x4(&_world4);
	
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);

	//set the object to render as a pyramid
	
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	_pImmediateContext->IASetVertexBuffers(0, 1, &_gridVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_gridIndexBuffer, DXGI_FORMAT_R16_UINT, 0);




	cb.mWorld = XMMatrixTranspose(world4);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(96, 0, 0);




	_pImmediateContext->IASetVertexBuffers(0, 1, &_pyramidVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(18, 0, 0);
	
	
	//set the next objects to render as cubes
	
	_pImmediateContext->IASetVertexBuffers(0, 1, &_cubeVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_cubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	
	cb.mWorld = XMMatrixTranspose(world2);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(36, 0, 0);

	cb.mWorld = XMMatrixTranspose(world3);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(36, 0, 0);

	///Update code inside init buffer for index and vertex to create multiple index and vertex buffers


    //The
    // Renders a triangle
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	       
	
    //
    // Present our back buffer to our front buffer
    //
   HRESULT hr = _pSwapChain->Present(0, 0);
}