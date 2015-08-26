#include "stdafx.h"
#include "FXShader.h"
#include "RendererCore.h"
#include "DirectX11Util.h"


FXShader::FXShader()
: m_pFX(0),
m_pTech(0),
m_pfxWorld(0),
m_pfxWorldViewProj(0),
m_pfxViewProj(0),
m_pfxWorldInvTranspose(0),
m_pfxTexTransform(0),
m_pfxShadowTransform(0),
m_pfxEyePosW(0),
m_pfxDirLight(0),
m_pfxPointLight(0),
m_pfxSpotLight(0),
m_pfxMaterial(0),
m_pfxFogStart(0),
m_pfxFogRange(0),
m_pfxFogColor(0),
m_pfxDiffusemap(0),
m_pfxDiffusemapArray(0),
m_pfxCubemap(0),
m_pfxNormalMap(0),
m_pfxShadowMap(0)
{
}


FXShader::~FXShader()
{
	m_vecTech.clear();
	ReleaseCOM(m_pFX);
}

shared_ptr<FXShader> FXShader::Create(const std::wstring& filename)
{
	FXShaderPtr spShader(new FXShader());
	spShader->CreateFX(filename);

	return spShader;
}

bool FXShader::CreateFX(const std::wstring& filename)
{
// 	std::ifstream fin(filename.c_str(), std::ios::binary);
// 
// 	fin.seekg(0, std::ios_base::end);
// 	int size = (int)fin.tellg();
// 	fin.seekg(0, std::ios_base::beg);
// 	std::vector<char> compiledShader(size);
// 
// 	fin.read(&compiledShader[0], size);
// 	fin.close();
// 
//  	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
//  		0, GetD3D11Device(), &m_pFX));

	m_pFX = DirectX11Util::CreateEffectFromMemory( GetD3D11Device(), filename.c_str() );
	if (!m_pFX)
		return false;

	D3DX11_EFFECT_DESC sDesc;
	m_pFX->GetDesc(&sDesc);

	for (UINT i = 0; i < sDesc.Techniques; ++i)
	{
		ID3DX11EffectTechnique* pTech = m_pFX->GetTechniqueByIndex(i);
		if ( pTech)
			m_vecTech.push_back(pTech);
	}

	m_pfxWorld				= m_pFX->GetVariableByName("gWorld")->AsMatrix();
	m_pfxWorldViewProj		= m_pFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_pfxViewProj			= m_pFX->GetVariableByName("gViewProj")->AsMatrix();
	m_pfxWorldInvTranspose = m_pFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_pfxTexTransform		= m_pFX->GetVariableByName("gTexTransform")->AsMatrix();
	m_pfxShadowTransform	= m_pFX->GetVariableByName("gShadowTransform")->AsMatrix();

	m_pfxEyePosW = m_pFX->GetVariableByName("gEyePosW")->AsVector();

	m_pfxDirLight = m_pFX->GetVariableByName("gDirLight");
	m_pfxDirLights = m_pFX->GetVariableByName("gDirLights");
	m_pfxPointLight = m_pFX->GetVariableByName("gPointLight");
	m_pfxSpotLight = m_pFX->GetVariableByName("gSpotLight");
	m_pfxMaterial = m_pFX->GetVariableByName("gMaterial");

	m_pfxFogStart = m_pFX->GetVariableByName("gFogStart")->AsScalar();
	m_pfxFogRange = m_pFX->GetVariableByName("gFogRange")->AsScalar();
	m_pfxFogColor = m_pFX->GetVariableByName("gFogColor")->AsVector();

	m_pfxDiffusemap = m_pFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_pfxDiffusemapArray = m_pFX->GetVariableByName("gDiffuseMapArray")->AsShaderResource();
	m_pfxCubemap = m_pFX->GetVariableByName("gCubeMap")->AsShaderResource();
	m_pfxNormalMap = m_pFX->GetVariableByName("gNormalMap")->AsShaderResource();
	m_pfxShadowMap = m_pFX->GetVariableByName("gShadowMap")->AsShaderResource();

	return true;
}

const string& FXShader::GetTechname() const 
{
	return m_strTechname; 
}

int FXShader::GetTech(const string& strTechName)
{

	vector<ID3DX11EffectTechnique*>::iterator itor = m_vecTech.begin();

	int iIndex = 0;
	for (; itor < m_vecTech.end(); ++itor)
	{
		D3DX11_TECHNIQUE_DESC desc;
		(*itor)->GetDesc(&desc);
		if (strcmp(strTechName.c_str(), desc.Name) == 0)
			return iIndex;
		iIndex++;
	}

	return -1;
}

void FXShader::SetTech(const string& strName) 
{ 
	m_strTechname = strName;
	if (m_pFX)
		m_pTech = m_pFX->GetTechniqueByName(strName.c_str());
}

