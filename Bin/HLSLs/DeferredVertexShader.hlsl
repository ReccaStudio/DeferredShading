#include "LightHelper.fx"


cbuffer cbPerObject : register( b0 )
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gPostProjRescale;
//	float4x4 gTexTransform;
//	float4x4 gShadowTransform;
//	Material gMaterial;
};


struct VertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
	float2 Tex      : TEXCOORD;
	float3 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH       : SV_POSITION;
	float4 PosW       : POSITION;
	float3 NormalW    : NORMAL;
	float3 TangentW   : TANGENT;
	float2 Tex        : TEXCOORD0;
//	float4 ShadowPosH : TEXCOORD1;
};

VertexOut BasicDeferredVS( VertexIn vin )
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// Output vertex attributes for interpolation across triangle.
	//vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	vout.Tex = vin.Tex;

	// Generate projective tex-coords to project shadow map onto scene.
	//vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);

	return vout;
}