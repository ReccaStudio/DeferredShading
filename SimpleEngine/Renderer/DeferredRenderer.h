#pragma once
#include "BaseRenderer.h"
#include "RenderTarget.h"
#include "StaticMesh.h"
#include <d3dx9math.h>

struct cbPerObject
{
	XMFLOAT4X4 gWorld;
	XMFLOAT4X4 gWorldInvTranspose;
	XMFLOAT4X4 gWorldViewProj;
	XMFLOAT4X4 gPostProjRescale;
// 	XMFLOAT4X4 gTexTransform;
// 	XMFLOAT4X4 gShadowTransform;
	//	Material gMaterial;
};

struct cbPerFrame
{
	XMFLOAT4X4 gProjInv;
	PointLight gPointLight;
	XMFLOAT3 EyePosW;
	float	 pad1;
	Material gMaterial;
};

class DeferredRenderer :	public BaseRenderer
{
public:
	DeferredRenderer();
	virtual ~DeferredRenderer();

	virtual void Init();
	virtual void Resize();
	virtual void Update( float dt );
	virtual void DrawScene();

protected:
	void DrawGeometryPass();
	void DrawMRTPass();
	void DrawAmbientLightPass();
	void DrawDirectionLightsPass();
	void DrawPointLightsPass();
	void DrawSpotLightsPass();
	void DrawFinalPass();

	void DrawScreenQuad();
	void DrawDebugStencil();

	void BuildBoxBuffers();
	void BuildGridBuffers();
	void BuildScreenQuadBufferByDebugStencil();
	void BuildSpherebyStencilMask();
	void BuildSpherebyDiffuseLight();
	void BuildQuadBufferByAmbientLight();

	RECT DetermineClipRect( const D3DXVECTOR3& position, const float range );

protected:
	vector<RenderTargetPtr>	m_vecRenderTargets;
	vector<PointLight>		m_vecPointLights;
	
	D3D11_VIEWPORT			m_ViewPort;

	RenderTargetPtr m_spAlbedoRT;
	RenderTargetPtr m_spNormalRT;
	RenderTargetPtr m_spDepthRT;
	RenderTargetPtr m_spLightingRT;

	VertexBufferPtr m_spVB;
	IndexBufferPtr  m_spIB;
	RenderStatesPtr m_spRS;
	VertexBufferPtr m_spScreenVB;
	IndexBufferPtr  m_spScreenIB;
	VertexBufferPtr m_spAmbientLightVB;
	IndexBufferPtr  m_spAmbientLightIB;


	StaticMeshPNTUV m_BoxesMesh;
	StaticMeshPNTUV m_GridMesh;
	StaticMeshPUV	m_DebugStencilQuad;
	StaticMeshPUV	m_AmbientLightQuad;
	StaticMeshPUV m_SphereLight;

	ID3D11InputLayout* m_pDefaultIO;
	ID3D11InputLayout* m_pFinalInputLO;
	ID3D11InputLayout* m_pGeomPassInputLO;

	ID3D11VertexShader* m_pDeferredVS;
	ID3D11PixelShader*	m_pDeferredPS;
	ID3D11VertexShader* m_pGeomPassVS;
	ID3D11VertexShader* m_pAmbientLightVS;

	ID3D11VertexShader*	m_pFinalPassVS;
	ID3D11PixelShader*	m_pFinalPassPS;
	ID3D11PixelShader*	m_pDebugPassPS;
	ID3D11PixelShader*	m_pAmbientLightPS;

	ID3D11Buffer*		m_pCBPerObject;
	ID3D11Buffer*		m_pCBPerFrame;
	ID3D11Buffer*		m_pCBAmbientLight;

	ID3D11SamplerState* m_pPointSampler;
	ID3D11SamplerState* m_pLinearSampler;

	RenderStatesPtr		m_spDebugRS;
	RenderStatesPtr		m_spAmbientLightRS;
	RenderStatesPtr		m_spGeomPassRS;
	TexturePtr			m_spDebugTex;

	DepthStencilTexturePtr	m_spDepthStencilTex;

	//XMMATRIX			m_ViewProj;
};

