#pragma once
#include "GameObject.h"
class PlayableObject : public GameObject
{
public:
	PlayableObject(MeshData mesh, Application* app, wchar_t* textureName);

	void Update(float deltaTime, XMMATRIX WorldVector = XMMatrixIdentity());
};

