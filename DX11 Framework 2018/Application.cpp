#include "Application.h"
#include "PlayableObject.h"
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"

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
	_pLight = new LightingData();
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	srand(time(NULL));

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

	// Make Skybox Vertex Shader
	ID3DBlob* pSkyboxVSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "SkyboxVS", "vs_4_0", &pSkyboxVSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = _pd3dDevice->CreateVertexShader(pSkyboxVSBlob->GetBufferPointer(), pSkyboxVSBlob->GetBufferSize(), nullptr, &_pSkyboxVS);

	if (FAILED(hr))
	{
		pSkyboxVSBlob->Release();
		return hr;
	}

	// Create Skybox Pixel Shader
	ID3DBlob* pSkyboxPSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "SkyboxPS", "ps_4_0", &pSkyboxPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = _pd3dDevice->CreatePixelShader(pSkyboxPSBlob->GetBufferPointer(), pSkyboxPSBlob->GetBufferSize(), nullptr, &_pSkyboxPS);
	pSkyboxPSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Make Water Vertex Shader
	ID3DBlob* pWaterVSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "WaterVS", "vs_4_0", &pWaterVSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = _pd3dDevice->CreateVertexShader(pWaterVSBlob->GetBufferPointer(), pWaterVSBlob->GetBufferSize(), nullptr, &_pWaterVS);

	if (FAILED(hr))
	{
		pWaterVSBlob->Release();
		return hr;
	}

	// Create Skybox Pixel Shader
	ID3DBlob* pWaterPSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "WaterPS", "ps_4_0", &pWaterPSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = _pd3dDevice->CreatePixelShader(pWaterPSBlob->GetBufferPointer(), pWaterPSBlob->GetBufferSize(), nullptr, &_pWaterPS);
	pWaterPSBlob->Release();

	if (FAILED(hr))
		return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

	// Create the sample state
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
    RECT rc = {0, 0, 1600, 900};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

void Application::LoadObjects()
{
	std::fstream jsonfile("Level.json"); // opening file
	nlohmann::json json1; // making JSON object
	jsonfile >> json1; // reading JSON file to object
	jsonfile.close(); // closing file
	nlohmann::json array = json1["Objects"];

	for (int i = 0; i < array.size(); i++)
	{
		// Get texture and mesh names
		std::string TempMeshName = array[i]["Mesh"].get<std::string>().c_str();
		char* MeshName = (char*) (TempMeshName.c_str());
		std::string TempTextName = (array[i]["Texture"].get<std::string>().c_str());
		std::wstring wTempTextName = std::wstring(TempTextName.begin(), TempTextName.end());
		wchar_t* TextureName = (wchar_t*)(wTempTextName.c_str());

		// Get transform
		XMFLOAT3 Position = XMFLOAT3(array[i]["Position"]["x"], array[i]["Position"]["y"], array[i]["Position"]["z"]);
		XMFLOAT3 Scale = XMFLOAT3(array[i]["Scale"]["x"], array[i]["Scale"]["y"], array[i]["Scale"]["z"]);
		XMFLOAT3 Rotation = XMFLOAT3(array[i]["Rotation"]["x"], array[i]["Rotation"]["y"], array[i]["Rotation"]["z"]);

		// Get mesh
		MeshData mesh = OBJLoader::Load(MeshName, _pd3dDevice);

		std::string Tag = array[i]["Tag"].get<std::string>();
		if (Tag == "Skybox")
		{
			Skybox = new GameObject(mesh, this, TextureName);
			Skybox->SetPosition(Position);
			Skybox->SetScale(Scale);
			Skybox->SetRotation(Rotation);
			Skybox->SetVertexShader(_pSkyboxVS);
			Skybox->SetPixelShader(_pSkyboxPS);
		}
		else if (Tag == "Boat")
		{
			Boat = new PlayableObject(mesh, this, TextureName);
			Boat->SetPosition(Position);
			Boat->SetScale(Scale);
			Boat->SetRotation(Rotation);
			Boat->SetVertexShader(_pVertexShader);
			Boat->SetPixelShader(_pPixelShader);
		}
		else if (Tag == "Water")
		{
			Water = new GameObject(mesh, this, TextureName);
			Water->SetPosition(Position);
			Water->SetScale(Scale);
			Water->SetRotation(Rotation);
			Water->SetVertexShader(_pWaterVS);
			Water->SetPixelShader(_pWaterPS);
		}
	}
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

	// Setup Stencil/Depth Description
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

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_pDepthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_pDepthStencilBuffer, nullptr, &_pDepthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _pDepthStencilView);

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

	LoadObjects();

	MeshData terrain = OBJLoader::Load("Terrain.obj", _pd3dDevice);

	Terrain = new GameObject(terrain, this, L"Terrain.dds");
	Terrain->SetPosition(XMFLOAT3(0.0f, -10.0f, 0.0f));
	Terrain->SetScale(XMFLOAT3(100.0f, 100.0f, 100.0f));
	Terrain->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Terrain->SetVertexShader(_pVertexShader);
	Terrain->SetPixelShader(_pPixelShader);

	// Set up cameras
	_pFirstPersonCam = new FirstPersonCamera(_WindowWidth, _WindowHeight, 0.01f, 11000.0f);
	_pFirstPersonCam->SetTarget(Boat);

	_pThirdPersonCam = new ThirdPersonCamera(_WindowWidth, _WindowHeight, 0.01f, 11000.0f);
	_pThirdPersonCam->SetTarget(Boat);
	_pActiveCam = (Camera*) _pThirdPersonCam;

	_pTopDownCam = new Camera(_WindowWidth, _WindowHeight, 0.01f, 500.0f);
	_pTopDownCam->SetDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_pTopDownCam->SetEye(XMFLOAT3(10.0f, 100.0f, 0.0f));

	_pFixedViewCam = new Camera(_WindowWidth, _WindowHeight, 0.01f, 500.0f);
	_pFixedViewCam->SetDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_pFixedViewCam->SetEye(XMFLOAT3(100.0f, 50.0f, 0.0f));

	Light light;
	light.Enabled = true;
	light.Type = DIRECTIONAL_LIGHT;
	light.Direction = XMFLOAT3(0, -1, 0);
	light.Position = XMFLOAT3(1000, 1000, 1000);
	light.Color = XMFLOAT4(0.5, 0.5, 0.5, 1.0);
	lights[0] = light;

	Light light2;
	light2.Enabled = true;
	light2.Type = POINT_LIGHT;
	light2.Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
	light2.Position = XMFLOAT3(0.0f, 5.0f, 0.0f);
	lights[1] = light2;

	Light light3;
	light3.Enabled = true;
	light3.Type = POINT_LIGHT;
	light3.Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	light3.Position = XMFLOAT3(0.0f, 5.0f, 30.0f);
	lights[2] = light3;

	Light light4;
	light4.Enabled = true;
	light4.Type = POINT_LIGHT;
	light4.Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	light4.Position = XMFLOAT3(0.0f, 5.0f, -30.0f);
	lights[3] = light4;

	// Initialize the view matrix
	_view = _pActiveCam->GetViewMatrix();

	// Initialize the projection matrix
	_projection = _pActiveCam->GetProjectionMatrix();
	
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

    if (FAILED(hr))
        return hr;

	// Setup Wireframe Mode
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_pWireFrame);

	if (FAILED(hr))
		return hr;

	// Setup Wireframe Mode
	D3D11_RASTERIZER_DESC fmdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_pFillMode);

	if (FAILED(hr))
		return hr;

	_pImmediateContext->RSSetState(_pFillMode);

    return S_OK;
}

