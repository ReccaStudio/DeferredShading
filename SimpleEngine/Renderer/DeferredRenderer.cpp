#include "stdafx.h"
#include "DeferredRenderer.h"
#include "RendererCore.h"
#include "DirectX11Util.h"
#include "GeometryGenerator.h"


const D3D11_INPUT_ELEMENT_DESC layoutPNUVT[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

const D3D11_INPUT_ELEMENT_DESC layoutPUV[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


DeferredRenderer::DeferredRenderer() : BaseRenderer() ,
m_pDeferredVS(NULL),
m_pDeferredPS(NULL),
m_pGeomPassVS(NULL),
m_pFinalPassVS(NULL),
m_pFinalPassPS(NULL),
m_pDebugPassPS(NULL),
m_pAmbientLightVS(NULL),
m_pAmbientLightPS(NULL),
m_pCBPerObject(NULL),
m_pCBPerFrame(NULL),
m_pPointSampler(NULL),
m_pLinearSampler(NULL),
m_pGeomPassInputLO(NULL),
m_pFinalInputLO(NULL)
{
	
}


DeferredRenderer::~DeferredRenderer()
{
	ReleaseCOM( m_pDeferredVS );
	ReleaseCOM( m_pDeferredPS );
	ReleaseCOM( m_pGeomPassVS );
	ReleaseCOM( m_pFinalPassVS );
	ReleaseCOM( m_pFinalPassPS );
	ReleaseCOM( m_pDebugPassPS );
	ReleaseCOM( m_pAmbientLightPS );
	ReleaseCOM( m_pAmbientLightVS );
	ReleaseCOM( m_pCBPerObject );
	ReleaseCOM( m_pCBPerFrame );
	ReleaseCOM( m_pPointSampler );
	ReleaseCOM( m_pLinearSampler );
	ReleaseCOM( m_pDefaultIO );
	ReleaseCOM( m_pFinalInputLO );
	ReleaseCOM( m_pGeomPassInputLO );
	ReleaseCOM( m_pCBAmbientLight );
}

void DeferredRenderer::Init()
{
	m_ViewPort.TopLeftX = 0.0f;
	m_ViewPort.TopLeftY = 0.0f;
	m_ViewPort.Width = static_cast<float>(RendererCore::Instance()->ClientWidth());
	m_ViewPort.Height = static_cast<float>(RendererCore::Instance()->ClientHeight());
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	// Compile the shaders to a model based on the feature level we acquired
	LPCSTR pszVSModel = "vs_5_0";
	LPCSTR pszPSModel = "ps_5_0";

	if ( GetD3D11Device()->GetFeatureLevel() != D3D_FEATURE_LEVEL_11_0 )
		::MessageBoxA( NULL, "This sample requires D3D_FEATURE_LEVEL_11_0 to run.", "Error", MB_OK | MB_ICONERROR );

	//Default deferred 
	ID3DBlob* pShaderBuffer = NULL;
	DirectX11Util::CompileFromFile( L"HLSLs/DeferredVertexShader.hlsl", "BasicDeferredVS", pszVSModel, &pShaderBuffer );
	HR( GetD3D11Device()->CreateVertexShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pDeferredVS ));
	HR( GetD3D11Device()->CreateInputLayout( layoutPNUVT, 4, pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), &m_pDefaultIO ) );
	pShaderBuffer->Release();


	DirectX11Util::CompileFromFile( L"HLSLs/DeferredPixelShader.hlsl", "BasicDeferredPS", pszPSModel, &pShaderBuffer );
	HR( GetD3D11Device()->CreatePixelShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pDeferredPS ) );
	pShaderBuffer->Release();

	//Ambient Light
	DirectX11Util::CompileFromFile( L"HLSLs/FinalPassVS.hlsl", "AmbientLightVS", pszVSModel, &pShaderBuffer );
	HR( GetD3D11Device()->CreateVertexShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pAmbientLightVS ) );
	pShaderBuffer->Release();

	DirectX11Util::CompileFromFile( L"HLSLs/FinalPassPS.hlsl", "AmbientLightPS", pszPSModel, &pShaderBuffer );
	HR( GetD3D11Device()->CreatePixelShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pAmbientLightPS ) );
	pShaderBuffer->Release();

	//stencil volume mask
	DirectX11Util::CompileFromFile( L"HLSLs/FinalPassVS.hlsl", "GeometryPassVS", pszVSModel, &pShaderBuffer );
	HR( GetD3D11Device()->CreateVertexShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pGeomPassVS ) );
	HR( GetD3D11Device()->CreateInputLayout( layoutPUV, 2, pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), &m_pGeomPassInputLO ) );
	pShaderBuffer->Release();

// 	DirectX11Util::CompileFromFile( L"HLSLs/FinalPassPS.hlsl", "FinalPassPS", pszPSModel, &pShaderBuffer );
// 	HR( GetD3D11Device()->CreatePixelShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pStencilVMPS ) );
// 	pShaderBuffer->Release();

	//Final 
	DirectX11Util::CompileFromFile( L"HLSLs/FinalPassVS.hlsl", "FinalPassVS", pszVSModel, &pShaderBuffer );
	HR( GetD3D11Device()->CreateVertexShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pFinalPassVS ) );
	HR( GetD3D11Device()->CreateInputLayout( layoutPUV, 2, pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), &m_pFinalInputLO ) );
	pShaderBuffer->Release();

	DirectX11Util::CompileFromFile( L"HLSLs/FinalPassPS.hlsl", "FinalPassPS", pszPSModel, &pShaderBuffer );
	HR( GetD3D11Device()->CreatePixelShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pFinalPassPS ) );
	pShaderBuffer->Release();

	//Debug Pass
	DirectX11Util::CompileFromFile( L"HLSLs/DebugPassPS.hlsl", "DebugPassPS", pszPSModel, &pShaderBuffer );
	HR( GetD3D11Device()->CreatePixelShader( pShaderBuffer->GetBufferPointer(), pShaderBuffer->GetBufferSize(), NULL, &m_pDebugPassPS ) );
	pShaderBuffer->Release();


	m_pCBPerObject = DirectX11Util::CreateBuffer( GetD3D11Device(), sizeof( cbPerObject ), D3D11_USAGE_DEFAULT, 0 );
	m_pCBPerFrame = DirectX11Util::CreateBuffer( GetD3D11Device(), sizeof( cbPerFrame ), D3D11_USAGE_DEFAULT, 0 );
	m_pCBAmbientLight = DirectX11Util::CreateBuffer( GetD3D11Device(), sizeof( float )*4, D3D11_USAGE_DEFAULT, 0 );

	m_pPointSampler = DirectX11Util::CreateSamplerState( GetD3D11Device(), D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP );
	m_pLinearSampler = DirectX11Util::CreateSamplerState( GetD3D11Device(), D3D11_FILTER_MIN_MAG_MIP_LINEAR );


	//m_spDepthStencilTex = dynamic_pointer_cast<DepthStencilTexture, Texture>(RendererCore::Instance()->GetTexture( "depthstencil" ));
	m_spGeomPassRS = RendererCore::Instance()->GetRenderState( "geometrypass" );

	//BuildGridBuffers();
	BuildBoxBuffers();
	BuildScreenQuadBufferByDebugStencil();
	BuildSpherebyStencilMask();
	BuildSpherebyDiffuseLight();
	BuildQuadBufferByAmbientLight();
}

