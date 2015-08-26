#include "stdafx.h"
#include "Texture.h"
#include "RendererCore.h"


Texture::Texture()
: m_pTexShaderRV(NULL),
m_uiWidth(0),
m_uiHeight(0),
m_bArray(false)
{
}


Texture::~Texture()
{
	ReleaseCOM(m_pTexShaderRV);
}

shared_ptr<Texture> Texture::Create(const wstring& filename)
{
	shared_ptr<Texture> spTexture(new Texture());
	spTexture->CreateFile(filename);
	return spTexture;
}

shared_ptr<Texture> Texture::Create(ID3D11Texture2D* pTexture2D)
{
	shared_ptr<Texture> spTexture(new Texture());
	spTexture->CreateTexture2D(pTexture2D);
	return spTexture;
}

shared_ptr<Texture> Texture::Create(const vector<wstring>& filelist,
	DXGI_FORMAT format, UINT filter, UINT mipFilter)
{
	shared_ptr<Texture> spTexture(new Texture());
	spTexture->CreateTexture2DArraySRV(filelist, format, filter, mipFilter);
	return spTexture;
}

bool Texture::CreateFile(const wstring& filename)
{
	HR(D3DX11CreateShaderResourceViewFromFile(GetD3D11Device(),
		filename.c_str(), 0, 0, &m_pTexShaderRV, 0));

// #ifdef _DEBUG
// 	const char c_szName[] = "mytexture.jpg";
// 	m_pRawTexture->SetPrivateData(WKPDID_D3DDebugObjectName, filename.length(), filename.c_str());
// 
// #endif
	return true;
}

bool Texture::CreateTexture2D(ID3D11Texture2D* pTexture2D)
{
	if (!pTexture2D)
		return false;

	//BindFlags : D3D11_BIND_SHADER_RESOURCE   
	D3D11_TEXTURE2D_DESC t2dDesc;
	pTexture2D->GetDesc(&t2dDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = t2dDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = t2dDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	HR(GetD3D11Device()->CreateShaderResourceView(pTexture2D, &srvDesc, &m_pTexShaderRV));

// 	const char c_szName[] = "mytexture.jpg";
// 	m_pRawTexture->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(c_szName)-1, c_szName);

	return true;
}

bool Texture::CreateTexture2DArraySRV(const vector<wstring>& filenames, 
	DXGI_FORMAT format, UINT filter, UINT mipFilter)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//

	UINT size = filenames.size();

	std::vector<ID3D11Texture2D*> srcTex(size);
	for (UINT i = 0; i < size; ++i)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;

		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.Depth = D3DX11_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX11_FROM_FILE;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = format;
		loadInfo.Filter = filter;
		loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo = 0;

		HR(D3DX11CreateTextureFromFile(GetD3D11Device(), filenames[i].c_str(),
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0));
	}

	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	HR(GetD3D11Device()->CreateTexture2D(&texArrayDesc, 0, &texArray));

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		// for each mipmap level...
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(GetD3D11DeviceImmContext()->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			GetD3D11DeviceImmContext()->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			GetD3D11DeviceImmContext()->Unmap(srcTex[texElement], mipLevel);
		}
	}

	//
	// Create a resource view to the texture array.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV = 0;
	HR(GetD3D11Device()->CreateShaderResourceView(texArray, &viewDesc, &m_pTexShaderRV));

	//
	// Cleanup--we only need the resource view.
	//

	ReleaseCOM(texArray);

	for (UINT i = 0; i < size; ++i)
		ReleaseCOM(srcTex[i]);

	return true;
}

DepthStencilTexture::DepthStencilTexture() : Texture(),
m_pDepthStencilView(0)
{

}

DepthStencilTexture::~DepthStencilTexture()
{
	ReleaseCOM(m_pDepthStencilView);
}

shared_ptr<Texture> DepthStencilTexture::Create(UINT uiWidth, UINT uiHeight)
{
	DepthStencilTexturePtr spTexture(new DepthStencilTexture());
	spTexture->CreateDepthStencil(uiWidth, uiHeight);

	return dynamic_pointer_cast<Texture, DepthStencilTexture>(spTexture);
}

bool DepthStencilTexture::CreateDepthStencil(UINT uiWidth, UINT uiHeight)
{
	m_uiWidth = uiWidth;
	m_uiHeight = uiHeight;

	m_Viewport.TopLeftX = 0.0f;
	m_Viewport.TopLeftY = 0.0f;
	m_Viewport.Width	= static_cast<float>(uiWidth);
	m_Viewport.Height	= static_cast<float>(uiHeight);
	m_Viewport.MinDepth = 0.0f;
	m_Viewport.MaxDepth = 1.0f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = m_uiWidth;
	texDesc.Height = m_uiHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = 0;
	HR(GetD3D11Device()->CreateTexture2D(&texDesc, 0, &depthMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(GetD3D11Device()->CreateDepthStencilView(depthMap, &dsvDesc, &m_pDepthStencilView));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(GetD3D11Device()->CreateShaderResourceView(depthMap, &srvDesc, &m_pTexShaderRV));

	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(depthMap);

	return true;
}


void DepthStencilTexture::BindDsvAndSetNullRenderTarget()
{
	GetD3D11DeviceImmContext()->RSSetViewports(1, &m_Viewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	GetD3D11DeviceImmContext()->OMSetRenderTargets(1, renderTargets, m_pDepthStencilView);

	GetD3D11DeviceImmContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
