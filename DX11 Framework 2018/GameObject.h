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

	bool HasSpecular = false;

	Application* app;

	bool IsChild = false;

	XMFLOAT3 Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

public:

	GameObject(MeshData _mesh, Application* _app, wchar_t* textureName);
	~GameObject();

	void Draw();
	void Update(XMMATRIX WorldVector = XMMatrixIdentity());

	void SetVertexShader(ID3D11VertexShader* vs) { vertexShader = vs; }
	void SetPixelShader(ID3D11PixelShader* ps) { pixelShader = ps; }

	void SetChild(GameObject* child) { Child = child; }
	void SetIsChild(bool val) { IsChild = val; }
	void SetHasSpec(bool value) { HasSpecular = value; }

	XMFLOAT3 GetPosition() { return Position; }
	void SetPosition(XMFLOAT3 pos) { Position = pos; }
	XMFLOAT3 GetScale() { return Scale; }
	void SetScale(XMFLOAT3 scale) { Scale = scale; }
	XMFLOAT3 GetRotation() { return Rotation; }
	void SetRotation(XMFLOAT3 rotation) { Rotation = rotation; }
};