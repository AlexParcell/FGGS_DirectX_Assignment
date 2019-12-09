//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

#define LIGHTCOUNT 4
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1

struct Light
{
	float3 Pos; // 12
	bool Enabled; // 4
	float3 Direction; // 12
	int LightType; // 4
	float4 Color; // 12
	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
	float Padding;
};

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float gTime;
	float3 padding;
	float4 DiffuseMtrl;
	float4 AmbientMtrl;
	float4 AmbientLight;
	float4 SpecularMtrl;
	float SpecularPower;
	float3 EyePosW;
	Light Lights[LIGHTCOUNT];
}

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
	float3 Normal : NORMAL0;
	float3 PosW : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct Lighting
{
	float4 diffuse;
	float4 specular;
};

float4 GetSpecular(Light light, float3 ToEye, float3 LightDirection, float3 Normal)
{
	float3 reflection = reflect(mul(LightDirection, -1.0), Normal); // Reflection of light against surface, about normal
	float specularAmount = pow(max(dot(reflection, ToEye), 0.0f), SpecularPower);
	return (specularAmount * (SpecularMtrl * light.Color));
}

float4 GetDiffuse(Light light, float3 LightDirection, float3 Normal)
{
	float diffuseAmount = max(dot(LightDirection, Normal), 0.0f);
	return diffuseAmount * (DiffuseMtrl * light.Color);
}

float4 GetAmbient()
{
	return AmbientLight * AmbientMtrl;
}

// This is a factor that weakens light as distance from it increases
// Constant Attenuation increases or decreases the intensity of the light source independent of distance
// Linear Attenuation decreases the intensity of the light linearly as the distance to the light source increases
// Quadratic Attenuation decreases the intensity of the light quadratically as distance to light increases
float GetAttenuation(Light light, float Distance)
{
	return 1.0f / (light.ConstantAttenuation + (light.LinearAttenuation * Distance) + (light.QuadraticAttenuation * Distance * Distance));
}

// Big directional light, works like the sun
Lighting GetDirectionalLight(Light light, float3 ToEye, float3 WorldPosition, float3 Normal)
{
	float3 LightDirection = light.Direction * -1;

	Lighting result;
	result.diffuse = GetDiffuse(light, LightDirection, Normal);
	result.specular = GetSpecular(light, ToEye, LightDirection, Normal);
	return result;
}

// A little point of light
Lighting GetPointLight(Light light, float3 ToEye, float3 WorldPosition, float3 Normal)
{
	float3 LightDirection = (light.Pos - WorldPosition);
	float Distance = length(LightDirection);
	LightDirection = normalize(LightDirection);
	
	float Attenuation = GetAttenuation(light, Distance);

	Lighting result;
	result.diffuse = GetDiffuse(light, LightDirection, Normal) * Attenuation;
	result.specular = GetSpecular(light, ToEye, LightDirection, Normal) * Attenuation;
	return result;
}

Lighting ProcessLighting(float3 WorldPosition, float3 Normal)
{
	float3 ToEye = normalize(EyePosW - WorldPosition); // Vector from point we want to shade to eye

	Lighting FinalResult;

	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		Lighting Result;

		switch (Lights[i].LightType)
		{
		case POINT_LIGHT:
			Result = GetPointLight(Lights[i], ToEye, WorldPosition, Normal);
			break;
		case DIRECTIONAL_LIGHT:
			Result = GetDirectionalLight(Lights[i], ToEye, WorldPosition, Normal);
			break;
		}

		FinalResult.diffuse += Result.diffuse;
		FinalResult.specular += Result.specular;
	}

	return FinalResult;
}

// Default shaders

VS_OUTPUT VS( float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD0 )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
	
	// Do World, View, Projection Transformation on Position of Vertex
	output.Pos = mul(Pos, World);
	output.PosW = output.Pos.xyz;
    output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// Convert from local space to world space 
	// W component of vector is 0 as vectors cannot be translated
	output.Normal = mul(float4(Normal, 0.0f), World).xyz;

	output.TexCoord = TexCoord;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 Color;

	float3 normal = normalize(input.Normal);

	float4 ambient = GetAmbient();
	Lighting finalLight = ProcessLighting(input.PosW, normal);

	float4 textureColour = txDiffuse.Sample(samLinear, input.TexCoord);

	return float4(textureColour.rgb * (ambient.rgb + finalLight.diffuse.rgb + finalLight.specular.rgb), DiffuseMtrl.a);
}