void DeferredRenderer::Resize()
{
	m_spAlbedoRT = 0;
	m_spNormalRT = 0;
	m_spDepthRT = 0;
	m_spLightingRT = 0;

	RendererCore::Instance()->RemoveRT( "albedo" );
	RendererCore::Instance()->RemoveRT( "normal" );
	RendererCore::Instance()->RemoveRT( "depth" );
	RendererCore::Instance()->RemoveRT( "lighting" );

	m_spAlbedoRT =
		RenderTarget::Create( RendererCore::Instance()->ClientWidth(), RendererCore::Instance()->ClientHeight() );
	m_spNormalRT =
		RenderTarget::Create( RendererCore::Instance()->ClientWidth(), RendererCore::Instance()->ClientHeight() );
	m_spDepthRT =
		RenderTarget::Create( RendererCore::Instance()->ClientWidth(), RendererCore::Instance()->ClientHeight() );
	m_spLightingRT =
		RenderTarget::Create( RendererCore::Instance()->ClientWidth(), RendererCore::Instance()->ClientHeight() );

	RendererCore::Instance()->InsertRT( "albedo", m_spAlbedoRT );
	RendererCore::Instance()->InsertRT( "normal", m_spNormalRT );
	RendererCore::Instance()->InsertRT( "depth", m_spDepthRT );
	RendererCore::Instance()->InsertRT( "lighting", m_spLightingRT );

}

void DeferredRenderer::Update(float dt)
{
	float fLightRotationAngle = 0.4f*dt;

	XMMATRIX R = XMMatrixRotationY( fLightRotationAngle );

	for ( auto itorElem = m_vDrawLightingElements.begin() ; itorElem != m_vDrawLightingElements.end() ; ++itorElem )
	{
		DrawElement* pElem = (*itorElem);
		if ( !pElem )
			continue;

		for ( auto itor = pElem->m_vecSubElement.begin(); itor != pElem->m_vecSubElement.end(); ++itor )
		{
			XMMATRIX world = XMLoadFloat4x4( &(*itor).m_World );

			world = world * R;
			XMStoreFloat4x4( &(*itor).m_World, world );
		}

		XMVECTOR pos = XMLoadFloat3( &pElem->m_PointLight.Position );
		pos = XMVector3Transform( pos, R );
		XMStoreFloat3( &pElem->m_PointLight.Position, pos );
	}
}

void DeferredRenderer::DrawScene()
{
	DrawGeometryPass();
	DrawMRTPass();

	ID3D11RenderTargetView* pRenderTargets[] = { RendererCore::Instance()->GetRTView() };
	GetD3D11DeviceImmContext()->OMSetRenderTargets( 1, pRenderTargets,
		RendererCore::Instance()->GetDSView() );

	DrawAmbientLightPass();
	DrawDirectionLightsPass();
	DrawPointLightsPass();
	DrawSpotLightsPass();

	//DrawDebugStencil();
	//DrawScreenQuad();
}

void DeferredRenderer::DrawGeometryPass()
{
	XMMATRIX view = XMLoadFloat4x4( RendererCore::Instance()->GetView() );
	XMMATRIX proj = XMLoadFloat4x4( RendererCore::Instance()->GetProj() );
	XMMATRIX viewproj = view*proj;

	for ( auto itorElem = m_vDrawElements.begin() ; itorElem != m_vDrawElements.end() ; ++itorElem )
	{
		DrawElement* pElem = (*itorElem);
		if ( !pElem )
			continue;

		UINT stride = pElem->stride;
		UINT offset = pElem->offset;

		GetD3D11DeviceImmContext()->IASetInputLayout( m_pGeomPassInputLO );
		GetD3D11DeviceImmContext()->IASetPrimitiveTopology( pElem->ePrimitiveTopology );

		for ( auto itor = pElem->m_vecSubElement.begin(); itor != pElem->m_vecSubElement.end(); ++itor )
		{
			ID3D11Buffer* pVB = pElem->m_spVB->GetVB();
			GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );

			if ( pElem->m_spIB )
				GetD3D11DeviceImmContext()->IASetIndexBuffer( pElem->m_spIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

			m_spGeomPassRS->SetStateAll( GetD3D11DeviceImmContext(), 0 );


			XMMATRIX world = XMLoadFloat4x4( &(*itor).m_World );
			//XMMATRIX shadowTransform = world * shadowTransform;
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose( world );
			XMMATRIX worldViewProj = world*viewproj;
			XMMATRIX texTransform = XMLoadFloat4x4( &(*itor).m_TexTransform );

			cbPerObject perObj;
			XMStoreFloat4x4( &perObj.gWorld, XMMatrixTranspose( world ) );
			XMStoreFloat4x4( &perObj.gWorldInvTranspose, XMMatrixTranspose( worldInvTranspose ) );
			XMStoreFloat4x4( &perObj.gWorldViewProj, XMMatrixTranspose( worldViewProj ) );
			GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerObject, 0, NULL, &perObj, 0, 0 );


			GetD3D11DeviceImmContext()->VSSetShader( m_pGeomPassVS, NULL, 0 );
			GetD3D11DeviceImmContext()->PSSetShader( NULL, NULL, 0 );
			GetD3D11DeviceImmContext()->VSSetConstantBuffers( 0, 1, &m_pCBPerObject );


			if ( pElem->m_bDrawIndex )
			{

				GetD3D11DeviceImmContext()->DrawIndexed( (*itor).m_IndexCount, (*itor).m_StartIndexLocation, (*itor).m_BaseVertexLocation );
			}
			else
			{
				GetD3D11DeviceImmContext()->Draw( (*itor).m_VertexCount, (*itor).m_StartVertexLocation );
			}

		}
	}
}

