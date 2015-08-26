#include "LightHelper.fx"


cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gPostProjRescale;
};


struct VertexIn
{
	float3 PosL     : POSITION;
	float2 Tex      : TEXCOORD;
};

struct VertexOut
{
	float4 PosH       : SV_POSITION;
	float4 PosW       : POSITION;
	float2 Tex        : TEXCOORD0;
};

VertexOut FinalPassVS( VertexIn vin )
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosW = mul( float4(vin.PosL, 1.0f), gWorld );
	float4 posH = mul( float4(vin.PosL, 1.0f), gWorldViewProj );

	vout.PosH = posH;

	vout.Tex = ProjToScreen(posH);
	//vout.Tex = mul( posH, gPostProjRescale );

	return vout;
}

VertexOut GeometryPassVS( VertexIn vin )
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosH = mul( float4(vin.PosL, 1.0f), gWorldViewProj );

	return vout;
}

VertexOut AmbientLightVS( VertexIn vin )
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosH = mul( float4(vin.PosL, 1.0f), gWorldViewProj );
	vout.Tex = vin.Tex;

	return vout;
}