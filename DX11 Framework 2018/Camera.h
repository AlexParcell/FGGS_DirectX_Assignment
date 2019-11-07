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

class Camera
{
	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -30.0f, 0.0f);
	XMVECTOR direction = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR right = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR forward = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	Application* app;
public:
	Camera(Application* _app)
	{
		app = _app;
	}

	void Update();

	XMVECTOR GetEye() { return eye; }
	XMVECTOR GetDirection() { return direction; }
	XMVECTOR GetUp() { return up; }
	XMVECTOR GetRight() { return right; }
	XMVECTOR GetForward() { return forward; }
};