void DeferredRenderer::DrawMRTPass()
{	
	XMMATRIX view = XMLoadFloat4x4( RendererCore::Instance()->GetView() );
	XMMATRIX proj = XMLoadFloat4x4( RendererCore::Instance()->GetProj() );
	XMMATRIX viewproj = view*proj;

	//G-buffer
	ID3D11RenderTargetView* pRenderTargets[3] = { m_spAlbedoRT->GetRTV(), m_spNormalRT->GetRTV(), m_spDepthRT->GetRTV() };
	GetD3D11DeviceImmContext()->OMSetRenderTargets( 3, pRenderTargets, RendererCore::Instance()->GetDSView() );

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GetD3D11DeviceImmContext()->ClearRenderTargetView( m_spAlbedoRT->GetRTV(), clearColor );
	GetD3D11DeviceImmContext()->ClearRenderTargetView( m_spNormalRT->GetRTV(), clearColor );
	GetD3D11DeviceImmContext()->ClearRenderTargetView( m_spDepthRT->GetRTV(), clearColor );
	/**/

	for ( auto itorElem = m_vDrawElements.begin() ; itorElem != m_vDrawElements.end() ; ++itorElem )
	{
		DrawElement* pElem = (*itorElem);
		if ( !pElem )
			continue;

		UINT stride = pElem->stride;
		UINT offset = pElem->offset;

		GetD3D11DeviceImmContext()->IASetInputLayout( pElem->m_pInputLayout );
		GetD3D11DeviceImmContext()->IASetPrimitiveTopology( pElem->ePrimitiveTopology );

		for ( auto itor = pElem->m_vecSubElement.begin(); itor != pElem->m_vecSubElement.end(); ++itor )
		{
			ID3D11Buffer* pVB = pElem->m_spVB->GetVB();
			GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );

			if ( pElem->m_spIB )
				GetD3D11DeviceImmContext()->IASetIndexBuffer( pElem->m_spIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

			pElem->m_spRenderState->SetStateAll( GetD3D11DeviceImmContext(), 0 );


			XMMATRIX world = XMLoadFloat4x4( &(*itor).m_World );
			//XMMATRIX shadowTransform = world * shadowTransform;
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose( world );
			XMMATRIX worldViewProj = world*viewproj;
			XMMATRIX texTransform = XMLoadFloat4x4( &(*itor).m_TexTransform );

			cbPerObject perObj;
			XMStoreFloat4x4( &perObj.gWorld, XMMatrixTranspose( world ) );
			XMStoreFloat4x4( &perObj.gWorldInvTranspose, XMMatrixTranspose( worldInvTranspose ) );
			XMStoreFloat4x4( &perObj.gWorldViewProj, XMMatrixTranspose( worldViewProj ) );
			GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerObject, 0, NULL, &perObj, 0, 0 );


			GetD3D11DeviceImmContext()->VSSetShader( m_pDeferredVS, NULL, 0 );
			GetD3D11DeviceImmContext()->PSSetShader( m_pDeferredPS, NULL, 0 );
			GetD3D11DeviceImmContext()->VSSetConstantBuffers( 0, 1, &m_pCBPerObject );

			ID3D11ShaderResourceView* pTexSRV[] = { (*itor).m_spDiffuseMap->GetSRV(), (*itor).m_spNormalMap->GetSRV() };
			GetD3D11DeviceImmContext()->PSSetShaderResources( 0, 2, pTexSRV );
			GetD3D11DeviceImmContext()->PSSetSamplers( 0, 1, &m_pLinearSampler );

			if ( pElem->m_bDrawIndex )
			{	

				GetD3D11DeviceImmContext()->DrawIndexed( (*itor).m_IndexCount, (*itor).m_StartIndexLocation, (*itor).m_BaseVertexLocation );
			}
			else
			{
				GetD3D11DeviceImmContext()->Draw( (*itor).m_VertexCount, (*itor).m_StartVertexLocation );
			}

		}
	}

 	ID3D11RenderTargetView* pClearRTV[] = { NULL, NULL, NULL };
 	GetD3D11DeviceImmContext()->OMSetRenderTargets( 3, pClearRTV, NULL );
}
void DeferredRenderer::DrawAmbientLightPass()
{
	XMMATRIX world = XMMatrixIdentity();

	UINT stride = sizeof( VertexPUV );
	UINT offset = 0;

	GetD3D11DeviceImmContext()->IASetInputLayout( m_pGeomPassInputLO );
	GetD3D11DeviceImmContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	if ( m_spAmbientLightVB )
	{ 
		ID3D11Buffer* pVB = m_spAmbientLightVB->GetVB();
		GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
	}

	if ( m_spAmbientLightIB )
		GetD3D11DeviceImmContext()->IASetIndexBuffer( m_spAmbientLightIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

	if ( m_spAmbientLightRS )
		m_spAmbientLightRS->SetStateAll( GetD3D11DeviceImmContext(), 0 );


	cbPerObject perObj;
	XMStoreFloat4x4( &perObj.gWorld, XMMatrixTranspose( world ) );
	XMStoreFloat4x4( &perObj.gWorldInvTranspose, XMMatrixTranspose( world ) );
	XMStoreFloat4x4( &perObj.gWorldViewProj, XMMatrixTranspose( world ) );
	//XMStoreFloat4x4( &perObj.gPostProjRescale, XMMatrixTranspose( postProjRescale ) );
	GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerObject, 0, NULL, &perObj, 0, 0 );

	XMFLOAT3  eyePosW = RendererCore::Instance()->GetEyePosW();
	Material mat;
	mat.Ambient = XMFLOAT4( 0.8f, 0.8f, 0.8f, 1.0f );
	mat.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	mat.Specular = XMFLOAT4( 0.4f, 0.4f, 0.4f, 16.0f );
	mat.Reflect = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );

	cbPerFrame perFrame;
	//perFrame.PtLight = (*itor);
	perFrame.EyePosW = eyePosW;
	perFrame.pad1 = 0.0f;
	perFrame.gMaterial = mat;
	GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerFrame, 0, NULL, &perFrame, 0, 0 );

	XMFLOAT4 ambientlight = XMFLOAT4( 0.15f, 0.15f, 0.15f, 0.15f );
	GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBAmbientLight, 0, NULL, &ambientlight, 0, 0 );

	GetD3D11DeviceImmContext()->VSSetShader( m_pAmbientLightVS, NULL, 0 );
	GetD3D11DeviceImmContext()->PSSetShader( m_pAmbientLightPS, NULL, 0 );
	GetD3D11DeviceImmContext()->VSSetConstantBuffers( 0, 1, &m_pCBPerObject );
	GetD3D11DeviceImmContext()->PSSetConstantBuffers( 0, 1, &m_pCBPerFrame );
	GetD3D11DeviceImmContext()->PSSetConstantBuffers( 1, 1, &m_pCBAmbientLight );


	ID3D11ShaderResourceView* pTexSRV[1] = { m_spAlbedoRT->GetSRV() };
	GetD3D11DeviceImmContext()->PSSetShaderResources( 0, 1, pTexSRV );
	GetD3D11DeviceImmContext()->PSSetSamplers( 0, 1, &m_pPointSampler );

	GetD3D11DeviceImmContext()->DrawIndexed( m_AmbientLightQuad.GetMesh().Indices.size(), 0, 0 );
}

void DeferredRenderer::DrawDirectionLightsPass()
{

}

