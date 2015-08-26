//***************************************************************************************
// LightHelper.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Structures and functions for lighting calculations.
//***************************************************************************************

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
};

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.Direction;

		// Add ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a point light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;

	// The distance from surface to light.
	float d = length(lightVec);

	// Range test.
	if (d > L.Range)
		return;

	// Normalize the light vector.
	lightVec /= d;

	// Ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// Attenuate
	float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));

	diffuse *= att;
	spec *= att;
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a spotlight.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;

		// The distance from surface to light.
		float d = length(lightVec);

	// Range test.
	if (d > L.Range)
		return;

	// Normalize the light vector.
	lightVec /= d;

	// Ambient term.
	ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
			float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}

	// Scale by spotlight factor and attenuate.
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);

	// Scale by spotlight factor and attenuate.
	float att = spot / dot(L.Att, float3(1.0f, d, d*d));

	ambient *= spot;
	diffuse *= att;
	spec *= att;
}


//---------------------------------------------------------------------------------------
// Transforms a normal map sample to world space.
//---------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
	float3 normalT = 2.0f*normalMapSample - 1.0f;

	// Build orthonormal basis.
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N)*N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}


//---------------------------------------------------------------------------------------
// Performs shadowmap test to determine if a pixel is in shadow.
//---------------------------------------------------------------------------------------

static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
						Texture2D shadowMap,
						float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	// Texel size.
	const float dx = SMAP_DX;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += shadowMap.SampleCmpLevelZero(samShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit /= 9.0f;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------


float3 LambertDiffuse(
	float3 lightNormal,
	float3 surfaceNormal,
	float3 lightColor,
	float3 pixelColor
	)
{
	const float INV_PI = 0.31830988618379069122f;

	// compute amount of contribution per light
	float diffuseAmount = saturate( dot( lightNormal, surfaceNormal ) );
	float3 diffuse = (diffuseAmount + 0.1f) * lightColor * pixelColor * INV_PI;
	return diffuse;
}

float SinTheta2( const float val ) { return max( 0.0f, 1.0f - val * val ); }
float SinTheta( const float val ) { return sqrt( SinTheta2( val ) ); }

float3 OrenNayarDiffuse(
	float3 lightNormal,
	float3 viewNormal,
	float3 surfaceNormal,
	float3 lightColor,
	float3 pixelColor,
	float sigma
	)
{
	const float INV_PI = 0.31830988618379069122f;

	sigma = sigma / 180.0f * 3.1415192653f;
	float sigmaSqr = sigma * sigma;
	float A = 1.0f - (sigmaSqr / (2.0f * (sigmaSqr + 0.33f)));
	float B = 0.45f * sigmaSqr / (sigmaSqr + 0.09f);

	float fDotLt = saturate( dot( lightNormal, surfaceNormal ) );
	float fDotVw = saturate( dot( viewNormal, surfaceNormal ) );

	float fSinThetaI = SinTheta( fDotLt );
	float fSinThetaO = SinTheta( fDotVw );

	float fMaxCos = saturate( dot( normalize( lightNormal - surfaceNormal * fDotLt ), normalize( viewNormal - surfaceNormal * fDotVw ) ) );

	float fSinAlpha, fTanBeta;
	if ( abs( fDotLt ) > abs( fDotVw ) )
	{
		fSinAlpha = fSinThetaO;
		fTanBeta = fSinThetaI / clamp( abs( fDotLt ), 0.01f, 1.0f );
	}
	else
	{
		fSinAlpha = fSinThetaI;
		fTanBeta = fSinThetaO / clamp( abs( fDotVw ), 0.01f, 1.0f );
	}

	return pixelColor * lightColor * INV_PI * (A + B * fMaxCos * fSinAlpha * fTanBeta) * (fDotLt + 0.1f);
}

float3 BlinnPhoneSpecular(
	float3 toEye,
	float3 lightNormal,
	float3 surfaceNormal,
	float3 materialSpecularColor,
	float materialSpecularPower,
	float lightSpecularIntensity,
	float3 lightColor
	)
{
	// compute specular contribution
	float3 vHalf = normalize( lightNormal + toEye );
	float specularAmount = saturate( dot( surfaceNormal, vHalf ) );
	specularAmount = pow( specularAmount, max( materialSpecularPower, 0.0001f ) ) * lightSpecularIntensity;
	float3 specular = materialSpecularColor * lightColor * specularAmount;

	return specular;
}

float3 DepthToPosition( float iDepth, float4 iPosProj, matrix mProjInv, float fFarClip )
{
	float3 vPosView = mul( iPosProj, mProjInv ).xyz;
	float3 vViewRay = float3(vPosView.xy * (fFarClip / vPosView.z), fFarClip);
	float3 vPosition = vViewRay * iDepth;

	return vPosition;
}

float2 ProjToScreen( float4 iCoord )
{
	float2 oCoord = iCoord.xy / iCoord.w;
	return 0.5f * (float2(oCoord.x, -oCoord.y) + 1);
}

float4 ScreenToProj( float2 iCoord )
{
	return float4(2.0f * float2(iCoord.x, 1.0f - iCoord.y) - 1, 0.0f, 1.0f);
}

void PerformFragmentAnalysis4( in uint4 inputCoverage[1], out uint sampleCount, out uint fragmentCount, out uint4 sampleWeights[1] )
{
	uint4 coverage = inputCoverage[0];

		sampleWeights[0] = uint4(1, 1, 1, 1);

	// Kill the same primitive IDs in the pixel
	if ( coverage.x == coverage.y ) { coverage.y = 0; sampleWeights[0].x += 1; sampleWeights[0].y = 0; }
	if ( coverage.x == coverage.z ) { coverage.z = 0; sampleWeights[0].x += 1; sampleWeights[0].z = 0; }
	if ( coverage.x == coverage.w ) { coverage.w = 0; sampleWeights[0].x += 1; sampleWeights[0].w = 0; }
	if ( coverage.y == coverage.z ) { coverage.z = 0; sampleWeights[0].y += 1; sampleWeights[0].z = 0; }
	if ( coverage.y == coverage.w ) { coverage.w = 0; sampleWeights[0].y += 1; sampleWeights[0].w = 0; }
	if ( coverage.z == coverage.w ) { coverage.w = 0; sampleWeights[0].z += 1; sampleWeights[0].w = 0; }

	sampleWeights[0].x = coverage.x > 0 ? sampleWeights[0].x : 0;
	sampleWeights[0].y = coverage.y > 0 ? sampleWeights[0].y : 0;
	sampleWeights[0].z = coverage.z > 0 ? sampleWeights[0].z : 0;
	sampleWeights[0].w = coverage.w > 0 ? sampleWeights[0].w : 0;

	sampleCount = (sampleWeights[0].x > 0 ? 1 : 0) + (sampleWeights[0].y > 0 ? 1 : 0) + (sampleWeights[0].z > 0 ? 1 : 0) + (sampleWeights[0].w > 0 ? 1 : 0);
	fragmentCount = sampleWeights[0].x + sampleWeights[0].y + sampleWeights[0].z + sampleWeights[0].w;
}
