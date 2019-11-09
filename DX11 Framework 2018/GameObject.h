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

	GameObject* Child = nullptr;

	ID3D11ShaderResourceView* Texture = nullptr;
	ID3D11ShaderResourceView* specMap = nullptr;

	Application* app;

	bool IsChild = false;

	XMVECTOR Position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
	XMVECTOR Rotation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

public:

	GameObject(MeshData _mesh, Application* _app, wchar_t* textureName);
	~GameObject();

	void Draw();
	void Update(XMMATRIX WorldVector = XMMatrixIdentity());

	void SetVertexShader(ID3D11VertexShader* vs) { vertexShader = vs; }
	void SetPixelShader(ID3D11PixelShader* ps) { pixelShader = ps; }

	void SetChild(GameObject* child) { Child = child; }
	void SetIsChild(bool val) { IsChild = val; }

	XMVECTOR GetPosition() { return Position; }
	void SetPosition(XMVECTOR pos) { Position = pos; }
	XMVECTOR GetScale() { return Scale; }
	void SetScale(XMVECTOR scale) { Scale = scale; }
	XMVECTOR GetRotation() { return Rotation; }
	void SetRotation(XMVECTOR rotation) { Rotation = rotation; }
};