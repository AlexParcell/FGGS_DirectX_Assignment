#include "ThirdPersonCamera.h"
#include "GameObject.h"

ThirdPersonCamera::ThirdPersonCamera(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: Camera(windowWidth, windowHeight, nearDepth, farDepth)
{

}

void ThirdPersonCamera::Update(float deltaTime)
{
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMVECTOR forward = XMLoadFloat3(&_forward);
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR up = XMLoadFloat3(&_up);
	XMVECTOR offset = XMLoadFloat3(&_offset);

	if (GetAsyncKeyState(0x50)) // P
	{
		offset += forward * _fCameraSensitivity * deltaTime;
	}

	if (GetAsyncKeyState(0x4F)) // O
	{
		offset -= forward * _fCameraSensitivity * deltaTime;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		offset += (up * _fCameraSensitivity * deltaTime);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		offset -= (up * _fCameraSensitivity * deltaTime);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		offset -= (right * _fCameraSensitivity * deltaTime);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		offset += (right * _fCameraSensitivity * deltaTime);
	}

	XMStoreFloat3(&_offset, offset);

	// Offset our position by the target and our offset from it
	XMFLOAT3 target = _pTarget->GetPosition();
	_eye.x = target.x + _offset.x;
	_eye.y = _offset.y;
	_eye.z = target.z + _offset.z;

	XMVECTOR direction = XMLoadFloat3(&_direction);
	XMVECTOR _target = XMLoadFloat3(&target);
	direction = _target;
	XMStoreFloat3(&_direction, direction);

	Camera::Update(deltaTime);
}