//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register (s0);

struct VS_INPUT
{
	//Vertex shader input
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL0;
	float3 PosW : POSITION0;
	float2 Tex : TEXCOORD0;
};

cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

	float4 DiffuseMtrl;
	float4 DiffuseLight;
	float3 LightVecW;
	float  gTime;

	float4 AmbientMaterial;
	float4 AmbientLight;
	float4 SpecularMtrl;

	float4 SpecularLight;
	float  SpecularPower;
	float3 EyePosW;
	float4 pad1;
	float4 pad2;
}

//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_INPUT VS( float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0)
{

    VS_INPUT output = (VS_INPUT)0;
	//Apply world transformations
    output.Pos = mul( Pos, World );

	output.Pos.y += cos((-gTime*1.5) + (Pos.x) + Pos.z * 10) * 2;
	output.Pos.x += cos((gTime * 1.5) + (Pos.x) + Pos.z * 10) * 2;
	output.Pos.z -= cos((gTime * 1.5) + (Pos.x) + Pos.z * 10) * 2;

	float3 toEye = normalize(EyePosW - output.Pos.xyz); //compute the vector from vertex eye to position (for lighting purposes):
	output.PosW = toEye;

	output.Pos = mul(output.Pos, View); //Apply view and projection transformations
	output.Pos = mul(output.Pos, Projection);

	float3 normalW = mul(float4(NormalL, 0.0f), World).xyz; //w Component of vector is 0 as vectors cannot be translated
	output.Norm = normalW;
	output.Tex = Tex;
	
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_INPUT input ) : SV_Target
{
	float3 normalW = normalize(input.Norm);

	//Compute reflection Vector
	float3 r = reflect(-LightVecW, normalW);

	float specularAmount = pow(max(dot(r, input.PosW), 0.0f), SpecularPower); //Calculate the specular light amount that makes it into the eye
	float diffuseAmount = max(dot(LightVecW, normalW), 0.0f); //Calculate the diffuse light amount
	float diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
	float3 ambient = AmbientMaterial * AmbientLight;

	float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rgb;
	float4 finalColour;
	float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);

	
	finalColour.rgb = textureColour + ambient + diffuse + specular; //sum up all terms and set alpha to diffuse material alpha
	finalColour.a = DiffuseMtrl.a;

    return finalColour;
}
