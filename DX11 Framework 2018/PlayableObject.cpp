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

	_fAcceleration = 0.0f;

	if (GetAsyncKeyState(0x57)) // W
	{
		_fAcceleration += 1000.0f * deltaTime;
		if (_fVelocity < 5000.0f)
		{
			_fVelocity += _fAcceleration * deltaTime;
		}
		else
		{
			_fVelocity = 5000.0f;
		}
	}

	if (GetAsyncKeyState(0x53)) // S
	{
		_fAcceleration -= 2000.0f * deltaTime;
		if (_fVelocity > 0.0f)
		{
			_fVelocity += _fAcceleration * deltaTime;
		}
		else
		{
			_fVelocity = 0.0f;
		}
	}

	pos += forward * _fVelocity * deltaTime;
	XMStoreFloat3(&_position, pos);

	GameObject::Update(deltaTime);
}