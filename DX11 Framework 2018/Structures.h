#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"

using namespace DirectX;

struct LightingData
{
	XMFLOAT3 lightDirection = XMFLOAT3(0.25f, 0.5f, 1.0f); // Light direction from surface (XYZ)
	XMFLOAT4 diffuseMaterial = XMFLOAT4(0.7f, 0.6f, 0.6f, 1.0f); // Diffuse material properties (RGBA)
	XMFLOAT4 diffuseLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // Diffuse light colour (RGBA)
	XMFLOAT4 ambientMaterial = XMFLOAT4(0.2f, 0.3f, 0.3f, 1.0f);
	XMFLOAT4 ambientLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 specularMaterial = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	XMFLOAT4 specularLight = XMFLOAT4(2.0f, 2.0f, 2.05f, 1.0f);
	float specularPower = 10.0f;
	XMFLOAT3 eyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	float gTime;
	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;
	XMFLOAT4 specularMaterial;
	XMFLOAT4 specularLight;
	float specularPower;
	XMFLOAT3 eyePosW;
};