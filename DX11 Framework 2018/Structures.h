#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"

using namespace DirectX;

enum LightType
{
	POINT_LIGHT = 0,
	DIRECTIONAL_LIGHT
};

struct Light
{
	XMFLOAT3 Position = XMFLOAT3(0.0f, 10000.0f, 0.0f);
	bool Enabled = false;
	XMFLOAT3 Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	LightType Type = POINT_LIGHT;
	XMFLOAT3 Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	float Padding;
};

struct LightingData
{
	XMFLOAT3 lightDirection = XMFLOAT3(0.0f, 1.0f, 0.0f); // Light direction from surface (XYZ)

	// Diffuse Light
	XMFLOAT4 diffuseMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); // Diffuse material properties (RGBA)
	XMFLOAT4 diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Diffuse light colour (RGBA)

	// Ambient Light
	XMFLOAT4 ambientMaterial = XMFLOAT4(0.6f, 0.6f, 0.6f, 0.2f); // Ambient material properties (RGBA)
	XMFLOAT4 ambientLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.2f); // Ambient light colour (RGBA)

	// Specular Light
	XMFLOAT4 specularMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f); // Specular material properties (RGBA)
	XMFLOAT4 specularLight = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f); // Specular light colour (RGBA)
	float specularPower = 1.0f; // Intensity of specular lights
	XMFLOAT3 eyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f); // Position of Camera
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
	Light lights[8];
};