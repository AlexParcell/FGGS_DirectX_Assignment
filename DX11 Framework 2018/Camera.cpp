#include "Camera.h"
#include "Application.h"
#include "GameObject.h"

Camera::Camera(CameraType camType, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_camType = camType;
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;

	SetViewMatrix();
	Reshape(_windowWidth, _windowHeight, _nearDepth, _farDepth);
}

void Camera::Update(float deltaTime)
{
	_deltaTime = deltaTime;
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
	}

	UpdateVectors();
	SetViewMatrix();
}

// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
void Camera::FirstPersonUpdate()
{
	_eye.x = _pTarget->GetPosition().x;
	_eye.z = _pTarget->GetPosition().z;
	_eye.y = _pTarget->GetPosition().y + 4.0f;

	XMVECTOR up = XMLoadFloat3(&_up);
	XMVECTOR direction = XMLoadFloat3(&_direction);
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMVECTOR offset = XMLoadFloat3(&_offset);

	if (GetAsyncKeyState(VK_UP))
	{
		offset += (up * _fCameraSensitivity * _deltaTime);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		offset -= (up * _fCameraSensitivity * _deltaTime);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		offset += (right * _fCameraSensitivity * _deltaTime);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		offset -= (right * _fCameraSensitivity * _deltaTime);
	}

	XMStoreFloat3(&_offset, offset);

	XMFLOAT3 target = _pTarget->GetPosition();
	_direction.x = target.x + _offset.x;
	_direction.y = _offset.y;
	_direction.z = target.z + _offset.z;
}

void Camera::ThirdPersonUpdate()
{
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMVECTOR forward = XMLoadFloat3(&_forward);
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR up = XMLoadFloat3(&_up);
	XMVECTOR offset = XMLoadFloat3(&_offset);

	if (GetAsyncKeyState(0x50)) // P
	{
		offset += forward * _fCameraSensitivity * _deltaTime;
	}

	if (GetAsyncKeyState(0x4F)) // O
	{
		offset -= forward * _fCameraSensitivity * _deltaTime;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		offset += (up * _fCameraSensitivity * _deltaTime);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		offset -= (up * _fCameraSensitivity * _deltaTime);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		offset -= (right * _fCameraSensitivity * _deltaTime);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		offset += (right * _fCameraSensitivity * _deltaTime);
	}

	XMStoreFloat3(&_offset, offset);

	// Offset our position by the target and our offset from it
	XMFLOAT3 target = _pTarget->GetPosition();
	_eye.x = target.x + _offset.x;
	_eye.y = _offset.y;
	_eye.z = target.z + _offset.z;
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
			viewMatrix = XMMatrixLookAtLH(eye, direction, up);
		}
		break;
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

	// If third person camera, have the camera direction set to look at the object
	if (_camType == CT_ThirdPerson)
	{
		XMVECTOR target = XMLoadFloat3(&_pTarget->GetPosition());

		direction = target;
		XMStoreFloat3(&_direction, direction);
		XMStoreFloat3(&_eye, eye);
	}

	forward = XMVector3Normalize(direction - eye);
	right = XMVector3Normalize(XMVector3Cross(globalUp, forward));
	up = XMVector3Normalize(XMVector3Cross(forward, right));

	XMStoreFloat3(&_right, right);
	XMStoreFloat3(&_forward, forward);
	XMStoreFloat3(&_up, up);
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	XMStoreFloat4x4(&_projectionMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / windowHeight, nearDepth, farDepth));
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