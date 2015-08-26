#pragma once


class DrawElement
{
public:
	DrawElement()
		: m_pInputLayout(NULL),
		ePrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
		IBFormat(DXGI_FORMAT_R32_UINT),
		m_pRasterS(NULL),
		m_pBlendS(NULL),
		m_pDepthStencilS(NULL),
		m_bDynamic(false),
		m_bDrawIndex(true),
		m_bStencilReflect(false),
		m_bShadowmap(false),
		m_bInstance(false),
		m_uiStencilRef(0)
	{
		stride = sizeof(VertexPNUV);
		offset = 0;

		XMMATRIX I = XMMatrixIdentity();
		XMStoreFloat4x4(&m_ReflectMatrix, I);
	}

	class SubElement
	{
	public:
		SubElement()
			: m_IndexCount(0),
			m_VertexCount(0),
			m_StartVertexLocation(0),
			m_StartIndexLocation(0),
			m_BaseVertexLocation(0),
			m_pTech(0),
			m_iTechIndex(0)
		{
			XMMATRIX I = XMMatrixIdentity();
			XMStoreFloat4x4(&m_World, I);
			XMStoreFloat4x4(&m_TexTransform, I);
		}

		SubElement(const SubElement& rhs)
		{
			//OutputDebugString(L"copy constructor\n");

			m_IndexCount = rhs.m_IndexCount;
			m_VertexCount = rhs.m_VertexCount;
			m_StartVertexLocation = rhs.m_StartVertexLocation;
			m_StartIndexLocation = rhs.m_StartIndexLocation;
			m_BaseVertexLocation = rhs.m_BaseVertexLocation;
			m_World = rhs.m_World;
			m_TexTransform = rhs.m_TexTransform;
			m_mat = rhs.m_mat;

			m_pTech = rhs.m_pTech;
			m_spDiffuseMap = rhs.m_spDiffuseMap;
			m_spNormalMap = rhs.m_spNormalMap;
			m_spCubeMap = rhs.m_spCubeMap;
			m_spShadowMap = rhs.m_spShadowMap;

			m_strTechname = rhs.m_strTechname;
			m_iTechIndex = rhs.m_iTechIndex;
		}

		SubElement(const SubElement&& rhs)
		{
			//OutputDebugString(L"move constructor\n");
			m_IndexCount = rhs.m_IndexCount;
			m_VertexCount = rhs.m_VertexCount;
			m_StartVertexLocation = rhs.m_StartVertexLocation;
			m_StartIndexLocation = rhs.m_StartIndexLocation;
			m_BaseVertexLocation = rhs.m_BaseVertexLocation;;
			m_World = rhs.m_World;
			m_TexTransform = rhs.m_TexTransform;
			m_mat = rhs.m_mat;

			m_pTech = rhs.m_pTech;
			m_spDiffuseMap = rhs.m_spDiffuseMap;
			m_spNormalMap = rhs.m_spNormalMap;
			m_spCubeMap = rhs.m_spCubeMap;
			m_spShadowMap = rhs.m_spShadowMap;

			m_strTechname = rhs.m_strTechname;
			m_iTechIndex = rhs.m_iTechIndex;
		}

		UINT					m_IndexCount;
		UINT					m_VertexCount;
		UINT					m_StartVertexLocation;
		UINT					m_StartIndexLocation;
		UINT					m_BaseVertexLocation;
		XMFLOAT4X4				m_World;
		XMFLOAT4X4				m_TexTransform;
		Material				m_mat;

		ID3DX11EffectTechnique*	m_pTech;
		TexturePtr				m_spDiffuseMap;
		TexturePtr				m_spNormalMap;
		TexturePtr				m_spCubeMap;
		TexturePtr				m_spShadowMap;

		string					m_strTechname;
		int						m_iTechIndex;
	};

	XMFLOAT4X4				m_ReflectMatrix;

	UINT stride;
	UINT offset;
	UINT m_uiStencilRef;

	DXGI_FORMAT				IBFormat;
	D3D_PRIMITIVE_TOPOLOGY	ePrimitiveTopology;

	VertexBufferPtr			m_spVB;
	IndexBufferPtr			m_spIB;
	FXShaderPtr				m_spShader;
	RenderStatesPtr			m_spRenderState;
	RenderStatesPtr			m_spPreRenderState;

	ID3D11InputLayout*		m_pInputLayout;
	ID3D11RasterizerState*	m_pRasterS;
	ID3D11BlendState*		m_pBlendS;
	ID3D11DepthStencilState* m_pDepthStencilS;

	vector<SubElement>		m_vecSubElement;

	PointLight				m_PointLight;

	bool m_bDynamic;
	bool m_bDrawIndex;
	bool m_bStencilReflect;
	bool m_bShadowmap;
	bool m_bInstance;
};