void DeferredRenderer::DrawPointLightsPass()
{
	XMMATRIX view = XMLoadFloat4x4( RendererCore::Instance()->GetView() );
	XMMATRIX proj = XMLoadFloat4x4( RendererCore::Instance()->GetProj() );
	XMMATRIX viewproj = view*proj;

	for ( auto itorElem = m_vDrawLightingElements.begin() ; itorElem != m_vDrawLightingElements.end() ; ++itorElem )
	{
		DrawElement* pElem = (*itorElem);
		if ( !pElem )
			continue;

		UINT stride = pElem->stride;
		UINT offset = pElem->offset;


		for ( auto itor = pElem->m_vecSubElement.begin(); itor != pElem->m_vecSubElement.end(); ++itor )
		{
			
			//Stencil volume mask
			GetD3D11DeviceImmContext()->IASetInputLayout( m_pGeomPassInputLO );
			GetD3D11DeviceImmContext()->IASetPrimitiveTopology( pElem->ePrimitiveTopology );


			ID3D11Buffer* pVB = pElem->m_spVB->GetVB();
			GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );

			if ( pElem->m_spIB )
				GetD3D11DeviceImmContext()->IASetIndexBuffer( pElem->m_spIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

			pElem->m_spPreRenderState->SetStateAll( GetD3D11DeviceImmContext(), 0 );


			XMMATRIX world = XMLoadFloat4x4( &(*itor).m_World );
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose( world );
			XMMATRIX worldViewProj = world*viewproj;

			XMMATRIX tl = XMMatrixTranslation( 1.0f + 1.0f / (float)RendererCore::Instance()->ClientWidth(),
				-1.0f - 1.0f / (float)RendererCore::Instance()->ClientHeight(), 0.0f );
			XMMATRIX scale = XMMatrixScaling( 0.5f, -0.5f, 1.0f );
			XMMATRIX postProjRescale = tl * scale;

			cbPerObject perObj;
			XMStoreFloat4x4( &perObj.gWorld, XMMatrixTranspose( world ) );
			XMStoreFloat4x4( &perObj.gWorldInvTranspose, XMMatrixTranspose( worldInvTranspose ) );
			XMStoreFloat4x4( &perObj.gWorldViewProj, XMMatrixTranspose( worldViewProj ) );
			XMStoreFloat4x4( &perObj.gPostProjRescale, XMMatrixTranspose( postProjRescale ) );
			GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerObject, 0, NULL, &perObj, 0, 0 );


			GetD3D11DeviceImmContext()->VSSetShader( m_pGeomPassVS, NULL, 0 );
			GetD3D11DeviceImmContext()->PSSetShader( NULL, NULL, 0 );
			GetD3D11DeviceImmContext()->VSSetConstantBuffers( 0, 1, &m_pCBPerObject );

			
			if ( pElem->m_bDrawIndex )
			{

				GetD3D11DeviceImmContext()->DrawIndexed( (*itor).m_IndexCount, (*itor).m_StartIndexLocation, (*itor).m_BaseVertexLocation );
			}
			else
			{
				GetD3D11DeviceImmContext()->Draw( (*itor).m_VertexCount, (*itor).m_StartVertexLocation );
			}
			/**/

			
			XMFLOAT3  eyePosW = RendererCore::Instance()->GetEyePosW();
			Material mat;
			mat.Ambient = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			mat.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			mat.Specular = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			mat.Reflect = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );

			cbPerFrame perFrame;
			//XMStoreFloat4x4( &perFrame.gProjInv, projInv );
			perFrame.gPointLight = pElem->m_PointLight;
			perFrame.EyePosW = eyePosW;
			perFrame.pad1 = 0.0f;
			perFrame.gMaterial = mat;


			GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerFrame, 0, NULL, &perFrame, 0, 0 );
			GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerObject, 0, NULL, &perObj, 0, 0 );

			//Light 
			pElem->m_spRenderState->SetStateAll( GetD3D11DeviceImmContext(), 1 );

			GetD3D11DeviceImmContext()->IASetInputLayout( m_pFinalInputLO );
			GetD3D11DeviceImmContext()->IASetPrimitiveTopology( pElem->ePrimitiveTopology );


			GetD3D11DeviceImmContext()->VSSetShader( m_pFinalPassVS, NULL, 0 );
			GetD3D11DeviceImmContext()->PSSetShader( m_pFinalPassPS, NULL, 0 );
			GetD3D11DeviceImmContext()->VSSetConstantBuffers( 0, 1, &m_pCBPerObject );
			GetD3D11DeviceImmContext()->PSSetConstantBuffers( 0, 1, &m_pCBPerFrame );

			ID3D11ShaderResourceView* pTexSRV[3] = { m_spAlbedoRT->GetSRV(), m_spNormalRT->GetSRV(), m_spDepthRT->GetSRV() };
			GetD3D11DeviceImmContext()->PSSetShaderResources( 0, 3, pTexSRV );
			GetD3D11DeviceImmContext()->PSSetSamplers( 0, 1, &m_pPointSampler );

			if ( pElem->m_bDrawIndex )
			{

				GetD3D11DeviceImmContext()->DrawIndexed( (*itor).m_IndexCount, (*itor).m_StartIndexLocation, (*itor).m_BaseVertexLocation );
			}
			else
			{
				GetD3D11DeviceImmContext()->Draw( (*itor).m_VertexCount, (*itor).m_StartVertexLocation );
			}
			/**/
		}
	}
}

void DeferredRenderer::DrawSpotLightsPass()
{

}

