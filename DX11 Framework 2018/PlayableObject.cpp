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

	if (GetAsyncKeyState(0x44)) // A
	{
		_rotation.y += 10.0f * deltaTime;
	}

	MakeForwardVector();
	XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&_forward));

	if (GetAsyncKeyState(0x57)) // W
	{
		pos += forward * 50.0f * deltaTime;
		XMStoreFloat3(&_position, pos);
	}

	GameObject::Update(deltaTime);
}