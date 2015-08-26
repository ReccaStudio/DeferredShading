#include "LightHelper.fx"

// Nonnumeric values cannot be added to a cbuffer.
Texture2D DiffuseMap : register( t0 );
Texture2D NormalMap  : register( t1 );
//Texture2D gShadowMap;

//SamplerState samLinear
//{
//	Filter = MIN_MAG_MIP_LINEAR;
//	AddressU = WRAP;
//	AddressV = WRAP;
//};

sampler samLinear : register(s0);

struct VertexOut
{
	float4 PosH       : SV_POSITION;
	float4 PosW       : POSITION;
	float3 NormalW    : NORMAL;
	float3 TangentW   : TANGENT;
	float2 Tex        : TEXCOORD0;
//	float4 ShadowPosH : TEXCOORD1;
};

struct PixelOut
{
	float4 Albedo       : COLOR0;
	float4 Normal	    : COLOR1;
	float4 Depth		: COLOR2;
//	float4 Material		: COLOR3;
};


PixelOut BasicDeferredPS( VertexOut input ) : SV_TARGET
{
	PixelOut output = (PixelOut)0;

	output.Albedo = DiffuseMap.Sample( samLinear, input.Tex );

	// Position as depth only in clipping space
	output.Depth = float4(input.PosH.z / input.PosH.w, 0, 0, 0);

	//
	// Normal mapping
	//
	float3 normalMapSample = NormalMap.Sample(samLinear, input.Tex).rgb;
	output.Normal.rgb = NormalSampleToWorldSpace(normalMapSample, input.NormalW, input.TangentW);
	output.Normal.a = 1.0f;


	return output;
}