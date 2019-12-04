#pragma once
#include "Camera.h"
class FirstPersonCamera : public Camera
{
public:
	FirstPersonCamera(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	void Update(float deltaTime);
};