void Application::Cleanup()
{
	delete _pFirstPersonCam;
	delete _pThirdPersonCam;
	
    if (_pImmediateContext) _pImmediateContext->ClearState();

	if (_pDepthStencilView) _pDepthStencilView->Release();
	if (_pDepthStencilBuffer) _pDepthStencilBuffer->Release();
	if (_pWireFrame) _pWireFrame->Release();
	if (_pFillMode) _pFillMode->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount64();

	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;

	float deltaTime = ((dwTimeCur - dwTimeStart) / 1000.0f) - t;

	t = (dwTimeCur - dwTimeStart) / 1000.0f;

    //
    // Animate the cube
    //
	Skybox->SetPosition(_pActiveCam->GetEye());
	Skybox->Update(deltaTime);
	Water->Update(deltaTime);
	Boat->Update(deltaTime);
	Terrain->Update(deltaTime);

	if (GetAsyncKeyState(VK_F1))
	{
		if (_bWireframe)
		{
			_pImmediateContext->RSSetState(_pFillMode);
		}
		else
		{
			_pImmediateContext->RSSetState(_pWireFrame);
		}
		_bWireframe = !_bWireframe;
	}

	if (GetAsyncKeyState(0x31)) // 1
	{
		_pActiveCam = _pFirstPersonCam;
	}

	if (GetAsyncKeyState(0x32)) // 2
	{
		_pActiveCam = _pThirdPersonCam;
	}

	if (GetAsyncKeyState(0x33)) // 1
	{
		_pActiveCam = _pTopDownCam;
	}

	if (GetAsyncKeyState(0x34)) // 2
	{
		_pActiveCam = _pFixedViewCam;
	}

	_pActiveCam->Update(deltaTime);
	_view = _pActiveCam->GetViewMatrix();

	_fTime = t;
	_pLight->eyePosW = _pActiveCam->GetEye();
}

void Application::Draw()
{
    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

    // Update Constant Buffer
    ConstantBuffer cb;
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
	cb.gTime = _fTime;
	cb.diffuseMaterial = _pLight->diffuseMaterial;
	cb.ambientMaterial = _pLight->ambientMaterial;
	cb.ambientLight = _pLight->ambientLight;
	cb.specularMaterial = _pLight->specularMaterial;
	cb.specularPower = _pLight->specularPower;
	cb.eyePosW = _pActiveCam->GetEye();
	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		if (lights[i].Enabled)
			cb.lights[i] = lights[i];
	}

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pCurrentCB = &cb;

    // Render objects

	Skybox->Draw();
	Water->Draw();
	Boat->Draw();
	Terrain->Draw();

    // Present our back buffer to our front buffer
    _pSwapChain->Present(0, 0);
}