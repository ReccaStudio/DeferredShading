#include "LightHelper.fx"

// Nonnumeric values cannot be added to a cbuffer.
Texture2D DiffuseRT : register(t0);
Texture2D NormalRT : register(t1);
Texture2D DepthRT  : register(t2);
//Texture2D gShadowMap;


sampler samPoint : register(s0);

cbuffer cbPerFrame : register(b0)
{
	float4x4 gProjInv;
	PointLight gPointLight;
	float3 gEyePosW;
	float  pad1;
	Material gMaterial;
};

float4 gAmbientLight : register(b1);

struct VertexOut
{
	float4 PosH       : SV_POSITION;
	float4 PosW       : POSITION;
	float2 Tex        : TEXCOORD0;
};

float4 FinalPassPS( VertexOut input ) : SV_TARGET
{
	float4 diffuseTex = DiffuseRT.Sample( samPoint, input.Tex );
	float4 normalTex = NormalRT.Sample( samPoint, input.Tex );
	float4 depth = DepthRT.Sample( samPoint, input.Tex );
	
	float3 toEyeW = normalize( gEyePosW - input.PosW.xyz );
	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;

	ComputePointLight( gMaterial, gPointLight, input.PosW.xyz, normalTex.xyz, toEyeW, A, D, S );
	ambient += A;
	diffuse += D;
	spec += S;

	//float3 normal = normalTex.xyz * 2 - 1;
	//float3 albedo = diffuseTex.xyz;
	//float3 emissive = albedo * normalTex.w * 10.0f;
	//normal = normalize( normal );

	float4 litColor = diffuseTex * ( ambient + diffuse ) + spec;

	// Common to take alpha from diffuse material.
	litColor.a = gMaterial.Diffuse.a;
	/**/
	//float4 litColor = gPointLight.Diffuse * diffuseTex;

	return litColor;
}


float4 AmbientLightPS( VertexOut input ) : SV_TARGET
{
	float4 color = DiffuseRT.Sample( samPoint, input.Tex );

	return color*gAmbientLight;
}