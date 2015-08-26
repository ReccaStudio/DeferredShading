#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "FXShader.h"
#include "VertexDefine.h"
#include "Texture.h"
#include "RenderStates.h"
#include "Singleton.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "DrawElement.h"
#include "RenderTarget.h"
#include "BaseRenderer.h"

enum ERendererType
{
	ERDR_Forward, 
	ERDR_Deferred,
};

class RendererCore 
{
public:
	RendererCore();
	virtual ~RendererCore();

	typedef unordered_map<string, VertexBufferPtr>	SharedVertexBufferMap;
	typedef unordered_map<string, IndexBufferPtr>	SharedIndexBufferMap;
	typedef unordered_map<string, VertexDeclarationPtr>		SharedVDeclMap;
	typedef unordered_map<string, TexturePtr>		SharedTextureMap;
	typedef unordered_map<string, RenderStatesPtr>	SharedRenderStateMap;
	typedef unordered_map<string, FXShaderPtr>		SharedShaderMap;
	typedef unordered_map<string, BitVDeclPtr>		SharedBitDeclMap;
	typedef unordered_map<string, RenderTargetPtr>	SharedRenderTargetMap;


	static void Create() {
		if (!m_pInst)
			m_pInst = new RendererCore;
	}

	static void Destroy() {
		SafeDelete(m_pInst);
	}
	static RendererCore* Instance(){
		return m_pInst;
	}

	bool Init(HINSTANCE hInst, HWND wMainWnd);
	void Resize();
	bool InitRenderer();

	virtual void UpdateScene(float dt);
	virtual void DrawScene();

	void AddDrawElem(DrawElement* pElem) {
		m_pRenderer->AddDrawElem(pElem);
	}
	void AddDrawScreenElem(DrawElement* pElem) {
		m_pRenderer->AddDrawScreenElem( pElem );
	}
	void AddDrawShadowElem(DrawElement* pElem) {
		m_pRenderer->AddDrawShadowElem( pElem );
	}
	void AddStencilElem( DrawElement* pElem ) {
		m_pRenderer->AddStencilElem( pElem );
	}

	ID3D11Device*			GetDevice() const { return m_pd3dDevice; }
	ID3D11DeviceContext*	GetDeviceImmContext() const { return m_pd3dImmediateContext; }
	ID3D11Texture2D*		GetDSTexture2D() const { return m_pDepthStencilTex2D; }
	ID3D11RenderTargetView* GetRTView() const { return m_pRenderTargetView;  }
	ID3D11DepthStencilView* GetDSView() const { return m_pDepthStencilView; }
	ID3D11ShaderResourceView* GetDSSRV() const { return m_pDepthStencilSRV;  }


	int ClientWidth() const { return m_iClientWidth; }
	void ClientWidth(int val) { m_iClientWidth = val; }
	int ClientHeight() const { return m_iClientHeight; }
	void ClientHeight(int val) { m_iClientHeight = val; }

	void SetView(const XMMATRIX& view) {
		XMStoreFloat4x4(&m_View, view);
	}
	void SetProj( const XMMATRIX& proj ) {
		XMStoreFloat4x4( &m_Proj, proj );
	}
	void SetEyePosW( XMFLOAT3 pos ) {
		m_EyePosW = pos;
	}

	const XMFLOAT4X4* GetView() const { return &m_View; }
	const XMFLOAT4X4* GetProj() const { return &m_Proj; }
	XMFLOAT3 GetEyePosW() const { return m_EyePosW; }
	ERendererType GetRDRType() const { return m_eRendererType; }

	void* MapbyVB(VertexBufferPtr spVB);
	void UnmapbyVB(VertexBufferPtr spVB);
// 	void SetPointLightPos(XMFLOAT3& PointLitPos);
// 	void SetSpotLightPos(XMFLOAT3 pos, XMVECTOR dir);

	VertexBufferPtr GetVB(const string& strName);
	bool InsertVB(const string& strName, VertexBufferPtr spVB);

	IndexBufferPtr GetIB(const string& strName);
	bool InsertIB(const string& strName, IndexBufferPtr spIB);

	VertexDeclarationPtr GetVDecl(const string& strName);
	bool InsertVDecl(const string& strName, VertexDeclarationPtr spVDecl);

	BitVDeclPtr GetBitVDecl(const string& strName);
	bool InsertBitVDecl(const string& strName, BitVDeclPtr spVDecl);

	TexturePtr GetTexture(const string& strName);
	bool InsertTexture(const string& strName, TexturePtr spTex);

	RenderStatesPtr GetRenderState(const string& strName);
	bool InsertRenderState(const string& strName, RenderStatesPtr spRS);

	FXShaderPtr GetShader(const string& strName);
	bool InsertShader(const string& strName, FXShaderPtr spShader);

	RenderTargetPtr GetRT(const string& strName);
	bool InsertRT(const string& strName, RenderTargetPtr spRT);
	void RemoveRT( const string& strName );

	void SetOriginalRenderTargetDepth();

protected:

protected:
	static RendererCore* m_pInst;

	BaseRenderer*			m_pRenderer;

	ID3D11Device*			m_pd3dDevice;
	ID3D11DeviceContext*	m_pd3dImmediateContext;
	IDXGISwapChain*			m_pSwapChain;

	ID3D11Texture2D*		m_pDepthStencilTex2D;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11ShaderResourceView* m_pDepthStencilSRV;

	D3D11_VIEWPORT			m_ViewPort;

	D3D_DRIVER_TYPE			m_eDriverType;

	UINT					m_u4xMsaaQuality;

	XMFLOAT4X4				m_View;
	XMFLOAT4X4				m_Proj;
	XMFLOAT3				m_EyePosW;


	int						m_iClientWidth;
	int						m_iClientHeight;
	bool					m_bEnable4xMsaa;

	ERendererType			m_eRendererType;

	SharedVertexBufferMap	m_mapVertexBuffers;
	SharedIndexBufferMap	m_mapIndexBuffers;
	SharedVDeclMap			m_mapVDecl;
	SharedBitDeclMap		m_mapBitVDecl;
	SharedTextureMap		m_mapTextures;
	SharedRenderStateMap	m_mapRenderStates;
	SharedShaderMap			m_mapShaders;
	SharedRenderTargetMap	m_mapRenderTargets;
};

#define GetD3D11Device() RendererCore::Instance()->GetDevice()
#define GetD3D11DeviceImmContext() RendererCore::Instance()->GetDeviceImmContext()

template<typename VertexType>
DrawElement* GenerateMeshElement(UINT uiSize, const string& strObjName, StaticMesh<VertexType>& rMesh, const string& strShaderName,
	const string& strTechname, const string& strRSName, const string& strVDeclName, bool bDrawIndex = true);

template<typename VertexType>
DrawElement* GenerateDFMeshElement( UINT uiSize, const string& strObjName, StaticMesh<VertexType>& rMesh, 
	const string& strRSName, const string& strVDeclName, bool bDrawIndex = true );

template<typename VertexType>
DrawElement* GenerateDMeshElement(UINT uiSize, const string& strObjName, DynamicMesh<VertexType>& rMesh, const string& strShaderName,
	const string& strTechname, const string& strRSName, const string& strVDeclName );

#include "RendererCore.inl"