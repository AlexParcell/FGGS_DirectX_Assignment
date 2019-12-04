#include "FirstPersonCamera.h"
#include "GameObject.h"

FirstPersonCamera::FirstPersonCamera(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: Camera(windowWidth, windowHeight, nearDepth, farDepth)
{

}

void FirstPersonCamera::Update(float deltaTime)
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
		offset += (up * _fCameraSensitivity * deltaTime);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		offset -= (up * _fCameraSensitivity * deltaTime);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		offset += (right * _fCameraSensitivity * deltaTime);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		offset -= (right * _fCameraSensitivity * deltaTime);
	}

	XMStoreFloat3(&_offset, offset);

	XMFLOAT3 target = _pTarget->GetPosition();
	_direction.x = target.x + _offset.x;
	_direction.y = _offset.y;
	_direction.z = target.z + _offset.z;

	Camera::Update(deltaTime);
}