void DeferredRenderer::DrawFinalPass()
{
// 	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
// 
// 	RendererCore::Instance()->SetOriginalRenderTargetDepth();
// 
// 
// 	XMMATRIX world = XMMatrixIdentity();
// 	XMMATRIX shadowTransform = world;// XMLoadFloat4x4();
// 	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose( world );
// 	XMMATRIX worldViewProj = world*m_ViewProj;
// 	XMMATRIX texTransform = world;// XMLoadFloat4x4( &(*itor).m_TexTransform );
//  	XMMATRIX tl = XMMatrixTranslation( 1.0f + 1.0f / (float)RendererCore::Instance()->ClientWidth(),
//  		-1.0f - 1.0f / (float)RendererCore::Instance()->ClientWidth(), 0.0f );
//  	XMMATRIX scale = XMMatrixScaling( 0.5f, -0.5f, 1.0f );
//  	XMMATRIX postProjRescale = tl * scale;
// 
// 	XMVECTOR det = XMMatrixDeterminant( postProjRescale );
// 	XMMATRIX projInv = XMMatrixInverse( &det, m_ViewProj );
// 
// 	int iIndex = 0;
// 	for ( auto itorElem = m_vDrawStencilElements.begin() ; itorElem != m_vDrawStencilElements.end() ; ++itorElem )
// 	{
// 		DrawElement* pElem = (*itorElem);
// 		if ( !pElem )
// 			continue;
// 
// 		PointLight ptLight = m_vecPointLights[iIndex];
// 
// 		UINT stride = pElem->stride;
// 		UINT offset = pElem->offset;
// 
// 		GetD3D11DeviceImmContext()->IASetInputLayout( pElem->m_pInputLayout );
// 		GetD3D11DeviceImmContext()->IASetPrimitiveTopology( pElem->ePrimitiveTopology );
// 
// 		for ( auto itor = pElem->m_vecSubElement.begin(); itor != pElem->m_vecSubElement.end(); ++itor )
// 		{
// 			ID3D11Buffer* pVB = pElem->m_spVB->GetVB();
// 			GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
// 
// 			if ( pElem->m_spIB )
// 				GetD3D11DeviceImmContext()->IASetIndexBuffer( pElem->m_spIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );
// 
// 			if ( pElem->m_pRasterS )
// 				GetD3D11DeviceImmContext()->RSSetState( pElem->m_pRasterS );
// 			if ( pElem->m_pBlendS )
// 				GetD3D11DeviceImmContext()->OMSetBlendState( pElem->m_pBlendS, blendFactor, 0xffffffff );
// 			if ( pElem->m_pDepthStencilS )
// 				GetD3D11DeviceImmContext()->OMSetDepthStencilState( pElem->m_pDepthStencilS, 1 );
// 
// 			cbPerObject perObj;
// 			XMStoreFloat4x4( &perObj.gWorld, XMMatrixTranspose( world ) );
// 			XMStoreFloat4x4( &perObj.gWorldInvTranspose, XMMatrixTranspose( world ) );
// 			XMStoreFloat4x4( &perObj.gWorldViewProj, XMMatrixTranspose( world ) );
// 			XMStoreFloat4x4( &perObj.gWorldViewProj, XMMatrixTranspose( world ) );
// 			XMStoreFloat4x4( &perObj.gPostProjRescale, XMMatrixTranspose( postProjRescale ) );
// 			GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerObject, 0, NULL, &perObj, 0, 0 );
// 
// 			XMFLOAT3  eyePosW = RendererCore::Instance()->GetEyePosW();
// 			Material mat;
// 			mat.Ambient = XMFLOAT4( 0.8f, 0.8f, 0.8f, 1.0f );
// 			mat.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
// 			mat.Specular = XMFLOAT4( 0.4f, 0.4f, 0.4f, 16.0f );
// 			mat.Reflect = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
// 
// 			cbPerFrame perFrame;
// 			XMStoreFloat4x4( &perFrame.gProjInv, projInv );
// 			perFrame.gPointLight = ptLight;
// 			perFrame.EyePosW = eyePosW;
// 			perFrame.pad1 = 0.0f;
// 			perFrame.gMaterial = mat;
// 			GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerFrame, 0, NULL, &perFrame, 0, 0 );
// 
// // 			D3DXVECTOR3 pos( perFrame.PtLight.Position.x, perFrame.PtLight.Position.y, perFrame.PtLight.Position.z );
// // 			D3D11_RECT rect;
// // 			rect = DetermineClipRect( pos, perFrame.PtLight.Range );
// // 			GetD3D11DeviceImmContext()->RSSetScissorRects( 1, &rect );
// 
// 			GetD3D11DeviceImmContext()->VSSetShader( m_pFinalPassVS, NULL, 0 );
// 			GetD3D11DeviceImmContext()->PSSetShader( m_pFinalPassPS, NULL, 0 );
// 			GetD3D11DeviceImmContext()->VSSetConstantBuffers( 0, 1, &m_pCBPerObject );
// 			GetD3D11DeviceImmContext()->PSSetConstantBuffers( 0, 1, &m_pCBPerFrame );
// 
// 
// 			ID3D11ShaderResourceView* pTexSRV[3] = { m_spAlbedoRT->GetSRV(), m_spNormalRT->GetSRV(), m_spDepthRT->GetSRV() };
// 			GetD3D11DeviceImmContext()->PSSetShaderResources( 0, 3, pTexSRV );
// 			GetD3D11DeviceImmContext()->PSSetSamplers( 0, 1, &m_pPointSampler );
// 
// 			if ( pElem->m_bDrawIndex )
// 			{
// 
// 				GetD3D11DeviceImmContext()->DrawIndexed( (*itor).m_IndexCount, (*itor).m_StartIndexLocation, (*itor).m_BaseVertexLocation );
// 			}
// 			else
// 			{
// 				GetD3D11DeviceImmContext()->Draw( (*itor).m_VertexCount, (*itor).m_StartVertexLocation );
// 			}
// 		}
// 	}
}

void DeferredRenderer::DrawScreenQuad()
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	int iCount = m_vDrawScreenElements.size();
	for ( int i = 0; i < iCount; ++i )
	{
		DrawElement* pElem = m_vDrawScreenElements[i];
		if ( !pElem )
			continue;

		UINT stride = pElem->stride;
		UINT offset = pElem->offset;

		ID3D11Buffer* pVB = pElem->m_spVB->GetVB();

		GetD3D11DeviceImmContext()->IASetInputLayout( pElem->m_pInputLayout );
		GetD3D11DeviceImmContext()->IASetPrimitiveTopology( pElem->ePrimitiveTopology );
		GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
		GetD3D11DeviceImmContext()->IASetIndexBuffer( pElem->m_spIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );


		if ( pElem->m_pRasterS )
			GetD3D11DeviceImmContext()->RSSetState( pElem->m_pRasterS );
		if ( pElem->m_pBlendS )
			GetD3D11DeviceImmContext()->OMSetBlendState( pElem->m_pBlendS, blendFactor, 0xffffffff );
		if ( pElem->m_pDepthStencilS )
			GetD3D11DeviceImmContext()->OMSetDepthStencilState( pElem->m_pDepthStencilS, pElem->m_uiStencilRef );



		for ( auto itor = pElem->m_vecSubElement.begin(); itor < pElem->m_vecSubElement.end(); ++itor )
		{
			ID3DX11EffectTechnique* pTech = pElem->m_spShader->GetTech( (*itor).m_iTechIndex );
			if ( !pTech )
				continue;

			D3DX11_TECHNIQUE_DESC techDesc;
			pTech->GetDesc( &techDesc );

			TexturePtr spDiffuseMap = (*itor).m_spDiffuseMap;

			for ( UINT p = 0; p < techDesc.Passes; ++p )
			{
				XMMATRIX worldViewProj = XMLoadFloat4x4( &(*itor).m_World );

				pElem->m_spShader->SetWorldViewProj( worldViewProj );
				pElem->m_spShader->SetDiffuseMap( m_spAlbedoRT->GetSRV() );
				//pElem->m_spShader->SetDiffuseMap( REnd );

				pTech->GetPassByIndex( p )->Apply( 0, GetD3D11DeviceImmContext() );
				GetD3D11DeviceImmContext()->DrawIndexed( 6, 0, 0 );
			}
		}
	}
}

