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
class GameObject;

class Camera
{
protected:
	XMFLOAT3 _eye = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 _direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 _up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 _right = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 _forward = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 _offset = XMFLOAT3(-30.0f, 0.0f, 0.0f);

	GameObject* _pTarget = nullptr;
	XMFLOAT4X4 _viewMatrix;
	XMFLOAT4X4 _projectionMatrix;
	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	float _fCameraSensitivity = 30.0f;
	bool _bActive = true;

	void SetViewMatrix(); // done internally, not a setter
	void UpdateVectors();

public:
	Camera(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	virtual void Update(float deltaTime);
	void FirstPersonUpdate();
	void ThirdPersonUpdate();
	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	XMFLOAT4X4 GetViewMatrix() { return _viewMatrix; }
	XMFLOAT4X4 GetProjectionMatrix() { return _projectionMatrix; }
	XMFLOAT4X4 GetViewProjectionMatrix();

	XMFLOAT3 GetEye() { return _eye; }
	XMFLOAT3 GetDirection() { return _direction; }
	XMFLOAT3 GetUp() { return _up; }
	XMFLOAT3 GetRight() { return _right; }
	XMFLOAT3 GetForward() { return _forward; }

	void SetTarget(GameObject* Target) { _pTarget = Target; }
	void SetEye(XMFLOAT3 eye) { _eye = eye; }
	void SetDirection(XMFLOAT3 direction) { _direction = direction; }
};