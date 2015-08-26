#pragma once
#include "BaseRenderer.h"


class ForwardRenderer : public BaseRenderer
{
public:
	ForwardRenderer();
	virtual ~ForwardRenderer();

	virtual void Init();
	virtual void Resize();
	virtual void Update( float dt );
	virtual void DrawScene();

protected:
	void DrawScreenQuad();
	void DrawShadowMap();
	void BuildShadowTransform();

protected:
	DirectionalLight		m_DirLights[3];
	PointLight				m_PointLight;
	SpotLight				m_SpotLight;
	XMFLOAT3				m_OriginalLightDir[3];
	float					m_fLightRotationAngle;

	BoundingSphere			m_SceneBounds;

	//static const int		SMapSize = 2048;
	XMFLOAT4X4				m_LightView;
	XMFLOAT4X4				m_LightProj;
	XMFLOAT4X4				m_ShadowTransform;


	DepthStencilTexturePtr	m_spDepthStencilTex;
	BuildShadowMapShaderPtr	m_spShadowMapShader;


};

