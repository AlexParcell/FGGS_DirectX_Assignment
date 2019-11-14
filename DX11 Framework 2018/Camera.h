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

enum CameraType
{
	CT_FirstPerson = 0,
	CT_ThirdPerson,
	CT_Path
};

enum PathType
{
	PT_Linear = 0,
	PT_Bezier,
	PT_CatmullRom
};

class Camera
{
	Application* _app;

	XMFLOAT3 _eye = XMFLOAT3(0.0f, 0.0f, -30.0f);
	XMFLOAT3 _direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 _up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 _right = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 _forward = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 _target = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4X4 _viewMatrix;
	XMFLOAT4X4 _projectionMatrix;
	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	CameraType _camType = CT_FirstPerson;
	PathType _pathType = PT_Linear;

	float _fCameraSensitivity = 0.05f;
	float _fInterpTime = 0.0f;
	bool _bActive = true;

	void SetViewMatrix(); // done internally, not a setter
	void UpdateVectors();

public:
	Camera(Application* app)
	{
		_app = app;
		SetViewMatrix();
	}

	Camera(Application* app, CameraType camType)
	{
		_app = app;
		_camType = camType;
		SetViewMatrix();
	}

	Camera(Application* app, CameraType camType, PathType pathType, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	void Update();
	void FirstPersonUpdate();
	void ThirdPersonUpdate();
	void PathUpdate();
	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	XMFLOAT4X4 GetViewMatrix() { return _viewMatrix; }
	XMFLOAT4X4 GetProjectionMatrix() { return _projectionMatrix; }
	XMFLOAT4X4 GetViewProjectionMatrix();

	XMFLOAT3 GetEye() { return _eye; }
	XMFLOAT3 GetDirection() { return _direction; }
	XMFLOAT3 GetUp() { return _up; }
	XMFLOAT3 GetRight() { return _right; }
	XMFLOAT3 GetForward() { return _forward; }

	void SetTargetVector(XMFLOAT3 Target) { _target = Target; }
	void SetCameraType(CameraType type) { _camType = type; }
	void SetPathType(PathType type) { _pathType = type; }
};