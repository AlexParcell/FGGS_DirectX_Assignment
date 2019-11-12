#include "Camera.h"
#include "Application.h"

Camera::Camera(Application* app, CameraType camType, PathType pathType, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_app = app;
	_camType = camType;
	_pathType = pathType;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;

	SetViewMatrix();
	Reshape(_windowWidth, _windowHeight, _nearDepth, _farDepth);
}

void Camera::Update()
{
	UpdateVectors();

	switch (_camType)
	{
		case (CT_FirstPerson):
		{
			FirstPersonUpdate();
		}
		break;
		case (CT_ThirdPerson):
		{
			ThirdPersonUpdate();
		}
		break;
		case (CT_Path):
		{
			PathUpdate();
		}
		break;
	}

	SetViewMatrix();
}

void Camera::FirstPersonUpdate()
{
	XMVECTOR up = XMLoadFloat3(&_up);
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMVECTOR direction = XMLoadFloat3(&_direction);
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR forward = XMLoadFloat3(&_forward);

	if (GetAsyncKeyState(0x57)) // W, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = up * _cameraSensitivity;
		eye += increment;
		direction += increment;
	}

	if (GetAsyncKeyState(0x53)) // S, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = up * _cameraSensitivity;
		eye -= increment;
		direction -= increment;
	}

	if (GetAsyncKeyState(0x41)) // A, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = right * _cameraSensitivity;
		eye -= increment;
		direction -= increment;
	}

	if (GetAsyncKeyState(0x44)) // D, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = right * _cameraSensitivity;
		eye += increment;
		direction += increment;
	}

	if (GetAsyncKeyState(0x50)) // P, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = forward * _cameraSensitivity;
		eye += increment;
		direction += increment;
	}

	if (GetAsyncKeyState(0x4F)) // O, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = forward * _cameraSensitivity;
		eye -= increment;
		direction -= increment;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		direction += (up * _cameraSensitivity);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		direction -= (up * _cameraSensitivity);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		direction -= (right * _cameraSensitivity);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		direction += (right * _cameraSensitivity);
	}

	XMStoreFloat3(&_up, up);
	XMStoreFloat3(&_eye, eye);
	XMStoreFloat3(&_right, right);
	XMStoreFloat3(&_direction, direction);
	XMStoreFloat3(&_forward, forward);
}

void Camera::ThirdPersonUpdate()
{
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMVECTOR forward = XMLoadFloat3(&_forward);
	XMVECTOR right = XMLoadFloat3(&_right);

	if (GetAsyncKeyState(VK_UP))
	{
		eye += (forward * _cameraSensitivity);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		eye -= (forward * _cameraSensitivity);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		eye -= (right * _cameraSensitivity);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		eye += (right * _cameraSensitivity);
	}

	XMStoreFloat3(&_eye, eye);
}

void Camera::PathUpdate()
{
	switch (_pathType)
	{
		case (PT_Linear):
		{
			// TO DO
		}
		break;
		case (PT_CatmullRom):
		{
			// TO DO
		}
		break;
		case (PT_Bezier):
		{
			// TO DO
		}
		break;
	}
}

void Camera::SetViewMatrix()
{
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMVECTOR direction = XMLoadFloat3(&_direction);
	XMVECTOR up = XMLoadFloat3(&_up);
	XMMATRIX viewMatrix;

	switch (_camType)
	{
	case (CT_FirstPerson):
	{
		viewMatrix = XMMatrixLookAtLH(eye, direction, up);
	}
	break;
	case (CT_ThirdPerson):
	{
		viewMatrix = XMMatrixLookToLH(eye, direction, up);
	}
	break;
	case (CT_Path):
	{
		viewMatrix = XMMatrixLookAtLH(eye, direction, up);
	}
	}

	XMStoreFloat4x4(&_viewMatrix, viewMatrix);
}

void Camera::UpdateVectors()
{
	XMVECTOR globalUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMVECTOR forward = XMLoadFloat3(&_forward);
	XMVECTOR direction = XMLoadFloat3(&_direction);
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR up = XMLoadFloat3(&_up);

	forward = XMVector3Normalize(direction - eye);
	right = XMVector3Normalize(XMVector3Cross(globalUp, forward));
	up = XMVector3Normalize(XMVector3Cross(forward, right));

	XMStoreFloat3(&_right, right);
	XMStoreFloat3(&_forward, forward);
	XMStoreFloat3(&_up, up);
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	XMStoreFloat4x4(&_projectionMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / windowHeight, 0.01f, 100.0f));
}

XMFLOAT4X4 Camera::GetViewProjectionMatrix()
{
	XMMATRIX View = XMLoadFloat4x4(&_viewMatrix);
	XMMATRIX Projection = XMLoadFloat4x4(&_projectionMatrix);
	XMMATRIX ViewProjection = View * Projection;
	XMFLOAT4X4 _viewProjection;
	XMStoreFloat4x4(&_viewProjection, ViewProjection);
	return _viewProjection;
}