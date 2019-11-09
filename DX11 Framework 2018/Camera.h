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
	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -30.0f, 0.0f);
	XMVECTOR direction = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR right = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR forward = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	float CameraSensitivity = 0.05f;

	Application* app;

	float InterpTime = 0.0f;

	CameraType camType = CT_FirstPerson;
	PathType pathType = PT_Linear;

	XMMATRIX ViewMatrix = XMMatrixLookAtLH(eye, direction, up);

	bool Active = true;

	void SetViewMatrix(); // done internally, not a setter

public:
	Camera(Application* _app)
	{
		app = _app;
		SetViewMatrix();
	}

	Camera(Application* _app, CameraType _camType)
	{
		app = _app;
		camType = _camType;
		SetViewMatrix();
	}

	Camera(Application* _app, CameraType _camType, PathType _pathType)
	{
		app = _app;
		camType = _camType;
		pathType = _pathType;
		SetViewMatrix();
	}

	void Update();
	void FirstPersonUpdate();
	void ThirdPersonUpdate();
	void PathUpdate();

	XMMATRIX GetViewMatrix() { return ViewMatrix; }

	XMVECTOR GetEye() { return eye; }
	XMVECTOR GetDirection() { return direction; }
	XMVECTOR GetUp() { return up; }
	XMVECTOR GetRight() { return right; }
	XMVECTOR GetForward() { return forward; }

	void SetTargetVector(XMVECTOR Target) { target = Target; }
	void SetCameraType(CameraType type) { camType = type; }
	void SetPathType(PathType type) { pathType = type; }
};