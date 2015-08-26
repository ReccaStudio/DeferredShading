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
	float4x4 gViewProj;
	Material gMaterial;
};

cbuffer cbFixed
{
	//
	// Compute texture coordinates to stretch texture over quad.
	//

	float2 gTexC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2DArray gDiffuseMapArray;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

struct VertexIn
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
};

struct VertexOut
{
	float3 CenterW : POSITION;
	float2 SizeW   : SIZE;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 UV : TEXCOORD;
	uint PrimID : SV_PrimitiveID;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.CenterW = vin.PosW;
	vout.SizeW   = vin.SizeW;

	return vout;
}

[maxvertexcount(4)]
void GS(point VertexOut gin[1],
	uint primID : SV_PrimitiveID,
	inout TriangleStream<GeoOut> triStream)
{
	//
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	//

	float3 up = float3(0.0f, 1.0f, 0.0f);
		float3 look = gEyePosW - gin[0].CenterW;
		look.y = 0.0f; // y-axis aligned, so project to xz-plane
	look = normalize(look);
	float3 right = cross(up, look);

		//
		// Compute triangle strip vertices (quad) in world space.
		//
		float halfWidth = 0.5f*gin[0].SizeW.x;
	float halfHeight = 0.5f*gin[0].SizeW.y;

	float4 v[4];
	v[0] = float4(gin[0].CenterW + halfWidth*right - halfHeight*up, 1.0f);
	v[1] = float4(gin[0].CenterW + halfWidth*right + halfHeight*up, 1.0f);
	v[2] = float4(gin[0].CenterW - halfWidth*right - halfHeight*up, 1.0f);
	v[3] = float4(gin[0].CenterW - halfWidth*right + halfHeight*up, 1.0f);

	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	//
	GeoOut gout;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		gout.PosH = mul(v[i], gViewProj);
		gout.PosW = v[i].xyz;
		gout.NormalW = look;
		gout.UV = gTexC[i];
		gout.PrimID = primID;

		triStream.Append(gout);
	}
}

float4 PS(GeoOut gout, uniform bool gUseTexture, uniform bool gFogEnabled) : SV_Target
{
	float4 albedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (gUseTexture)
	{
		// Sample texture.
		float3 uvw = float3(gout.UV, gout.PrimID % 4);
		albedo = gDiffuseMapArray.Sample(samLinear, uvw);

		clip(albedo.a - 0.05f);
	}

	// Interpolating normal can unnormalize it, so normalize it.
	gout.NormalW = normalize(gout.NormalW);

	float3 toEye = gEyePosW - gout.PosW;
		float distToEye = length(toEye);

	float3 toEyeW = toEye / distToEye;// normalize(gEyePosW - gout.PosW);

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;

	ComputeDirectionalLight(gMaterial, gDirLight, gout.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	ComputePointLight(gMaterial, gPointLight, gout.PosW, gout.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	ComputeSpotLight(gMaterial, gSpotLight, gout.PosW, gout.NormalW, toEyeW, A, D, S);
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


technique11 TreeSpriteTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(CompileShader(gs_5_0, GS()));
		SetPixelShader(CompileShader(ps_5_0, PS(true, true)));
	}
}