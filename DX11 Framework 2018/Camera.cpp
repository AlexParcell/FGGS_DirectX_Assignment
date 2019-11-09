#include "Camera.h"
#include "Application.h"

void Camera::Update()
{
	XMVECTOR globalUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	forward = XMVector3Normalize(direction - eye);
	right = XMVector3Normalize(XMVector3Cross(globalUp, forward));
	up = XMVector3Normalize(XMVector3Cross(forward, right));

	switch (camType)
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
	if (GetAsyncKeyState(0x57)) // W, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = up * CameraSensitivity;
		eye += increment;
		direction += increment;
	}

	if (GetAsyncKeyState(0x53)) // S, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = up * CameraSensitivity;
		eye -= increment;
		direction -= increment;
	}

	if (GetAsyncKeyState(0x41)) // A, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = right * CameraSensitivity;
		eye -= increment;
		direction -= increment;
	}

	if (GetAsyncKeyState(0x44)) // D, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = right * CameraSensitivity;
		eye += increment;
		direction += increment;
	}

	if (GetAsyncKeyState(0x50)) // P, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = forward * CameraSensitivity;
		eye += increment;
		direction += increment;
	}

	if (GetAsyncKeyState(0x4F)) // O, https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	{
		XMVECTOR increment = forward * CameraSensitivity;
		eye -= increment;
		direction -= increment;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		direction += (up * CameraSensitivity);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		direction -= (up * CameraSensitivity);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		direction -= (right * CameraSensitivity);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		direction += (right * CameraSensitivity);
	}
}

void Camera::ThirdPersonUpdate()
{
	if (GetAsyncKeyState(VK_UP))
	{
		eye += (forward * CameraSensitivity);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		eye -= (forward * CameraSensitivity);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		eye -= (right * CameraSensitivity);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		eye += (right * CameraSensitivity);
	}
}

void Camera::PathUpdate()
{
	switch (pathType)
	{
		case (PT_Linear):
		{

		}
		break;
		case (PT_CatmullRom):
		{

		}
		break;
		case (PT_Bezier):
		{

		}
		break;
	}
}

void Camera::SetViewMatrix()
{
	switch (camType)
	{
	case (CT_FirstPerson):
	{
		ViewMatrix = XMMatrixLookAtLH(eye, direction, up);
	}
	break;
	case (CT_ThirdPerson):
	{
		ViewMatrix = XMMatrixLookToLH(eye, direction, up);
	}
	break;
	case (CT_Path):
	{
		ViewMatrix = XMMatrixLookAtLH(eye, direction, up);
	}
	}
}