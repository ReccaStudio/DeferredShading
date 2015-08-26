//=============================================================================
// Lighting.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and lights geometry.
//=============================================================================

#include "LightHelper.fx"

cbuffer cbPerFrame
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;

	float3 gEyePosW;

	float gFogStart;
	float gFogRange;
	float4 gFogColor;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 UV	   : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float2 UV	   : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to world space space.
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	//vout.UV = vin.UV;
	vout.UV = mul(float4(vin.UV, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

float4 PS(VertexOut pin, uniform bool gUseTexture, uniform bool gFogEnabled) : SV_Target
{
	float4 albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (gUseTexture)
	{
		albedo = gDiffuseMap.Sample(samAnisotropic, pin.UV);

		//clip(albedo.a - 0.1f);
	}

	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

	float3 toEye = gEyePosW - pin.PosW;
	float distToEye = length(toEye);

	float3 toEyeW = toEye / distToEye;// normalize(gEyePosW - pin.PosW);

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;

	ComputeDirectionalLight(gMaterial, gDirLight, pin.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	ComputePointLight(gMaterial, gPointLight, pin.PosW, pin.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	ComputeSpotLight(gMaterial, gSpotLight, pin.PosW, pin.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	float4 litColor = albedo * (ambient + diffuse) + spec;

	if (gFogEnabled)
	{
		float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
		litColor = lerp(litColor, gFogColor, fogLerp);
	}

	// Common to take alpha from diffuse material.
	litColor.a = gMaterial.Diffuse.a * albedo.a;

	return litColor;
}


technique11 TexLightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS( true, true )));
	}
}

technique11 NoTexLightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(false, true)));
	}
}



