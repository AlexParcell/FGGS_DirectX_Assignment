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

	XMVECTOR Position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	XMVECTOR Rotation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	
public:

	GameObject(MeshData _mesh, Application* _app);

	void Draw();
	void Update();

	void SetVertexShader(ID3D11VertexShader* vs) { vertexShader = vs; }
	void SetPixelShader(ID3D11PixelShader* ps) { pixelShader = ps; }

	XMVECTOR GetPosition() { return Position; }
	void SetPosition(XMVECTOR pos) { Position = pos; }
};