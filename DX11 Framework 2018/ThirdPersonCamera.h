#pragma once
#include "Camera.h"
class ThirdPersonCamera : public Camera
{
public:
	ThirdPersonCamera(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);

	void Update(float deltaTime);
};

