#pragma once
#include "GameObject.h"
class PlayableObject : public GameObject
{
	float _fVelocity = 0.0f;
	float _fAcceleration = 0.0f;

public:
	PlayableObject(MeshData mesh, Application* app, wchar_t* textureName);

	void Update(float deltaTime, XMMATRIX WorldVector = XMMatrixIdentity());
};