void DeferredRenderer::DrawDebugStencil()
{
	XMMATRIX view = XMLoadFloat4x4( RendererCore::Instance()->GetView() );
	XMMATRIX proj = XMLoadFloat4x4( RendererCore::Instance()->GetProj() );
	XMMATRIX viewproj = view*proj;

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	RendererCore::Instance()->SetOriginalRenderTargetDepth();

	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX shadowTransform = world;// XMLoadFloat4x4();
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose( world );
	XMMATRIX worldViewProj = world*viewproj;
	XMMATRIX texTransform = world;// XMLoadFloat4x4( &(*itor).m_TexTransform );
	XMMATRIX tl = XMMatrixTranslation( 1.0f + 1.0f / (float)RendererCore::Instance()->ClientWidth(),
	 	-1.0f - 1.0f / (float)RendererCore::Instance()->ClientWidth(), 0.0f );
	XMMATRIX scale = XMMatrixScaling( 0.5f, -0.5f, 1.0f );
	XMMATRIX postProjRescale = tl * scale;

	UINT stride = sizeof( VertexPUV );
	UINT offset = 0;

	GetD3D11DeviceImmContext()->IASetInputLayout( m_pFinalInputLO );
	GetD3D11DeviceImmContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	ID3D11Buffer* pVB = m_spScreenVB->GetVB();
	GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );

	if ( m_spScreenVB )
		GetD3D11DeviceImmContext()->IASetIndexBuffer( m_spScreenIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

	m_spDebugRS->SetStateAll( GetD3D11DeviceImmContext(), 1 );


	cbPerObject perObj;
	XMStoreFloat4x4( &perObj.gWorld, XMMatrixTranspose( world ) );
	XMStoreFloat4x4( &perObj.gWorldInvTranspose, XMMatrixTranspose( world ) );
	XMStoreFloat4x4( &perObj.gWorldViewProj, XMMatrixTranspose( world ) );
	XMStoreFloat4x4( &perObj.gPostProjRescale, XMMatrixTranspose( postProjRescale ) );
	GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerObject, 0, NULL, &perObj, 0, 0 );

	XMFLOAT3  eyePosW = RendererCore::Instance()->GetEyePosW();
	Material mat;
	mat.Ambient = XMFLOAT4( 0.8f, 0.8f, 0.8f, 1.0f );
	mat.Diffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	mat.Specular = XMFLOAT4( 0.4f, 0.4f, 0.4f, 16.0f );
	mat.Reflect = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );

	cbPerFrame perFrame;
	//perFrame.PtLight = (*itor);
	perFrame.EyePosW = eyePosW;
	perFrame.pad1 = 0.0f;
	perFrame.gMaterial = mat;
	GetD3D11DeviceImmContext()->UpdateSubresource( m_pCBPerFrame, 0, NULL, &perFrame, 0, 0 );


	GetD3D11DeviceImmContext()->VSSetShader( m_pFinalPassVS, NULL, 0 );
	GetD3D11DeviceImmContext()->PSSetShader( m_pDebugPassPS, NULL, 0 );
	GetD3D11DeviceImmContext()->VSSetConstantBuffers( 0, 1, &m_pCBPerObject );
	GetD3D11DeviceImmContext()->PSSetConstantBuffers( 0, 1, &m_pCBPerFrame );


	ID3D11ShaderResourceView* pTexSRV[1] = { m_spDebugTex->GetSRV() };
	GetD3D11DeviceImmContext()->PSSetShaderResources( 0, 1, pTexSRV );
	GetD3D11DeviceImmContext()->PSSetSamplers( 0, 1, &m_pPointSampler );

	GetD3D11DeviceImmContext()->DrawIndexed( m_DebugStencilQuad.GetMesh().Indices.size(), 0, 0 );

}


float GetHeight( float x, float z )
{
	return 0.3f*(z*sinf( 0.1f*x ) + x*cosf( 0.1f*z ));
}

XMFLOAT3 GetHillNormal( float x, float z )
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf( 0.1f*x ) - 0.3f*cosf( 0.1f*z ),
		1.0f,
		-0.3f*sinf( 0.1f*x ) + 0.03f*x*sinf( 0.1f*z ) );

	XMVECTOR unitNormal = XMVector3Normalize( XMLoadFloat3( &n ) );
	XMStoreFloat3( &n, unitNormal );

	return n;
}

void DeferredRenderer::BuildBoxBuffers()
{
	MeshDataPNTUV& boxes = m_BoxesMesh.GetMesh();

	MeshDataPNTUV meshData;
	GeometryGenerator geoGen;
	geoGen.CreateBox( 3.0f, 3.0f, 3.0f, meshData );

	
	boxes.RealV.resize( meshData.Vertices.size() );
	for ( size_t i = 0; i < meshData.Vertices.size(); ++i )
	{
		boxes.RealV[i].Position = meshData.Vertices[i].Position;
		boxes.RealV[i].Normal = meshData.Vertices[i].Normal;
		boxes.RealV[i].TexC = meshData.Vertices[i].TexC;
		boxes.RealV[i].TangentU = meshData.Vertices[i].TangentU;
	}

	boxes.Indices.insert( boxes.Indices.end(), meshData.Indices.begin(), meshData.Indices.end() );


	int iTotal = 40;
	int iFactor = 8;
	float fWidth = 4.0f;
	float fOffsetX = -0.5f * iFactor * fWidth;
	float fOffsetY = -0.5f * (iTotal / iFactor)* fWidth;

	char strObj[128];
	int w = 0, h = 0;
	for ( int i = 0 ; i < iTotal ; ++i )
	{
		sprintf_s( strObj,  "DF_box_%d", i );
		string strName( strObj );

		w = (i) % iFactor ;
		h = (i) / iFactor;

		DrawElement* pElem = GenerateDFMeshElement( sizeof( VertexPNTUV )* boxes.RealV.size(),
			strName, m_BoxesMesh, "mrtpass", "PNTUV" );
		{
			pElem->stride = sizeof( VertexPNTUV );
			pElem->m_pInputLayout = m_pDefaultIO;

			XMMATRIX world = XMMatrixTranslation( fWidth*w + fOffsetX, 0.0f, fWidth*h + fOffsetY );
			XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_World, world );
			XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_TexTransform, XMMatrixIdentity() );

			pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
			pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
			pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4( 0.8f, 0.8f, 0.8f, 16.0f );
			pElem->m_vecSubElement[0].m_mat.Reflect = XMFLOAT4( 0.5f, 0.5f, 0.5f, 1.0f );

			pElem->m_vecSubElement[0].m_spDiffuseMap = RendererCore::Instance()->GetTexture( "bricks" );
			pElem->m_vecSubElement[0].m_spNormalMap = RendererCore::Instance()->GetTexture( "bricks_nmap" );
		}

		RendererCore::Instance()->AddDrawElem( pElem );

	}

}

