//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float gTime;
	float3 LightVecW;
	float4 DiffuseMtrl;
	float4 DiffuseLight;
	float4 AmbientMtrl;
	float4 AmbientLight;
	float4 SpecularMtrl;
	float4 SpecularLight;
	float SpecularPower;
	float3 EyePosW;
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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float4 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord : TEXCOORD0 )
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


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 Color;

	float3 Normal = normalize(input.Normal);

	float3 toEye = normalize(EyePosW - input.PosW);

	float3 reflection = reflect(mul(LightVecW, -1.0), Normal);

	// Compute Colour using Diffuse lighting only
	float diffuseAmount = max(dot(LightVecW, Normal), 0.0f);

	float specularAmount = pow(max(dot(reflection, toEye), 0.0f), SpecularPower);

	float4 ambient = (AmbientLight * AmbientMtrl);
	float4 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight);
	float4 specular = (specularAmount * (SpecularMtrl * SpecularLight));

	float4 textureColour = txDiffuse.Sample(samLinear, input.TexCoord);

	Color.rgb = textureColour.rgb * (ambient.rgb + diffuse.rgb + specular.rgb);
	Color.a = DiffuseMtrl.a;

	return Color;
}
