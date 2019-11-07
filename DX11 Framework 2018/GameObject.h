#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "DDSTextureLoader.h"
#include "OBJLoader.h"

class Application;

class GameObject
{
public:

	MeshData mesh;
	XMFLOAT4X4 world;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	Application* app;

	XMFLOAT3 Position;
	XMFLOAT3 Scale;
	XMFLOAT3 Rotation;
	
public:

	GameObject(MeshData _mesh, Application* _app)
	{
		mesh = _mesh;
		app = _app;
		XMStoreFloat4x4(&world, XMMatrixIdentity());
	}

	void Draw();
	void Update();

	void SetVertexShader(ID3D11VertexShader* vs) { vertexShader = vs; }
	void SetPixelShader(ID3D11PixelShader* ps) { pixelShader = ps; }
};