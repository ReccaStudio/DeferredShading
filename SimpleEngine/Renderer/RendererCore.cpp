#include "stdafx.h"
#include "RendererCore.h"
#include "ForwardRenderer.h"
#include "DeferredRenderer.h"

RendererCore* RendererCore::m_pInst = NULL;

RendererCore::RendererCore()
: m_eDriverType(D3D_DRIVER_TYPE_HARDWARE),
m_iClientWidth(1024),
m_iClientHeight(768),
m_bEnable4xMsaa(false),
m_u4xMsaaQuality(0),
m_pd3dDevice(0),
m_pd3dImmediateContext(0),
m_pSwapChain(0),
m_pDepthStencilTex2D(0),
m_pRenderTargetView(0),
m_pDepthStencilView(0)
{
	ZeroMemory(&m_ViewPort, sizeof(D3D11_VIEWPORT));


	m_eRendererType = ERDR_Deferred;
	switch ( m_eRendererType )
	{
	case ERDR_Forward:
		m_pRenderer = new ForwardRenderer();
		break;
	case ERDR_Deferred:
		m_pRenderer = new DeferredRenderer();
		break;
	default:
		break;
	}
}


RendererCore::~RendererCore()
{


	//unordered_map<TexturePtr>::iterator itor = m_mapTextures.begin();
	//while (itor != m_mapTextures.end())
	//{
	//	itor->second;
	//	itor++;
	//}

// 	for (auto itor = m_mapTextures.begin(); itor != m_mapTextures.end(); ++itor)
// 	{
// 		shared_ptr<Texture> spTex = itor->second;
// 		if (spTex)
// 			spTex.reset();
// 	}

	m_mapVertexBuffers.clear();
	m_mapIndexBuffers.clear();
	m_mapBitVDecl.clear();
	m_mapTextures.clear();
	m_mapRenderTargets.clear();

	m_mapShaders.clear();
	m_mapVDecl.clear();
	m_mapRenderStates.clear();

	SafeDelete( m_pRenderer );

	ReleaseCOM(m_pRenderTargetView);
	ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pSwapChain);
	ReleaseCOM(m_pDepthStencilTex2D);

	// Restore all default settings.
	if (m_pd3dImmediateContext)
		m_pd3dImmediateContext->ClearState();

	ReleaseCOM(m_pd3dImmediateContext);
	ReleaseCOM(m_pd3dDevice);

}

bool RendererCore::Init(HINSTANCE hInst, HWND wMainWnd)
{
	// Create the device and device context.

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		m_eDriverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&m_pd3dDevice,
		&featureLevel,
		&m_pd3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	HR(m_pd3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_u4xMsaaQuality));
	assert(m_u4xMsaaQuality > 0);

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_iClientWidth;
	sd.BufferDesc.Height = m_iClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4X MSAA? 
	if (m_bEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_u4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = wMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(m_pd3dDevice, &sd, &m_pSwapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.

	ID3D11Debug *d3dDebug = nullptr;
	if (SUCCEEDED(m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug)))
	{
		ID3D11InfoQueue *d3dInfoQueue = nullptr;
		if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
			D3D11_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
				// Add more message IDs here as needed
			};

			D3D11_INFO_QUEUE_FILTER filter;
			memset(&filter, 0, sizeof(filter));
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
			d3dInfoQueue->Release();
		}
		d3dDebug->Release();
	}

	return true;
}

bool RendererCore::InitRenderer()
{
	if ( m_pRenderer )
		m_pRenderer->Init();

	return true;
}