void DeferredRenderer::BuildGridBuffers()
{
	MeshDataPNTUV& grid = m_GridMesh.GetMesh();

	MeshDataPNTUV meshData;
	GeometryGenerator geoGen;
	geoGen.CreateGrid( 100.0f, 100.0f, 50, 40, meshData );

	grid.RealV.resize( meshData.Vertices.size() );
	for ( size_t i = 0; i < meshData.Vertices.size(); ++i )
	{
		XMFLOAT3 p = meshData.Vertices[i].Position;

		//p.y = GetHeight( p.x, p.z );

		grid.RealV[i].Position = p;
		//grid.RealV[i].Normal = GetHillNormal( p.x, p.z );
		grid.RealV[i].Normal = meshData.Vertices[i].Normal;
		grid.RealV[i].TexC = meshData.Vertices[i].TexC;
		grid.RealV[i].TangentU = meshData.Vertices[i].TangentU;
	}

	grid.Indices.insert( grid.Indices.end(), meshData.Indices.begin(), meshData.Indices.end() );

	DrawElement* pElem = GenerateDFMeshElement( sizeof( VertexPNTUV )* grid.RealV.size(),
		"Grid", m_GridMesh, "cull", "PNTUV" );
	{
		pElem->stride = sizeof( VertexPNTUV );
		pElem->m_pInputLayout = m_pDefaultIO;

		XMMATRIX world = XMMatrixTranslation( 0.0f, 0.0f, 0.0f );
		XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_World, world );
		XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_TexTransform, XMMatrixScaling( 8.0f, 10.0f, 1.0f ) );

		pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
		pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
		pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4( 0.8f, 0.8f, 0.8f, 16.0f );
		pElem->m_vecSubElement[0].m_mat.Reflect = XMFLOAT4( 0.5f, 0.5f, 0.5f, 1.0f );

		pElem->m_vecSubElement[0].m_spDiffuseMap = RendererCore::Instance()->GetTexture( "bricks" );
		pElem->m_vecSubElement[0].m_spNormalMap = RendererCore::Instance()->GetTexture( "bricks_nmap" );
	}

	RendererCore::Instance()->AddDrawElem( pElem );
}

void DeferredRenderer::BuildScreenQuadBufferByDebugStencil()
{
	MeshDataPUV& meshData = m_DebugStencilQuad.GetMesh();

	MeshDataPNTUV Quad;
	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad( Quad );

	meshData.RealV.resize( Quad.Vertices.size() );
	for ( int i = 0; i < (int)Quad.Vertices.size(); ++i )
	{
		meshData.RealV[i].Pos = Quad.Vertices[i].Position;
		meshData.RealV[i].UV = Quad.Vertices[i].TexC;
	}

	meshData.Indices.insert( meshData.Indices.end(), Quad.Indices.begin(), Quad.Indices.end() );


	m_spScreenVB = VertexBuffer::Create( sizeof( VertexPUV )* meshData.RealV.size(), m_DebugStencilQuad.GetData(), false );
	RendererCore::Instance()->InsertVB( "FinalSC", m_spScreenVB );

	m_spScreenIB = IndexBuffer::Create( sizeof( UINT )* meshData.Indices.size(), &meshData.Indices[0], false );
	RendererCore::Instance()->InsertIB( "FinalSC", m_spScreenIB );

	m_spDebugRS = RendererCore::Instance()->GetRenderState( "stencilclippedpixels" );
	m_spDebugTex = RendererCore::Instance()->GetTexture( "stones" );
}

void DeferredRenderer::BuildSpherebyStencilMask()
{
	MeshDataPUV& sphere = m_SphereLight.GetMesh();
	GeometryGenerator geoGen;

	float fRange = 10.0f;
	float fRadius = 5.0f;
	MeshDataPNTUV tempData;
	//geoGen.CreateGrid( 100.0f, 100.0f, 50, 40, grid );
	//geoGen.CreateBox( 3.0f, 3.0f, 3.0f, grid );
	geoGen.CreateSphere( fRadius, 10, 10, tempData );

	sphere.RealV.clear();
	sphere.Indices.clear();
	sphere.RealV.resize( tempData.Vertices.size() );
	for ( size_t i = 0; i < tempData.Vertices.size(); ++i )
	{
		sphere.RealV[i].Pos = tempData.Vertices[i].Position;
		sphere.RealV[i].UV = tempData.Vertices[i].TexC;
	}

	sphere.Indices.insert( sphere.Indices.end(), tempData.Indices.begin(), tempData.Indices.end() );

	char strObj[128];
	int w = 0, h = 0;
	for ( int i = 0 ; i < 2 ; ++i )
	{
		sprintf_s( strObj, "Sm_sphere_%d", i );
		string strName( strObj );

		w = (i) % 2 ;
		h = (i) / 2;

		DrawElement* pElem = GenerateDFMeshElement( sizeof( VertexPUV )* sphere.RealV.size(),
			strName, m_SphereLight, "stencilvolumemask", "PUV" );
		{
			pElem->stride = sizeof( VertexPUV );
			pElem->m_pInputLayout = m_pFinalInputLO;

			XMFLOAT3 pos = XMFLOAT3( fRange*w, 3.0f, fRange*h );

			XMMATRIX world = XMMatrixTranslation( pos.x, pos.y, pos.z );
			XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_World, world );
			XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_TexTransform, XMMatrixIdentity() );

			pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
			pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
			pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4( 0.8f, 0.8f, 0.8f, 16.0f );
			pElem->m_vecSubElement[0].m_mat.Reflect = XMFLOAT4( 0.5f, 0.5f, 0.5f, 1.0f );

			pElem->m_vecSubElement[0].m_spDiffuseMap = RendererCore::Instance()->GetTexture( "stones" );
			pElem->m_vecSubElement[0].m_spNormalMap = RendererCore::Instance()->GetTexture( "stones_nmap" );
		}

		

		PointLight ptL;
		ptL.Ambient = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		ptL.Diffuse = XMFLOAT4( 0.4f, 0.4f, 0.0f, 1.0f );
		ptL.Specular = XMFLOAT4( 0.7f, 0.7f, 0.7f, 1.0f );
		ptL.Att = XMFLOAT3( 0.0f, 0.1f, 0.0f );
		ptL.Range = fRange;
		ptL.Position = XMFLOAT3( fRange*w, 3.0f, fRange*h );
		m_vecPointLights.push_back( ptL );

		RendererCore::Instance()->AddStencilElem( pElem );
	}
}

