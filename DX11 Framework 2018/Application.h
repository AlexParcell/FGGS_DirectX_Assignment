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
#include "JSON.hpp"

#include <stdlib.h>
#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

#define LIGHTCOUNT 4

using namespace DirectX;
using namespace std;

class PlayableObject;
class FirstPersonCamera;
class ThirdPersonCamera;

class Application
{
private:
	HINSTANCE               _hInst = nullptr;
	HWND                    _hWnd = nullptr;
	D3D_DRIVER_TYPE         _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       _featureLevel = D3D_FEATURE_LEVEL_11_0;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;

	ID3D11Device*           _pd3dDevice = nullptr;
	ID3D11DeviceContext*    _pImmediateContext = nullptr;
	IDXGISwapChain*         _pSwapChain = nullptr;
	ID3D11RenderTargetView* _pRenderTargetView = nullptr;

	// Shaders
	ID3D11VertexShader*     _pVertexShader = nullptr;
	ID3D11PixelShader*      _pPixelShader = nullptr;
	ID3D11VertexShader*		_pSkyboxVS = nullptr;
	ID3D11PixelShader*		_pSkyboxPS = nullptr;
	ID3D11VertexShader*		_pWaterVS = nullptr;
	ID3D11PixelShader*		_pWaterPS = nullptr;
	ID3D11VertexShader*		_pTerrainVS = nullptr;
	ID3D11PixelShader*		_pTerrainPS = nullptr;

	ID3D11InputLayout*      _pVertexLayout = nullptr;
	ID3D11Buffer*           _pConstantBuffer = nullptr;
	ID3D11DepthStencilView* _pDepthStencilView = nullptr;
	ID3D11Texture2D*		_pDepthStencilBuffer = nullptr;
	ID3D11RasterizerState*	_pWireFrame = nullptr;
	ID3D11RasterizerState*	_pFillMode = nullptr;
	ID3D11SamplerState*		_pSamplerLinear = nullptr;

	FirstPersonCamera* _pFirstPersonCam = nullptr;
	ThirdPersonCamera* _pThirdPersonCam = nullptr;
	Camera* _pTopDownCam = nullptr;
	Camera* _pFixedViewCam = nullptr;
	Camera* _pActiveCam = nullptr;

	LightingData* _pLight = nullptr;
	ConstantBuffer* _pCurrentCB;

	GameObject* Skybox = nullptr;
	PlayableObject* Boat = nullptr;
	vector<GameObject*> Objects;

	bool _bWireframe = false;
	float _fTime = 0.0f;

	Light lights[LIGHTCOUNT];

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

	ConstantBuffer* GetCurrentConstantBuffer() { return _pCurrentCB; }
	ID3D11Buffer* GetConstantBuffer() { return _pConstantBuffer; }
	ID3D11DeviceContext* GetImmediateContext() { return _pImmediateContext; }
	ID3D11Device* GetDevice() {	return _pd3dDevice; }
	float GetTime() { return _fTime; }

	void LoadObjects();
	void LoadLights();
	void LoadCameras();

	XMFLOAT4X4* GetViewMatrix() { return &_view; }
};

