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
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;

	// Light direction from surface (XYZ)
	_lightDirection = XMFLOAT3(0.25f, 0.5f, 1.0f);
	// Diffuse material properties (RGBA)
	_diffuseMaterial = XMFLOAT4(0.7f, 0.6f, 0.6f, 1.0f);
	// Diffuse light colour (RGBA)
	_diffuseLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	_ambientLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_ambientMaterial = XMFLOAT4(0.2f, 0.3f, 0.3f, 1.0f);

	_specularMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	_specularLight = XMFLOAT4(2.0f, 2.0f, 2.05f, 1.0f);
	_specularPower = 10.0f;
	_eyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
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

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());

    // Initialize the view matrix
	Eye = XMVectorSet(0.0f, 0.0f, -30.0f, 0.0f);
	At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

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

	CreateDDSTextureFromFile(_pd3dDevice, L"Crate_COLOR", nullptr, &_pTextureRV);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);

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

// To do: figure out how this works source: https://gamedev.stackexchange.com/questions/135943/directx11-texturing-terrain-mesh-with-shared-vertices
HRESULT Application::MakeGrid(int size)
{
	HRESULT hr;

	int dimension = 0;

	if (size < 1)
	{
		dimension = 1;
	}
	else
	{
		dimension = size;
	}

	dimension++;

	int vertexCount = (dimension + 1) * (dimension + 1);
	int actualVertexCount = 0;
	int indexCount = 6 * (dimension * dimension);
	int actualIndexCount = 0;
	PlaneIndices = indexCount;
	SimpleVertex* vertices = new SimpleVertex[vertexCount];
	WORD* indices = new WORD[indexCount];

	for (int z = 0, index = 0; z < dimension; ++z)
	{
		for (int x = 0; x < dimension; ++x, ++index)
		{
			vertices[index].Pos = XMFLOAT3(x, 0.0f, z);
			vertices[index].TexCoord = XMFLOAT2(0.0f, 0.0f);
			actualVertexCount++;
		}
	}

	for (int z = 0, index = 0; z < dimension - 1; ++z)
	{
		for (int x = 0; x < dimension - 1; ++x)
		{
			indices[index++] = z * dimension + x;
			indices[index++] = (z + 1) * dimension + x;
			indices[index++] = (z + 1) * dimension + (x + 1);

			indices[index++] = z * dimension + x;
			indices[index++] = (z + 1) * dimension + (x + 1);
			indices[index++] = z * dimension + (x + 1);
			actualIndexCount += 6;
		}
	}

	CalculateVertexNormals(vertices, actualVertexCount, indices, actualIndexCount);

	// do the index buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

	// do the vertex buffer
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(SimpleVertex) * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	ZeroMemory(&vInitData, sizeof(vInitData));
	vInitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&vbd, &vInitData, &_pPlaneVertexBuffer);

	delete[] vertices;
	delete[] indices;

	if (FAILED(hr))
		return hr;

	float centerOffset = float(dimension / 2) * -1.0f;
	XMMATRIX world = XMMatrixTranslation(centerOffset, -15.0f, centerOffset) * XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMStoreFloat4x4(&_planeWorld, world);

	return hr;
}

void Application::CalculateVertexNormals(SimpleVertex vertices[], int VertexCount, WORD indices[], int indexCount)
{
	/*
	for each vertex,
	go through each 3 indices, find if the vertex is in there
	if it is, get the cross product of the other vertices against this one
	then, normalize it to get it into a unit vector and add it to vectors already done for this vertex
	go through every indice until done
	then, divide the sum we have by the occurances to get the averaged normal
	*/

	for (int i = 0; i < VertexCount; i++)
	{
		XMVECTOR SumVector = { 0.0f, 0.0f, 0.0f };
		int occurances = 0;

		for (int j = 0; j < indexCount; j += 3)
		{
			if (indices[j] == i || indices[j + 1] == i || indices[i + 2] == i)
			{
				XMVECTOR a = XMLoadFloat3(&(vertices[indices[j]].Pos));
				XMVECTOR b = XMLoadFloat3(&vertices[indices[j + 1]].Pos);
				XMVECTOR c = XMLoadFloat3(&vertices[indices[j + 2]].Pos);

				XMVECTOR ac = XMVectorSubtract(c, a);
				XMVECTOR bc = XMVectorSubtract(b, c);
				XMVECTOR polyNormal = XMVector3Cross(ac, bc);
				XMVECTOR unitNormal = XMVector3Normalize(polyNormal);

				SumVector += unitNormal;

				occurances++;
			}
		}

		XMVECTOR FinalNormal = SumVector / occurances;
		XMStoreFloat3(&vertices[i].Normal, FinalNormal);
	}
}

