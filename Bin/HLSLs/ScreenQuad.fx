
#include "LightHelper.fx"


cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
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

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


struct VertexIn
{
	float3 PosL    : POSITION;
	float2 UV	   : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 UV	   : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = float4(vin.PosL, 1.0f);

	vout.UV = vin.UV;

	return vout;
}

float4 PS(VertexOut pin, uniform bool gUseTexture) : SV_Target
{
	float4 albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (gUseTexture)
	{
		albedo = gDiffuseMap.Sample(samAnisotropic, pin.UV);

		//clip(albedo.a - 0.1f);
	}

	//float4 litColor = albedo * (gMaterial.Ambient + gMaterial.Diffuse) + gMaterial.Specular;

	// Common to take alpha from diffuse material.
	//litColor.a = gMaterial.Diffuse.a * albedo.a;

	return albedo;
}


technique11 ScreenQuadTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(true)));
	}
}
