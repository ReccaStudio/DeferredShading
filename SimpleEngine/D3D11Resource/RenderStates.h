#pragma once

class RenderStates
{
public:
	RenderStates();
	virtual ~RenderStates();

	enum ERasterS
	{
		ERS_None			= 0x0000,
		ERS_Wireframe		= 0x0001,
		ERS_NoCull			= 0x0002,
		ERS_CullClockwise	= 0x0004,
		ERS_Cull			= 0x0008,
	};

	enum EBlendS
	{
		EBS_None				= 0x0000,
		EBS_AlphatoCoverage		= 0x0001,
		EBS_Transparent			= 0x0002,
		EBS_ADD_ONE				= 0x0004,
		EBS_NoRenderTargetWrite	= 0x0008,
	};

	enum EDepthStencilS
	{
		EDSS_None				= 0x0000,
		EDSS_Default			= 0x0001,
		EDSS_MarkMirror			= 0x0002,
		EDSS_DrawRelection		= 0x0004,
		EDSS_NoDoubleBlend		= 0x0008,
		EDSS_StencilVolumeMask	= 0x0010,
		EDSS_StencilClippedPixels		= 0x0020,
		EDSS_DepthWRDisable		= 0x0040,
	};

	static shared_ptr<RenderStates> Create(ERasterS eRs = ERS_None, 
		EBlendS eBs = EBS_None, EDepthStencilS eDS = EDSS_None);

	ID3D11RasterizerState* GetRS() { return m_pRState; }
	ID3D11BlendState* GetBS() { return m_pBState;  }
	ID3D11DepthStencilState* GetDSS() { return m_pDSState;  }

	void CreateRasterizerState(ERasterS eRs);
	void CreateBlendState(EBlendS eBs);
	void CreateDepthStencilState(EDepthStencilS eDS);

	void SetStateAll( ID3D11DeviceContext* pDeviceContext, UINT uiStencilRef = 0 );

protected:

protected:
	ID3D11RasterizerState*		m_pRState;
	ID3D11BlendState*			m_pBState;
	ID3D11DepthStencilState*	m_pDSState;
};

typedef shared_ptr<RenderStates> RenderStatesPtr;