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
	MeshData _mesh;
	XMFLOAT4X4 _world;
	XMFLOAT3 _position = XMFLOAT3(0.0f, -5.0f, 0.0f);
	XMFLOAT3 _scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 _rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 _forward = XMFLOAT3(1.0f, 0.0f, 0.0f);

	Application* _pApp;
	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader* _pPixelShader;
	GameObject* _pChild = nullptr;
	ID3D11ShaderResourceView* _pTexture = nullptr;

	bool _bIsChild = false;

public:
	GameObject(MeshData mesh, Application* app, wchar_t* textureName);
	~GameObject();

	void Draw();
	virtual void Update(float deltaTime, XMMATRIX WorldVector = XMMatrixIdentity());

	void SetVertexShader(ID3D11VertexShader* vs) { _pVertexShader = vs; }
	void SetPixelShader(ID3D11PixelShader* ps) { _pPixelShader = ps; }

	void SetChild(GameObject* child) { _pChild = child; }
	void SetIsChild(bool val) { _bIsChild = val; }

	MeshData GetMeshData() { return _mesh; }

	XMFLOAT3 GetPosition() { return _position; }
	void SetPosition(XMFLOAT3 pos) { _position = pos; }
	XMFLOAT3 GetScale() { return _scale; }
	void SetScale(XMFLOAT3 scale) { _scale = scale; }
	XMFLOAT3 GetRotation() { return _rotation; }
	void SetRotation(XMFLOAT3 rotation) { _rotation = rotation; }

	void MakeForwardVector();
};