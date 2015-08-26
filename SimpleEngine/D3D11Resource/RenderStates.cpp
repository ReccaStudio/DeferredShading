#include "stdafx.h"
#include "RenderStates.h"
#include "RendererCore.h"

RenderStates::RenderStates()
: m_pRState(NULL),
m_pBState(NULL),
m_pDSState(NULL)
{
}


RenderStates::~RenderStates()
{
	ReleaseCOM(m_pRState);
	ReleaseCOM(m_pBState);
	ReleaseCOM(m_pDSState);
}

RenderStatesPtr RenderStates::Create(ERasterS eRs, EBlendS eBs, EDepthStencilS eDS)
{
	RenderStatesPtr spNewRState(new RenderStates());
	spNewRState->CreateRasterizerState(eRs);
	spNewRState->CreateBlendState(eBs);
	spNewRState->CreateDepthStencilState(eDS);

	return spNewRState;
}

void RenderStates::CreateRasterizerState(ERasterS eRs)
{
	if (eRs == ERS_None)
		return;

	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));

// 	rsDesc.DepthBias = 100000;
// 	rsDesc.DepthBiasClamp = 0.0f;
// 	rsDesc.SlopeScaledDepthBias = 1.0f;

	if (eRs & ERS_Wireframe)
	{
		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
		rsDesc.CullMode = D3D11_CULL_BACK;
		rsDesc.FrontCounterClockwise = false;
		rsDesc.DepthClipEnable = true;
	}
	else if (eRs & ERS_NoCull)
	{
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_NONE;
		rsDesc.FrontCounterClockwise = false;
		rsDesc.DepthClipEnable = true;
	}
	else if (eRs & ERS_CullClockwise)
	{
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_BACK;
		rsDesc.FrontCounterClockwise = true;
		rsDesc.DepthClipEnable = true;
	}
	else if (eRs & ERS_Cull)
	{
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_BACK;
		rsDesc.FrontCounterClockwise = false;
		rsDesc.DepthClipEnable = true;
	}

	rsDesc.DepthClipEnable = false;

	HR(GetD3D11Device()->CreateRasterizerState(&rsDesc, &m_pRState));
	DXUT_SetDebugName( m_pRState, "RasterizeState" );
}

void RenderStates::CreateBlendState(EBlendS eBs)
{
	if ( eBs == EBS_None )
		return;

	D3D11_BLEND_DESC bsDesc = { 0 };
	//ZeroMemory(&bsDesc, sizeof(D3D11_BLEND_DESC));
// 	if ( eBs == EBS_None )
// 	{
// 		bsDesc.AlphaToCoverageEnable = false;
// 		bsDesc.IndependentBlendEnable = false;
// 		bsDesc.RenderTarget[0].BlendEnable = false;
// 	}
	if (eBs & EBS_AlphatoCoverage)
	{
		bsDesc.AlphaToCoverageEnable = true;
		bsDesc.IndependentBlendEnable = false;
		bsDesc.RenderTarget[0].BlendEnable = false;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else if (eBs & EBS_Transparent)
	{
		bsDesc.AlphaToCoverageEnable = false;
		bsDesc.IndependentBlendEnable = false;

		bsDesc.RenderTarget[0].BlendEnable = true;
		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else if ( eBs & EBS_ADD_ONE )
	{
		bsDesc.AlphaToCoverageEnable = false;
		bsDesc.IndependentBlendEnable = false;

		bsDesc.RenderTarget[0].BlendEnable = true;
		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	else if ( eBs & EBS_NoRenderTargetWrite )
	{
		bsDesc.AlphaToCoverageEnable = false;
		bsDesc.IndependentBlendEnable = false;

		bsDesc.RenderTarget[0].BlendEnable = false;
		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = 0;
	}

	HR(GetD3D11Device()->CreateBlendState(&bsDesc, &m_pBState));
	DXUT_SetDebugName( m_pBState, "BlendState" );
}

void RenderStates::CreateDepthStencilState( EDepthStencilS eDS )
{
	if ( eDS & EDSS_None )
		return;

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory( &dsDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	//if ( eDS & EDSS_None )
	//{
	//	dsDesc.DepthEnable = false;
	//	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//	dsDesc.StencilEnable = false;
	//	dsDesc.StencilWriteMask = 0xff;
	//	dsDesc.StencilReadMask = 0xff;
	//}
	if ( eDS & EDSS_Default )
	{
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = false;
		dsDesc.StencilWriteMask = 0xff;
		dsDesc.StencilReadMask = 0xff;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	}
	else if ( eDS & EDSS_MarkMirror )
	{
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = true;
		dsDesc.StencilWriteMask = 0xff;
		dsDesc.StencilReadMask = 0xff;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	}
	else if ( eDS & EDSS_DrawRelection )
	{
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = true;
		dsDesc.StencilWriteMask = 0xff;
		dsDesc.StencilReadMask = 0xff;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	}
	else if ( eDS & EDSS_NoDoubleBlend )
	{
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = true;
		dsDesc.StencilWriteMask = 0xff;
		dsDesc.StencilReadMask = 0xff;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	}
	else if ( eDS & EDSS_StencilVolumeMask )
	{
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = true;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.StencilReadMask = 0xFF;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	}
	else if ( eDS & EDSS_StencilClippedPixels )
	{
		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_NEVER;
		dsDesc.StencilEnable = true;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.StencilReadMask = 0xFF;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	}
	else if ( eDS & EDSS_DepthWRDisable )
	{
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.StencilEnable = false;
		dsDesc.StencilWriteMask = 0xFF;
		dsDesc.StencilReadMask = 0xFF;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	}

	HR(GetD3D11Device()->CreateDepthStencilState(&dsDesc, &m_pDSState));
	DXUT_SetDebugName( m_pDSState, "DepthStencilState" );

}

void RenderStates::SetStateAll( ID3D11DeviceContext* pDeviceContext, UINT uiStencilRef )
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if ( !pDeviceContext )
		return;

	pDeviceContext->RSSetState( m_pRState );
	pDeviceContext->OMSetBlendState( m_pBState, blendFactor, 0xFFFFFFFF);
	pDeviceContext->OMSetDepthStencilState( m_pDSState, uiStencilRef );
}