HRESULT Application::MakeCube()
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
		6, 5, 4,
	};

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));

	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(WORD) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitIndexData;
	ZeroMemory(&InitIndexData, sizeof(InitIndexData));
	InitIndexData.pSysMem = indices;
	hr = _pd3dDevice->CreateBuffer(&ibd, &InitIndexData, &_pIndexBuffer);

	if (FAILED(hr))
		return hr;

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	CalculateVertexNormals(vertices, 8, indices, 36);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 8;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::MakePyramid()
{
	HRESULT hr;

	WORD pyramidIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		1, 4, 2,
		2, 4, 3,
	};

	// index buffer

	SimpleVertex pyramidVertices[] =
	{
		{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	CalculateVertexNormals(pyramidVertices, 5, pyramidIndices, 18);

	// vertex buffer

	D3D11_BUFFER_DESC pyramidbd;
	ZeroMemory(&pyramidbd, sizeof(pyramidbd));
	pyramidbd.Usage = D3D11_USAGE_DEFAULT;
	pyramidbd.ByteWidth = sizeof(SimpleVertex) * 5;
	pyramidbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	pyramidbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA PyramidInitData;
	ZeroMemory(&PyramidInitData, sizeof(PyramidInitData));
	PyramidInitData.pSysMem = pyramidVertices;

	hr = _pd3dDevice->CreateBuffer(&pyramidbd, &PyramidInitData, &_pPyramidVertexBuffer);

	if (FAILED(hr))
		return hr;

	// index buffer

	D3D11_BUFFER_DESC pyramidibd;
	ZeroMemory(&pyramidibd, sizeof(pyramidibd));
	pyramidibd.Usage = D3D11_USAGE_DEFAULT;
	pyramidibd.ByteWidth = sizeof(WORD) * 18;
	pyramidibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	pyramidibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA PyramidIndexInitData;
	ZeroMemory(&PyramidIndexInitData, sizeof(PyramidIndexInitData));
	PyramidIndexInitData.pSysMem = pyramidIndices;

	hr = _pd3dDevice->CreateBuffer(&pyramidibd, &PyramidIndexInitData, &_pPyramidIndexBuffer);

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

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

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

	MakePyramid();
	MakeCube();
	MakeGrid(40);

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
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	if (FAILED(hr))
		return hr;

	// Setup Wireframe Mode
	D3D11_RASTERIZER_DESC fmdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_fillMode);

	if (FAILED(hr))
		return hr;

	_pImmediateContext->RSSetState(_fillMode);

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_fillMode) _fillMode->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
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

	float Scalar = 1.0f;

	if (GetAsyncKeyState(VK_DOWN))
	{
		Scalar = -1.0f;
	}

    //
    // Animate the cube
    //
	XMStoreFloat4x4(&_world, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixRotationY(t * Scalar) * XMMatrixTranslation(0.0f, 0.0f, 0.0f));

	XMMATRIX Object1 = XMMatrixTranslation(0.0f, 10.0f, 0.0f) * XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationX(t * Scalar);
	XMMATRIX Object2 = XMMatrixTranslation(7.0f, 0.0f, 0.0f) * XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(t * Scalar) * Object1;
	XMMATRIX Object3 = XMMatrixTranslation(5.0f, 0.0f, 0.0f) * XMMatrixScaling(1.0f, 1.0f, 1.0f) * XMMatrixRotationY(t * Scalar);
	XMMATRIX Object4 = XMMatrixTranslation(0.0f, 10.0f, 0.0f)* XMMatrixScaling(0.5f, 0.5f, 0.5f)* XMMatrixRotationX(t * Scalar) * Object3;

	XMStoreFloat4x4(&_world2, Object1);
	XMStoreFloat4x4(&_world3, Object2);
	XMStoreFloat4x4(&_world4, Object3);
	XMStoreFloat4x4(&_world5, Object4);

	for (int i = 0; i < 100; i++)
	{
		XMMATRIX Asteroid = XMMatrixTranslation(float((rand() % 50) - 25), 0.0f, float((rand() % 50) - 25))
			* XMMatrixScaling(0.2f, 0.2f, 0.2f)
			* XMMatrixRotationY(t * Scalar)
			* Object4;
		XMFLOAT4X4 world;
		XMStoreFloat4x4(&world, Asteroid);

		if (AsteroidsSet)
		{
			asteroids[i] = world;
		}
		else
		{
			asteroids.push_back(world);
		}
	}
	if (!AsteroidsSet)
	{
		AsteroidsSet = true;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		if (Wireframe)
		{
			_pImmediateContext->RSSetState(_fillMode);
		}
		else
		{
			_pImmediateContext->RSSetState(_wireFrame);
		}
		Wireframe = !Wireframe;
	}

	_fTime = t;
	XMStoreFloat3(&_eyePosW, At);
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX world = XMLoadFloat4x4(&_world);
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

	// Set cube index buffer
	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    //
    // Update variables
    //
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
	cb.gTime = _fTime;
	cb.lightDirection = _lightDirection;
	cb.diffuseMaterial = _diffuseMaterial;
	cb.diffuseLight = _diffuseLight;
	cb.ambientMaterial = _ambientMaterial;
	cb.ambientLight = _ambientLight;
	cb.specularMaterial = _specularMaterial;
	cb.specularLight = _specularLight;
	cb.specularPower = _specularPower;
	cb.eyePosW = _eyePosW;

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    //
    // Renders a triangle
    //
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	_pImmediateContext->DrawIndexed(36, 0, 0);

	world = XMLoadFloat4x4(&_world2);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pImmediateContext->DrawIndexed(36, 0, 0);  

	world = XMLoadFloat4x4(&_world3);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pImmediateContext->DrawIndexed(36, 0, 0);

	// Set pyramid vertex buffer

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pPyramidVertexBuffer, &stride, &offset);

	// Set pyramid index buffer
	_pImmediateContext->IASetIndexBuffer(_pPyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_world2);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	world = XMLoadFloat4x4(&_world4);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pImmediateContext->DrawIndexed(18, 0, 0);

	world = XMLoadFloat4x4(&_world5);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pImmediateContext->DrawIndexed(18, 0, 0);

	for (int i = 0; i < asteroids.size(); i++)
	{
		world = XMLoadFloat4x4(&asteroids[i]);
		cb.mWorld = XMMatrixTranspose(world);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		_pImmediateContext->DrawIndexed(18, 0, 0);
	}

	// PLANE

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pPlaneVertexBuffer, &stride, &offset);

	// Set pyramid index buffer
	_pImmediateContext->IASetIndexBuffer(_pPlaneIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	world = XMLoadFloat4x4(&_planeWorld);
	cb.mWorld = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pImmediateContext->DrawIndexed(PlaneIndices, 0, 0);

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}