void FXShader::SetDirLight(DirectionalLight& DirLight)
{
	if (m_pfxDirLight)
		m_pfxDirLight->SetRawValue(&DirLight, 0, sizeof(DirectionalLight));
}


void FXShader::SetDirLights(const DirectionalLight* lights)
{
	if (m_pfxDirLights)
		m_pfxDirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight));
}

void FXShader::SetPointLight(PointLight& PointLight)
{
	if (m_pfxPointLight)
		m_pfxPointLight->SetRawValue(&PointLight, 0, sizeof(PointLight));
}

void FXShader::SetSpotLight(SpotLight& SpotLight)
{
	if (m_pfxSpotLight)
		m_pfxSpotLight->SetRawValue(&SpotLight, 0, sizeof(SpotLight));
}

void FXShader::SetEyePosW(XMFLOAT3& EyePosW)
{
	if (m_pfxEyePosW)
		m_pfxEyePosW->SetRawValue(&EyePosW, 0, sizeof(XMFLOAT3));
}

void FXShader::SetWorld(XMMATRIX& world)
{
	if (m_pfxWorld)
		m_pfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
}

void FXShader::SetWorldViewProj(XMMATRIX& worldViewProj)
{
	if (m_pfxWorldViewProj)
		m_pfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
}

void FXShader::SetViewProj(XMMATRIX& viewProj)
{
	if (m_pfxViewProj)
		m_pfxViewProj->SetMatrix(reinterpret_cast<float*>(&viewProj));

}

void FXShader::SetWorldInvTranspose(XMMATRIX& worldInvTranspose)
{
	if (m_pfxWorldInvTranspose)
		m_pfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&worldInvTranspose));
}

void FXShader::SetTexTransform(XMMATRIX& texTransform)
{
	if (m_pfxTexTransform)
		m_pfxTexTransform->SetMatrix(reinterpret_cast<float*>(&texTransform));
}

void FXShader::SetShadowTransform(XMMATRIX& shadowTransform)
{
	if (m_pfxShadowTransform)
		m_pfxShadowTransform->SetMatrix(reinterpret_cast<float*>(&shadowTransform));
}

void FXShader::SetMaterial(Material& mat)
{
	if (m_pfxMaterial)
		m_pfxMaterial->SetRawValue(&mat, 0, sizeof(Material));
}

void FXShader::SetDiffuseMap(ID3D11ShaderResourceView* pDiffusemap)
{
	if (m_pfxDiffusemap)
		m_pfxDiffusemap->SetResource(pDiffusemap);
}


void FXShader::SetDiffuseMapArray(ID3D11ShaderResourceView* pDiffusemapArray)
{
	if (m_pfxDiffusemapArray)
		m_pfxDiffusemapArray->SetResource(pDiffusemapArray);

}

void FXShader::SetCubeMap(ID3D11ShaderResourceView* pCubemap)
{
	if (m_pfxCubemap)
		m_pfxCubemap->SetResource(pCubemap);

}

void FXShader::SetNormalMap(ID3D11ShaderResourceView* pNormalMap)
{
	if (m_pfxNormalMap)
		m_pfxNormalMap->SetResource(pNormalMap);
}

void FXShader::SetShadowMap(ID3D11ShaderResourceView* pShadowMap)
{
	if (m_pfxShadowMap)
		m_pfxShadowMap->SetResource(pShadowMap);
}

void FXShader::SetFogStart(float fogstart)
{
	if (m_pfxFogStart)
	m_pfxFogStart->SetFloat(fogstart);
}

void FXShader::SetFogRange(float range)
{
	if (m_pfxFogRange)
		m_pfxFogRange->SetFloat(range);
}

void FXShader::SetFogColor(const FXMVECTOR v)
{
	if (m_pfxFogColor)
		m_pfxFogColor->SetFloatVector(reinterpret_cast<const float*>(&v));
}



BuildShadowMapShader::BuildShadowMapShader() : FXShader()
{

}

BuildShadowMapShader::~BuildShadowMapShader()
{

}

shared_ptr<FXShader> BuildShadowMapShader::Create(const wstring& filename)
{
	FXShaderPtr spShader(new BuildShadowMapShader());
	spShader->CreateFX(filename);

	return spShader;
}

bool BuildShadowMapShader::CreateFX(const std::wstring& filename)
{
	if (!FXShader::CreateFX(filename))
		return false;

	m_pfxHeightScale = m_pFX->GetVariableByName("gHeightScale")->AsScalar();
	m_pfxMaxTessDistance = m_pFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	m_pfxMinTessDistance = m_pFX->GetVariableByName("gMinTessDistance")->AsScalar();
	m_pfxMinTessFactor = m_pFX->GetVariableByName("gMinTessFactor")->AsScalar();
	m_pfxMaxTessFactor = m_pFX->GetVariableByName("gMaxTessFactor")->AsScalar();

	return true;
}