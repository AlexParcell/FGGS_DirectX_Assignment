#include "PlayableObject.h"

PlayableObject::PlayableObject(MeshData mesh, Application* app, wchar_t* textureName)
	: GameObject(mesh, app, textureName)
{

}

void PlayableObject::Update(float deltaTime, XMMATRIX WorldVector)
{
	XMVECTOR pos = XMLoadFloat3(&_position);

	if (GetAsyncKeyState(0x41)) // A
	{
		_rotation.y -= 10.0f * deltaTime;
	}

	if (GetAsyncKeyState(0x44)) // D
	{
		_rotation.y += 10.0f * deltaTime;
	}

	MakeForwardVector();
	XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&_forward));

	Acceleration = 0.0f;

	if (GetAsyncKeyState(0x57)) // W
	{
		Acceleration += 1000.0f * deltaTime;
		if (Velocity < 5000.0f)
		{
			Velocity += Acceleration * deltaTime;
		}
		else
		{
			Velocity = 5000.0f;
		}
	}

	if (GetAsyncKeyState(0x53)) // S
	{
		Acceleration -= 2000.0f * deltaTime;
		if (Velocity > 0.0f)
		{
			Velocity += Acceleration * deltaTime;
		}
		else
		{
			Velocity = 0.0f;
		}
	}

	pos += forward * Velocity * deltaTime;
	XMStoreFloat3(&_position, pos);

	GameObject::Update(deltaTime);
}