void RendererCore::Resize()
{
	assert(m_pd3dImmediateContext);
	assert(m_pd3dDevice);
	assert(m_pSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(m_pRenderTargetView);
	ReleaseCOM(m_pDepthStencilView);
	ReleaseCOM(m_pDepthStencilTex2D);


	// Resize the swap chain and recreate the render target view.

	HR(m_pSwapChain->ResizeBuffers(1, m_iClientWidth, m_iClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_pd3dDevice->CreateRenderTargetView(backBuffer, 0, &m_pRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_iClientWidth;
	depthStencilDesc.Height = m_iClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (m_bEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_u4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(m_pd3dDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilTex2D));
	HR(m_pd3dDevice->CreateDepthStencilView(m_pDepthStencilTex2D, 0, &m_pDepthStencilView));


	// Bind the render target view and depth/stencil view to the pipeline.

	m_pd3dImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


	// Set the viewport transform.

	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<float>(m_iClientWidth);
	m_ViewPort.Height = static_cast<float>(m_iClientHeight);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	m_pd3dImmediateContext->RSSetViewports(1, &m_ViewPort);

	if ( m_pRenderer )
		m_pRenderer->Resize();
}

void RendererCore::UpdateScene(float dt)
{
	if ( m_pRenderer )
		m_pRenderer->Update( dt );
}

void RendererCore::DrawScene()
{
	m_pd3dImmediateContext->ClearRenderTargetView( m_pRenderTargetView, reinterpret_cast<const float*>(&Colors::Black) );
	m_pd3dImmediateContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

	
	if ( m_pRenderer )
		m_pRenderer->DrawScene();

	// restore default states, as the SkyFX changes them in the effect file.
	m_pd3dImmediateContext->RSSetState(0);
	m_pd3dImmediateContext->OMSetDepthStencilState(0, 0);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pd3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	// Unbind shadow map as a shader input because we are going to render to it next frame.
	// The shadow might might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	m_pd3dImmediateContext->PSSetShaderResources(0, 16, nullSRV);

	HR(m_pSwapChain->Present(0, 0));
}

void* RendererCore::MapbyVB(VertexBufferPtr spVB)
{
	D3D11_MAPPED_SUBRESOURCE mapData;
	m_pd3dImmediateContext->Map(spVB->GetVB(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapData);

	return mapData.pData;
}

void RendererCore::UnmapbyVB(VertexBufferPtr spVB)
{
	m_pd3dImmediateContext->Unmap(spVB->GetVB(), 0);
}

// void RendererCore::SetPointLightPos(XMFLOAT3& PointLitPos)
// {
// 	m_PointLight.Position = PointLitPos;
// }
// 
// void RendererCore::SetSpotLightPos(XMFLOAT3 pos, XMVECTOR dir)
// {
// 	// The spotlight takes on the camera position and is aimed in the
// 	// same direction the camera is looking.  In this way, it looks
// 	// like we are holding a flashlight.
// 	m_SpotLight.Position = pos;
// 	XMStoreFloat3(&m_SpotLight.Direction, dir);
// }

VertexBufferPtr RendererCore::GetVB(const string& strName)
{
	SharedVertexBufferMap::iterator itor = m_mapVertexBuffers.find(strName);

	if (itor == m_mapVertexBuffers.end())
		return NULL;

	return itor->second;

}

bool RendererCore::InsertVB(const string& strName, VertexBufferPtr spVB)
{
	SharedVertexBufferMap::_Pairib ret = m_mapVertexBuffers.insert(make_pair(strName, spVB));

	return ret.second;

}

IndexBufferPtr RendererCore::GetIB(const string& strName)
{
	SharedIndexBufferMap::iterator itor = m_mapIndexBuffers.find(strName);

	if (itor == m_mapIndexBuffers.end())
		return NULL;

	return itor->second;

}

bool RendererCore::InsertIB(const string& strName, IndexBufferPtr spIB)
{
	SharedIndexBufferMap::_Pairib ret = m_mapIndexBuffers.insert(make_pair(strName, spIB));

	return ret.second;
}

VertexDeclarationPtr RendererCore::GetVDecl(const string& strName)
{
	SharedVDeclMap::iterator itor = m_mapVDecl.find(strName);

	if (itor == m_mapVDecl.end())
		return NULL;

	return itor->second;
}

bool RendererCore::InsertVDecl(const string& strName, VertexDeclarationPtr spVDecl)
{
	SharedVDeclMap::_Pairib ret = m_mapVDecl.insert(make_pair(strName, spVDecl));

	return ret.second;
}

BitVDeclPtr RendererCore::GetBitVDecl(const string& strName)
{
	SharedBitDeclMap::iterator itor = m_mapBitVDecl.find(strName);

	if (itor == m_mapBitVDecl.end())
		return NULL;

	return itor->second;
}

bool RendererCore::InsertBitVDecl(const string& strName, BitVDeclPtr spVDecl)
{
	SharedBitDeclMap::_Pairib ret = m_mapBitVDecl.insert(make_pair(strName, spVDecl));

	return ret.second;
}

TexturePtr RendererCore::GetTexture(const string& strName)
{
	SharedTextureMap::iterator itor = m_mapTextures.find(strName);

	if (itor == m_mapTextures.end())
		return NULL;

	return itor->second;
}

bool RendererCore::InsertTexture(const string& strName, TexturePtr spTex)
{
	SharedTextureMap::_Pairib ret = m_mapTextures.insert(make_pair(strName, spTex));

	return ret.second;
}

RenderStatesPtr RendererCore::GetRenderState(const string& strName)
{
	SharedRenderStateMap::iterator itor = m_mapRenderStates.find(strName);

	if (itor == m_mapRenderStates.end())
		return NULL;

	return itor->second;
}

bool RendererCore::InsertRenderState(const string& strName, RenderStatesPtr spRS)
{
	SharedRenderStateMap::_Pairib ret = m_mapRenderStates.insert(make_pair(strName, spRS));

	return ret.second;
}

FXShaderPtr RendererCore::GetShader(const string& strName)
{
	SharedShaderMap::iterator itor = m_mapShaders.find(strName);

	if (itor == m_mapShaders.end())
		return NULL;

	return itor->second;
}

bool RendererCore::InsertShader(const string& strName, FXShaderPtr spShader)
{
	SharedShaderMap::_Pairib ret = m_mapShaders.insert(make_pair(strName, spShader));

	return ret.second;

}

RenderTargetPtr RendererCore::GetRT(const string& strName)
{
	SharedRenderTargetMap::iterator itor = m_mapRenderTargets.find(strName);

	if (itor == m_mapRenderTargets.end())
		return NULL;

	return itor->second;
}

bool RendererCore::InsertRT(const string& strName, RenderTargetPtr spRT)
{
	SharedRenderTargetMap::_Pairib ret = m_mapRenderTargets.insert(make_pair(strName, spRT));

	return ret.second;

}

void RendererCore::RemoveRT( const string& strName )
{
	SharedRenderTargetMap::iterator itor = m_mapRenderTargets.find( strName );

	if ( itor != m_mapRenderTargets.end() )
		itor->second.reset();
}

void RendererCore::SetOriginalRenderTargetDepth()
{
	m_pd3dImmediateContext->RSSetState( 0 );

	//
	// Restore the back and depth buffer to the OM stage.
	//
	ID3D11RenderTargetView* renderTargets[1] = { m_pRenderTargetView };
	m_pd3dImmediateContext->OMSetRenderTargets( 1, renderTargets, m_pDepthStencilView );
	m_pd3dImmediateContext->RSSetViewports( 1, &m_ViewPort );
	/**/
}
