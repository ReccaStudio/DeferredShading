#include "LightHelper.fx"

// Nonnumeric values cannot be added to a cbuffer.
Texture2D DiffuseRT : register(t0);
//Texture2D gShadowMap;


sampler samPoint : register(s0);

cbuffer cbPerFrame : register(b0)
{
	PointLight PtLight;
	float3 gEyePosW;
	float  pad1;
	Material gMaterial;
};

struct VertexOut
{
	float4 PosH       : SV_POSITION;
	float4 PosW       : POSITION;
	float2 Tex        : TEXCOORD0;
};

float4 DebugPassPS( VertexOut input ) : SV_TARGET
{
	float4 diffuseTex = DiffuseRT.Sample( samPoint, input.Tex );


	return diffuseTex;
}