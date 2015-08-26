#include "stdafx.h"
#include "RenderTarget.h"
#include "RendererCore.h"


RenderTarget::RenderTarget() :
m_pTex2D(NULL),
m_pRTV(NULL),
m_pSRV(NULL)
{
}


RenderTarget::~RenderTarget()
{
	ReleaseCOM(m_pTex2D);
	ReleaseCOM(m_pRTV);
	ReleaseCOM(m_pSRV);
}

shared_ptr<RenderTarget> RenderTarget::Create(UINT uiWidth, UINT uiHeight)
{
	shared_ptr<RenderTarget> spRT( new RenderTarget() );
	spRT->CreateRT(uiWidth, uiHeight);

	return spRT;
}

bool RenderTarget::CreateRT(UINT uiWidth, UINT uiHeight)
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D11_TEXTURE2D_DESC));

	texDesc.Width = uiWidth;
	texDesc.Height = uiHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HR( GetD3D11Device()->CreateTexture2D(&texDesc, NULL, &m_pTex2D) );

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	ZeroMemory( &rtDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC) );
	
	rtDesc.Format = texDesc.Format;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	HR( GetD3D11Device()->CreateRenderTargetView( m_pTex2D, &rtDesc, &m_pRTV) );


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	HR( GetD3D11Device()->CreateShaderResourceView(m_pTex2D, &srvDesc, &m_pSRV) );

	return true;
}

void RenderTarget::Resize( UINT uiWidth, UINT uiHeight )
{
	ReleaseCOM( m_pTex2D );
	ReleaseCOM( m_pRTV );
	ReleaseCOM( m_pSRV );

	CreateRT( uiWidth, uiHeight );
}