void DeferredRenderer::BuildSpherebyDiffuseLight()
{
	MeshDataPUV& sphere = m_SphereLight.GetMesh();
	GeometryGenerator geoGen;

	float fWidth = 8.0f;
	float fRadius = 6.0f;
	MeshDataPNTUV tempData;
	geoGen.CreateSphere( fRadius, 100, 100, tempData );

	sphere.RealV.clear();
	sphere.Indices.clear();
	sphere.RealV.resize( tempData.Vertices.size() );
	for ( size_t i = 0; i < tempData.Vertices.size(); ++i )
	{
		sphere.RealV[i].Pos = tempData.Vertices[i].Position;
		sphere.RealV[i].UV = tempData.Vertices[i].TexC;
	}

	sphere.Indices.insert( sphere.Indices.end(), tempData.Indices.begin(), tempData.Indices.end() );

	std::srand( (unsigned int)std::time( 0 ) );

	char strObj[128];
	int w = 0, h = 0;

	for ( int i = 0 ; i < 10 ; ++i )
	{
		sprintf_s( strObj, "DF_sphere_%d", i );
		string strName( strObj );

		w = (i) % 2 ;
		h = (i) / 2;


		int iR = std::rand();
		int iG= std::rand();
		int iB = std::rand();
		float r = float(iR) / float( RAND_MAX );
		float g = float(iG) / float( RAND_MAX );
		float b = float(iB) / float( RAND_MAX );

		DrawElement* pElem = GenerateDFMeshElement( sizeof( VertexPUV )* sphere.RealV.size(),
			strName, m_SphereLight, "stencilclippedpixels", "PUV" );
		{
			pElem->stride = sizeof( VertexPUV );
			pElem->m_pInputLayout = m_pFinalInputLO;

			XMFLOAT3 pos = XMFLOAT3( fWidth*w, 3.0f, fWidth*h );

			XMMATRIX world = XMMatrixTranslation( pos.x, pos.y, pos.z );
			XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_World, world );
			XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_TexTransform, XMMatrixIdentity() );

			pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
			pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
			pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4( 0.8f, 0.8f, 0.8f, 16.0f );
			pElem->m_vecSubElement[0].m_mat.Reflect = XMFLOAT4( 0.5f, 0.5f, 0.5f, 1.0f );

			pElem->m_vecSubElement[0].m_spDiffuseMap = RendererCore::Instance()->GetTexture( "stones" );
			pElem->m_vecSubElement[0].m_spNormalMap = RendererCore::Instance()->GetTexture( "stones_nmap" );
		}

		pElem->m_spPreRenderState = RendererCore::Instance()->GetRenderState( "stencilvolumemask" );

		PointLight ptL;
		ptL.Ambient = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
		ptL.Diffuse = XMFLOAT4( r, g, b, 1.0f );
		ptL.Specular = XMFLOAT4( 0.7f, 0.7f, 0.7f, 1.0f );
		ptL.Att = XMFLOAT3( 0.0f, 0.4f, 0.0f );
		ptL.Range = fRadius;
		ptL.Position = XMFLOAT3( fWidth*w, 3.0f, fWidth*h );
		pElem->m_PointLight = ptL;

		m_vDrawLightingElements.push_back( pElem );
	}
}

void DeferredRenderer::BuildQuadBufferByAmbientLight()
{
	MeshDataPUV& meshData = m_AmbientLightQuad.GetMesh();

	MeshDataPNTUV Quad;
	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad( Quad );

	meshData.RealV.resize( Quad.Vertices.size() );
	for ( int i = 0; i < (int)Quad.Vertices.size(); ++i )
	{
		meshData.RealV[i].Pos = Quad.Vertices[i].Position;
		meshData.RealV[i].UV = Quad.Vertices[i].TexC;
	}

	meshData.Indices.insert( meshData.Indices.end(), Quad.Indices.begin(), Quad.Indices.end() );

	m_spAmbientLightVB = VertexBuffer::Create( sizeof( VertexPUV )* meshData.RealV.size(), m_AmbientLightQuad.GetData(), false );
	RendererCore::Instance()->InsertVB( "AlVB", m_spAmbientLightVB );

	m_spAmbientLightIB = IndexBuffer::Create( sizeof( UINT )* meshData.Indices.size(), &meshData.Indices[0], false );
	RendererCore::Instance()->InsertIB( "AlIB", m_spAmbientLightIB );

	m_spAmbientLightRS = RendererCore::Instance()->GetRenderState( "ambientlight" );
}

RECT DeferredRenderer::DetermineClipRect( const D3DXVECTOR3& position, const float range )
{
	//compute 3D bounding box of light in world space
	D3DXVECTOR3 bbox3D[8];
	bbox3D[0].x = position.x - range;  bbox3D[0].y = position.y + range;  bbox3D[0].z = position.z - range;
	bbox3D[1].x = position.x + range;  bbox3D[1].y = position.y + range;  bbox3D[1].z = position.z - range;
	bbox3D[2].x = position.x - range;  bbox3D[2].y = position.y - range;  bbox3D[2].z = position.z - range;
	bbox3D[3].x = position.x + range;  bbox3D[3].y = position.y - range;  bbox3D[3].z = position.z - range;
	bbox3D[4].x = position.x - range;  bbox3D[4].y = position.y + range;  bbox3D[4].z = position.z + range;
	bbox3D[5].x = position.x + range;  bbox3D[5].y = position.y + range;  bbox3D[5].z = position.z + range;
	bbox3D[6].x = position.x - range;  bbox3D[6].y = position.y - range;  bbox3D[6].z = position.z + range;
	bbox3D[7].x = position.x + range;  bbox3D[7].y = position.y - range;  bbox3D[7].z = position.z + range;


	D3DXMATRIX view = (const float*)(RendererCore::Instance()->GetView());
	D3DXMATRIX proj = (const float*)(RendererCore::Instance()->GetProj());

	//project coordinates
	D3DXMATRIX viewProjMat = view*proj;
	D3DXVECTOR2 projBox[8];
	for ( int i = 0; i < 8; ++i )
	{
		D3DXVECTOR4 projPoint;
		D3DXVec3Transform( &projPoint, &bbox3D[i], &viewProjMat );
		projBox[i].x = projPoint.x / projPoint.w;
		projBox[i].y = projPoint.y / projPoint.w;

		//clip to extents
		if ( projBox[i].x < -1.0f )
			projBox[i].x = -1.0f;
		else if ( projBox[i].x > 1.0f )
			projBox[i].x = 1.0f;
		if ( projBox[i].y < -1.0f )
			projBox[i].y = -1.0f;
		else if ( projBox[i].y > 1.0f )
			projBox[i].y = 1.0f;

		//go to pixel coordinates
		projBox[i].x = ((projBox[i].x + 1.0f) / 2.0f) * RendererCore::Instance()->ClientWidth();
		projBox[i].y = ((-projBox[i].y + 1.0f) / 2.0f) * RendererCore::Instance()->ClientHeight();
	}

	//compute 2D bounding box of projected coordinates
	unsigned int minX = 0xFFFFFFFF;
	unsigned int maxX = 0x00000000;
	unsigned int minY = 0xFFFFFFFF;
	unsigned int maxY = 0x00000000;
	for ( int i = 0; i < 8; ++i )
	{
		unsigned int x = static_cast<unsigned int>(projBox[i].x);
		unsigned int y = static_cast<unsigned int>(projBox[i].y);
		if ( x < minX )
			minX = x;
		if ( x > maxX )
			maxX = x;
		if ( y < minY )
			minY = y;
		if ( y > maxY )
			maxY = y;
	}
	RECT bbox2D;
	bbox2D.top = minY;
	bbox2D.bottom = maxY;
	bbox2D.left = minX;
	bbox2D.right = maxX;

	return bbox2D;
}