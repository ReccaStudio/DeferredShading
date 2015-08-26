#include "LightHelper.fx"
 

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	float4x4 gShadowTransform;
	Material gMaterial;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gShadowMap;
Texture2D gDiffuseMap;
Texture2D gNormalMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
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
	float4 ShadowPosH : TEXCOORD1;
};

VertexOut BasicDeferredVS( VertexIn vin )
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosW = mul( float4(vin.PosL, 1.0f), gWorld );
	vout.NormalW = mul( vin.NormalL, (float3x3)gWorldInvTranspose );
	vout.TangentW = mul( vin.TangentL, (float3x3)gWorld );

	// Transform to homogeneous clip space.
	vout.PosH = mul( float4(vin.PosL, 1.0f), gWorldViewProj );

	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul( float4(vin.Tex, 0.0f, 1.0f), gTexTransform ).xy;

	// Generate projective tex-coords to project shadow map onto scene.
	vout.ShadowPosH = mul( float4(vin.PosL, 1.0f), gShadowTransform );

	return vout;
}


struct Pixel_MRT_Out
{
	float4 Color0	: COLOR0;
	float4 Color1	: COLOR1;
	float4 Color2	: COLOR2;
	//	float4 Material		: COLOR3;
};


Pixel_MRT_Out BasicDeferredPS( VertexOut input ) : SV_TARGET
{
	Pixel_MRT_Out output = (Pixel_MRT_Out)0;

	//Albeldo
	output.Color0 = gDiffuseMap.Sample( samLinear, input.Tex );

	// Position as depth only in clipping space
	output.Color2 = float4(input.PosW.z / input.PosW.w, 0, 0, 0);

	//
	// Normal mapping
	//
	float3 normalMapSample = gNormalMap.Sample( samLinear, input.Tex ).rgb;
	output.Color1.rgb = NormalSampleToWorldSpace( normalMapSample, input.NormalW, input.TangentW );
	output.Color1.a = 0.0f;


	return output;
}

float4 ShowRenderTarget1_PS( VertexOut input ) : SV_TARGET
{

	
}

technique CreateMRTPerPixelNormal
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, BasicDeferredVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, BasicDeferredPS()) );
	}
};

technique ShowRenderTarget1
{
	pass p0
	{
		SetVertexShader( CompileShader(vs_5_0, BasicDeferredVS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, ShowRenderTarget1_PS()) );
	}
};