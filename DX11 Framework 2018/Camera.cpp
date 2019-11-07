#include "Camera.h"
#include "Application.h"

void Camera::Update()
{
	XMVECTOR globalUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	forward = XMVector3Normalize(direction - eye);
	right = XMVector3Normalize(XMVector3Cross(globalUp, forward));
	up = XMVector3Normalize(XMVector3Cross(forward, right));

	if (GetAsyncKeyState(0x57)) // W, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		eye += (up * 0.01f);
		direction += (up * 0.01f);
	}

	if (GetAsyncKeyState(0x53)) // S, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		eye -= (up * 0.01f);
		direction -= (up * 0.01f);
	}

	if (GetAsyncKeyState(0x41)) // A, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		eye -= (right * 0.01f);
		direction -= (right * 0.01f);
	}

	if (GetAsyncKeyState(0x44)) // D, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		eye += (right * 0.01f);
		direction += (right * 0.01f);
	}

	if (GetAsyncKeyState(0x50)) // P, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		eye += (forward * 0.01f);
		direction += (forward * 0.01f);
	}

	if (GetAsyncKeyState(0x4F)) // O, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		eye -= (forward * 0.01f);
		direction -= (forward * 0.01f);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		XMFLOAT4 directionVector;

		XMStoreFloat4(&directionVector, direction);
		directionVector.y += 0.01f;

		direction = XMLoadFloat4(&directionVector);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		XMFLOAT4 directionVector;

		XMStoreFloat4(&directionVector, direction);
		directionVector.y -= 0.01f;

		direction = XMLoadFloat4(&directionVector);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		XMFLOAT4 directionVector;

		XMStoreFloat4(&directionVector, direction);
		directionVector.x -= 0.01f;

		direction = XMLoadFloat4(&directionVector);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		XMFLOAT4 directionVector;

		XMStoreFloat4(&directionVector, direction);
		directionVector.x += 0.01f;

		direction = XMLoadFloat4(&directionVector);
	}
}