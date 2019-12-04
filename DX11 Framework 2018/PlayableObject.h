#pragma once
#include "GameObject.h"
class PlayableObject : public GameObject
{
	float Velocity = 0.0f;
	float Acceleration = 0.0f;

public:
	PlayableObject(MeshData mesh, Application* app, wchar_t* textureName);

	void Update(float deltaTime, XMMATRIX WorldVector = XMMatrixIdentity());
};