// SKYBOX SHADERS

VS_OUTPUT SkyboxVS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD0)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, World);
	output.PosW = output.Pos.xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// Convert from local space to world space 
	// W component of vector is 0 as vectors cannot be translated
	output.Normal = mul(float4(Normal, 0.0f), World).xyz;

	output.TexCoord = TexCoord;

	return output;
}

float4 SkyboxPS(VS_OUTPUT input) : SV_Target
{
	float4 color = float4(1.0f / 255.0f, 119.0f / 255.0f, 255.0f / 255.0f, 1.0f);
	float4 white = float4(158.0f / 255.0f, 223.0f / 255.0f, 255.0f / 255.0f, 1.0f);
	return lerp(white, color, clamp(input.PosW.y / 10000 - 0.2, 0, 1)); // divide by the scale (300) to get between 0 and 1, add 0.3 to define gradient start
}

// WATER SHADERS

VS_OUTPUT WaterVS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD0)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	Pos.y += 0.6f * sin(Pos.x) * sin(3.0f * gTime);
	Normal.y += 0.6f * sin(Normal.x) * sin(3.0f * gTime);

	// Modify normals to reflect movement of position

	output.Pos = mul(Pos, World);
	output.PosW = output.Pos.xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// Convert from local space to world space 
	// W component of vector is 0 as vectors cannot be translated
	output.Normal = mul(float4(Normal, 0.0f), World).xyz;

	output.TexCoord = TexCoord;

	return output;
}

float4 WaterPS(VS_OUTPUT input) : SV_Target
{
	float3 normal = normalize(input.Normal);

	float4 ambient = GetAmbient();
	Lighting finalLight = ProcessLighting(input.PosW, normal);

	// This enables tiling (200 tiles per plane) and also does a scrolling effect to make the water look more fluid
	float2 newTexCoord = float2(input.TexCoord.x * 100 + (sin(gTime) * 0.3), input.TexCoord.y * 100 + (sin(gTime) * 0.3));
	float4 textureColour = txDiffuse.Sample(samLinear, newTexCoord);

	return float4(textureColour.rgb * (ambient.rgb + finalLight.diffuse.rgb + finalLight.specular.rgb), DiffuseMtrl.a);
}

// Terrain Shaders

VS_OUTPUT TerrainVS(float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD0)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, World);
	output.PosW = output.Pos.xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// Convert from local space to world space 
	// W component of vector is 0 as vectors cannot be translated
	output.Normal = mul(float4(Normal, 0.0f), World).xyz;

	output.TexCoord = TexCoord;

	return output;
}

float4 TerrainPS(VS_OUTPUT input) : SV_Target
{
	float4 sand = float4(237.0f / 255.0f, 201.0f / 255.0f, 175.0f / 255.0f, 1.0f);
	float4 grass = float4(0.4, 0.7, 0.4, 1.0f);
	float4 rock = float4(0.5, 0.5, 0.5, 0.5);
	float4 snow = float4(1.0, 1.0, 1.0, 1.0);
	float4 color;
	float height = clamp(input.PosW.y / 1000, 0, 1);

	float sandBoundary = 0.05;
	float grassBoundary = 0.47;
	float rockBoundary = 0.995;

	// When calculating the factor, we're basically getting the height to start at 0, and then multiplying it so the max value it can reach in it's range is 1
	if (height < sandBoundary)
	{
		float factor = height * (1 / sandBoundary);
		color = lerp(sand, grass, factor);
	}
	else if (height < grassBoundary)
	{
		color = grass;
	}
	else if (height < rockBoundary)
	{
		color = rock;
	}
	else
	{
		color = snow;
	}

	float3 normal = normalize(input.Normal);
	float4 ambient = GetAmbient();
	Lighting finalLight = ProcessLighting(input.PosW, normal);

	return float4(color.rgb * (ambient.rgb + finalLight.diffuse.rgb + finalLight.specular.rgb), DiffuseMtrl.a);
}