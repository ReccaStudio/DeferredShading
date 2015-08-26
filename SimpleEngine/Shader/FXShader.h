#pragma once

class FXShader
{
public:
	FXShader();
	virtual ~FXShader();

	static shared_ptr<FXShader> Create(const std::wstring& filename);

	virtual bool CreateFX(const std::wstring& filename);

	const string& GetTechname() const;

	int	GetTech(const string& strTechName);
	ID3DX11EffectTechnique* GetTech(int iIndex) { 
		if (iIndex < 0)
			return NULL;

		return m_vecTech[iIndex];
	}

	ID3DX11Effect* GetFX() const { return m_pFX; }
	ID3DX11EffectTechnique* GetTech() const { return m_pTech; }
	ID3DX11EffectMatrixVariable* GetWorld() const { return m_pfxWorld; }
	ID3DX11EffectMatrixVariable* GetWorldViewProj() const { return m_pfxWorldViewProj; }
	ID3DX11EffectMatrixVariable* GetWViewProj() const { return m_pfxViewProj; }
	ID3DX11EffectMatrixVariable* GetWorldInvTranspose() const { return m_pfxWorldInvTranspose; }
	ID3DX11EffectMatrixVariable* GetTexTransform() const { return m_pfxTexTransform; }
	ID3DX11EffectMatrixVariable* GetShadowTransform() const { return m_pfxShadowTransform; }
	ID3DX11EffectVectorVariable* GetEyePosW() const { return m_pfxEyePosW; }
	ID3DX11EffectVariable* GetDirLight() const { return m_pfxDirLight; }
	ID3DX11EffectVariable* GetDirLights() const { return m_pfxDirLights; }
	ID3DX11EffectVariable* GetPointLight() const { return m_pfxPointLight; }
	ID3DX11EffectVariable* GetSpotLight() const { return m_pfxSpotLight; }
	ID3DX11EffectVariable* GetMaterial() const { return m_pfxMaterial; }
	ID3DX11EffectVariable* GetFogStart() const { return m_pfxFogStart; }
	ID3DX11EffectVariable* GetFogRange() const { return m_pfxFogRange; }
	ID3DX11EffectVariable* GetFogColor() const { return m_pfxFogColor; }
	ID3DX11EffectShaderResourceVariable* GetDiffusemap() const { return m_pfxDiffusemap; }
	ID3DX11EffectShaderResourceVariable* GetDiffusemapArray() const { return m_pfxDiffusemapArray; }
	ID3DX11EffectShaderResourceVariable* GetCubemap() const { return m_pfxCubemap; }
	ID3DX11EffectShaderResourceVariable* GetNormalMap() const { return m_pfxNormalMap; }
	ID3DX11EffectShaderResourceVariable* GetShadowMap() const { return m_pfxShadowMap; }


	void SetTech(const string& strName);
	void SetDirLight(DirectionalLight& DirLight);
	void SetDirLights(const DirectionalLight* lights);
	void SetPointLight(PointLight& PointLight);
	void SetSpotLight(SpotLight& SpotLight);
	void SetEyePosW(XMFLOAT3& EyePosW);
	void SetWorld(XMMATRIX& world);
	void SetWorldViewProj(XMMATRIX& worldViewProj);
	void SetViewProj(XMMATRIX& viewProj);
	void SetWorldInvTranspose(XMMATRIX& worldInvTranspose);
	void SetTexTransform(XMMATRIX& texTransform);
	void SetShadowTransform(XMMATRIX& shadowTransform);
	void SetMaterial(Material& mat);

	void SetDiffuseMap(ID3D11ShaderResourceView* pDiffusemap);
	void SetDiffuseMapArray(ID3D11ShaderResourceView* pDiffusemapArray);
	void SetCubeMap(ID3D11ShaderResourceView* pCubemap);
	void SetNormalMap(ID3D11ShaderResourceView* pNormalMap);
	void SetShadowMap(ID3D11ShaderResourceView* pShadowMap);

	void SetFogStart(float fogstart);
	void SetFogRange(float range);
	void SetFogColor(const FXMVECTOR v);
protected:
	string	m_strTechname;

	vector<ID3DX11EffectTechnique*> m_vecTech;

	ID3DX11Effect* m_pFX;
	ID3DX11EffectTechnique* m_pTech;
	ID3DX11EffectMatrixVariable* m_pfxWorld;
	ID3DX11EffectMatrixVariable* m_pfxWorldViewProj;
	ID3DX11EffectMatrixVariable* m_pfxViewProj;
	ID3DX11EffectMatrixVariable* m_pfxWorldInvTranspose;
	ID3DX11EffectMatrixVariable* m_pfxTexTransform;
	ID3DX11EffectMatrixVariable* m_pfxShadowTransform;

	ID3DX11EffectVectorVariable* m_pfxEyePosW;
	ID3DX11EffectVariable* m_pfxDirLight;
	ID3DX11EffectVariable* m_pfxDirLights;
	ID3DX11EffectVariable* m_pfxPointLight;
	ID3DX11EffectVariable* m_pfxSpotLight;
	ID3DX11EffectVariable* m_pfxMaterial;

	ID3DX11EffectScalarVariable* m_pfxFogStart;
	ID3DX11EffectScalarVariable* m_pfxFogRange;
	ID3DX11EffectVectorVariable* m_pfxFogColor;


	ID3DX11EffectShaderResourceVariable* m_pfxDiffusemap;
	ID3DX11EffectShaderResourceVariable* m_pfxDiffusemapArray;
	ID3DX11EffectShaderResourceVariable* m_pfxCubemap;
	ID3DX11EffectShaderResourceVariable* m_pfxNormalMap;
	ID3DX11EffectShaderResourceVariable* m_pfxShadowMap;

};

class BuildShadowMapShader : public FXShader
{
public:
	BuildShadowMapShader();
	virtual ~BuildShadowMapShader();

	static shared_ptr<FXShader> Create(const std::wstring& filename);

	virtual bool CreateFX(const std::wstring& filename);

	void SetHeightScale(float f)                        { m_pfxHeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { m_pfxMaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { m_pfxMinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { m_pfxMinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { m_pfxMaxTessFactor->SetFloat(f); }

protected:
	ID3DX11EffectScalarVariable* m_pfxHeightScale;
	ID3DX11EffectScalarVariable* m_pfxMaxTessDistance;
	ID3DX11EffectScalarVariable* m_pfxMinTessDistance;
	ID3DX11EffectScalarVariable* m_pfxMinTessFactor;
	ID3DX11EffectScalarVariable* m_pfxMaxTessFactor;
};

typedef shared_ptr<FXShader> FXShaderPtr;
typedef shared_ptr<BuildShadowMapShader> BuildShadowMapShaderPtr;