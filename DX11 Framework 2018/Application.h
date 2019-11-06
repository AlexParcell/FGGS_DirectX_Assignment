#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "DDSTextureLoader.h"

#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace DirectX;
using namespace std;

struct SimpleVertex
{
    XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	float gTime;
	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;
	XMFLOAT4 specularMaterial;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 eyePosW;
};

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
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world, _world2, _world3, _world4, _world5, _planeWorld;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _fillMode;
	bool Wireframe = false;
	vector<XMFLOAT4X4> asteroids;
	bool AsteroidsSet = false;

	ID3D11Buffer* _pPyramidVertexBuffer;
	ID3D11Buffer* _pPyramidIndexBuffer;

	ID3D11Buffer* _pPlaneVertexBuffer;
	ID3D11Buffer* _pPlaneIndexBuffer;
	int PlaneIndices;

	float _fTime = 0.0f;

	XMFLOAT3 _lightDirection;
	XMFLOAT4 _diffuseMaterial;
	XMFLOAT4 _diffuseLight;
	XMFLOAT4 _ambientMaterial;
	XMFLOAT4 _ambientLight;
	XMFLOAT4 _specularMaterial;
	XMFLOAT4 _specularLight;
	float _specularPower;
	XMFLOAT3 _eyePosW;

	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -30.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11SamplerState* _pSamplerLinear = nullptr;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT MakePyramid();
	HRESULT MakeCube();
	HRESULT MakeGrid(int size);

	UINT _WindowHeight;
	UINT _WindowWidth;

	void CalculateVertexNormals(SimpleVertex vertices[], int VertexCount, WORD indices[], int indexCount);

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

