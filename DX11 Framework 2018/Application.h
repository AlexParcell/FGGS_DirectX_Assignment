#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"

#include "DDSTextureLoader.h"
#include "OBJLoader.h"
#include "GameObject.h"
#include "Camera.h"
#include "Structures.h"

#include <stdlib.h>
#include <time.h>
#include <vector>

#define GRID_SIZE 5

using namespace DirectX;
using namespace std;

class Application
{
private:
	HINSTANCE               _hInst = nullptr;
	HWND                    _hWnd = nullptr;
	D3D_DRIVER_TYPE         _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       _featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*           _pd3dDevice = nullptr;
	ID3D11DeviceContext*    _pImmediateContext = nullptr;
	IDXGISwapChain*         _pSwapChain = nullptr;
	ID3D11RenderTargetView* _pRenderTargetView = nullptr;
	ID3D11VertexShader*     _pVertexShader = nullptr;
	ID3D11PixelShader*      _pPixelShader = nullptr;
	ID3D11InputLayout*      _pVertexLayout = nullptr;
	ID3D11Buffer*           _pConstantBuffer = nullptr;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView = nullptr;
	ID3D11Texture2D* _depthStencilBuffer = nullptr;
	ID3D11RasterizerState* _wireFrame = nullptr;
	ID3D11RasterizerState* _fillMode = nullptr;
	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11SamplerState* _pSamplerLinear = nullptr;

	Camera* cam = nullptr;
	LightingData* light = nullptr;
	ConstantBuffer* currentCB;

	vector<GameObject*> cubes;

	bool Reverse = false;
	bool Wireframe = false;
	float _fTime = 0.0f;

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

	ConstantBuffer* GetCurrentConstantBuffer() { return currentCB; }
	ID3D11Buffer* GetConstantBuffer() { return _pConstantBuffer; }
	ID3D11DeviceContext* GetImmediateContext() { return _pImmediateContext; }
	float GetTime() { return _fTime; }
};

