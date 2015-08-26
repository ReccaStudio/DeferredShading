#include "stdafx.h"
#include "ForwardRenderer.h"
#include "RendererCore.h"


ForwardRenderer::ForwardRenderer() : BaseRenderer(),
m_fLightRotationAngle( 0.0f )
{
	m_DirLights[0].Ambient = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_DirLights[0].Diffuse = XMFLOAT4( 0.7f, 0.7f, 0.6f, 1.0f );
	m_DirLights[0].Specular = XMFLOAT4( 0.8f, 0.8f, 0.7f, 1.0f );
	m_DirLights[0].Direction = XMFLOAT3( -0.57735f, -0.57735f, 0.57735f );


	m_DirLights[1].Ambient = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_DirLights[1].Diffuse = XMFLOAT4( 0.40f, 0.40f, 0.40f, 1.0f );
	m_DirLights[1].Specular = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_DirLights[1].Direction = XMFLOAT3( 0.707f, -0.707f, 0.0f );

	m_DirLights[2].Ambient = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_DirLights[2].Diffuse = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_DirLights[2].Specular = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.0f );
	m_DirLights[2].Direction = XMFLOAT3( 0.0f, 0.0, -1.0f );

	m_OriginalLightDir[0] = m_DirLights[0].Direction;
	m_OriginalLightDir[1] = m_DirLights[1].Direction;
	m_OriginalLightDir[2] = m_DirLights[2].Direction;

	// Point light--position is changed every frame to animate in UpdateScene function.
	m_PointLight.Ambient = XMFLOAT4( 0.3f, 0.3f, 0.3f, 1.0f );
	m_PointLight.Diffuse = XMFLOAT4( 0.7f, 0.7f, 0.7f, 1.0f );
	m_PointLight.Specular = XMFLOAT4( 0.7f, 0.7f, 0.7f, 1.0f );
	m_PointLight.Att = XMFLOAT3( 0.0f, 0.1f, 0.0f );
	m_PointLight.Range = 25.0f;

	// Spot light--position and direction changed every frame to animate in UpdateScene function.
	m_SpotLight.Ambient = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
	m_SpotLight.Diffuse = XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f );
	m_SpotLight.Specular = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_SpotLight.Att = XMFLOAT3( 1.0f, 0.0f, 0.0f );
	m_SpotLight.Spot = 96.0f;
	m_SpotLight.Range = 10000.0f;

	m_SceneBounds.Center = XMFLOAT3( 0.0f, 0.0f, 0.0f );
	m_SceneBounds.Radius = sqrtf( 10.0f*10.0f + 15.0f*15.0f );
}


ForwardRenderer::~ForwardRenderer()
{
}

void ForwardRenderer::Init()
{

}

void ForwardRenderer::Resize()
{

}

void ForwardRenderer::Update( float dt )
{
	//
	m_fLightRotationAngle += 0.1f*dt;

	XMMATRIX R = XMMatrixRotationY( m_fLightRotationAngle );
	for ( int i = 0; i < 3; ++i )
	{
		XMVECTOR lightDir = XMLoadFloat3( &m_OriginalLightDir[i] );
		lightDir = XMVector3TransformNormal( lightDir, R );
		XMStoreFloat3( &m_DirLights[i].Direction, lightDir );
	}

	BuildShadowTransform();
}

void ForwardRenderer::DrawScene()
{
	DrawShadowMap();

	RendererCore::Instance()->SetOriginalRenderTargetDepth();

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set constants
	XMMATRIX view = XMLoadFloat4x4( RendererCore::Instance()->GetView() );
	XMMATRIX proj = XMLoadFloat4x4( RendererCore::Instance()->GetProj() );

	XMMATRIX shadowTransform = XMLoadFloat4x4( &m_ShadowTransform );


	int iCount = m_vDrawElements.size();
	for ( int i = 0; i < iCount; ++i )
	{
		DrawElement* pElem = m_vDrawElements[i];
		if ( !pElem )
			continue;

		UINT stride = pElem->stride;
		UINT offset = pElem->offset;

		GetD3D11DeviceImmContext()->IASetInputLayout( pElem->m_pInputLayout );
		GetD3D11DeviceImmContext()->IASetPrimitiveTopology( pElem->ePrimitiveTopology );

		pElem->m_spShader->SetDirLight( m_DirLights[0] );
		pElem->m_spShader->SetDirLights( m_DirLights );
		pElem->m_spShader->SetPointLight( m_PointLight );
		pElem->m_spShader->SetSpotLight( m_SpotLight );
		pElem->m_spShader->SetEyePosW( RendererCore::Instance()->GetEyePosW() );
		pElem->m_spShader->SetFogStart( 15.0f );
		pElem->m_spShader->SetFogRange( 300.0f );
		pElem->m_spShader->SetFogColor( Colors::Silver );

		for ( auto itor = pElem->m_vecSubElement.begin(); itor != pElem->m_vecSubElement.end() ; ++itor )
		{
			ID3DX11EffectTechnique* pTech = pElem->m_spShader->GetTech( (*itor).m_iTechIndex );
			if ( !pTech )
				continue;


			ID3D11Buffer* pVB = pElem->m_spVB->GetVB();
			GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );

			if ( pElem->m_spIB )
				GetD3D11DeviceImmContext()->IASetIndexBuffer( pElem->m_spIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

			if ( pElem->m_pRasterS )
				GetD3D11DeviceImmContext()->RSSetState( pElem->m_pRasterS );
			if ( pElem->m_pBlendS )
				GetD3D11DeviceImmContext()->OMSetBlendState( pElem->m_pBlendS, blendFactor, 0xffffffff );
			if ( pElem->m_pDepthStencilS )
				GetD3D11DeviceImmContext()->OMSetDepthStencilState( pElem->m_pDepthStencilS, pElem->m_uiStencilRef );

			XMMATRIX world;
			XMFLOAT3 oldLightDirections;
			if ( pElem->m_bStencilReflect )
			{
				XMVECTOR mirrorPlane = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f ); // xy plane
				XMMATRIX R = XMMatrixReflect( mirrorPlane );
				world = XMLoadFloat4x4( &pElem->m_vecSubElement[0].m_World ) * R;

				oldLightDirections = m_DirLights[0].Direction;

				XMVECTOR lightDir = XMLoadFloat3( &m_DirLights[0].Direction );
				XMVECTOR reflectedLightDir = XMVector3TransformNormal( lightDir, R );
				XMStoreFloat3( &m_DirLights[0].Direction, reflectedLightDir );

				//임시방편
				pElem->m_spShader->SetDirLight( m_DirLights[0] );
			}
			else if ( pElem->m_bShadowmap )
			{
				XMVECTOR shadowPlane = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ); // xz plane
				XMVECTOR toMainLight = -XMLoadFloat3( &m_DirLights[0].Direction );
				XMMATRIX S = XMMatrixShadow( shadowPlane, toMainLight );
				XMMATRIX shadowOffsetY = XMMatrixTranslation( 0.0f, 0.001f, 0.0f );

				world = XMLoadFloat4x4( &pElem->m_vecSubElement[0].m_World )*S*shadowOffsetY;
			}
			else
			{
				world = XMLoadFloat4x4( &pElem->m_vecSubElement[0].m_World );
			}

			TexturePtr spDiffuseMap = (*itor).m_spDiffuseMap;
			TexturePtr spCubeMap = (*itor).m_spCubeMap;
			TexturePtr spNormalMap = (*itor).m_spNormalMap;
			TexturePtr spShadowMap = (*itor).m_spShadowMap;

			world = XMLoadFloat4x4( &(*itor).m_World );

			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose( world );
			XMMATRIX viewProj = view*proj;
			XMMATRIX worldViewProj = world*view*proj;

			XMMATRIX texTransform = XMLoadFloat4x4( &(*itor).m_TexTransform );

			D3DX11_TECHNIQUE_DESC techDesc;
			pTech->GetDesc( &techDesc );

			for ( UINT p = 0; p < techDesc.Passes; ++p )
			{

				pElem->m_spShader->SetWorld( world );
				pElem->m_spShader->SetWorldViewProj( worldViewProj );
				pElem->m_spShader->SetViewProj( viewProj );
				pElem->m_spShader->SetWorldInvTranspose( worldInvTranspose );
				pElem->m_spShader->SetTexTransform( texTransform );
				pElem->m_spShader->SetShadowTransform( world*shadowTransform );
				pElem->m_spShader->SetMaterial( (*itor).m_mat );

				if ( spDiffuseMap )
				{
					if ( !spDiffuseMap->IsArray() )
						pElem->m_spShader->SetDiffuseMap( spDiffuseMap->GetSRV() );
					else
						pElem->m_spShader->SetDiffuseMapArray( spDiffuseMap->GetSRV() );
				}

				if ( spCubeMap )
				{
					pElem->m_spShader->SetCubeMap( spCubeMap->GetSRV() );
				}

				if ( spNormalMap )
				{
					pElem->m_spShader->SetNormalMap( spNormalMap->GetSRV() );
				}

				if ( spShadowMap )
				{
					pElem->m_spShader->SetShadowMap( spShadowMap->GetSRV() );
				}

				pTech->GetPassByIndex( p )->Apply( 0, GetD3D11DeviceImmContext() );

				if ( pElem->m_bDrawIndex )
				{

					GetD3D11DeviceImmContext()->DrawIndexed( (*itor).m_IndexCount, (*itor).m_StartIndexLocation, (*itor).m_BaseVertexLocation );
				}
				else
				{
					GetD3D11DeviceImmContext()->Draw( (*itor).m_VertexCount, (*itor).m_StartVertexLocation );
				}
			}


			if ( pElem->m_bStencilReflect )
			{
				m_DirLights[0].Direction = oldLightDirections;
			}
		}
	}

	DrawScreenQuad();
}

void ForwardRenderer::DrawScreenQuad()
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	XMMATRIX view = XMLoadFloat4x4( RendererCore::Instance()->GetView() );
	XMMATRIX proj = XMLoadFloat4x4( RendererCore::Instance()->GetProj() );

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



		for ( auto itor = pElem->m_vecSubElement.begin(); itor != pElem->m_vecSubElement.end(); ++itor )
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
				pElem->m_spShader->SetDiffuseMap( spDiffuseMap->GetSRV() );

				pTech->GetPassByIndex( p )->Apply( 0, GetD3D11DeviceImmContext() );
				GetD3D11DeviceImmContext()->DrawIndexed( 6, 0, 0 );
			}
		}
	}

}

void ForwardRenderer::DrawShadowMap()
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };


	if ( !m_spDepthStencilTex )
	{
		m_spDepthStencilTex = dynamic_pointer_cast<DepthStencilTexture, Texture>(RendererCore::Instance()->GetTexture( "depthstencil" ));

		m_spShadowMapShader = dynamic_pointer_cast<BuildShadowMapShader, FXShader>(RendererCore::Instance()->GetShader( "BuildShadowMap" ));
	}

	m_spDepthStencilTex->BindDsvAndSetNullRenderTarget();

	XMMATRIX view = XMLoadFloat4x4( &m_LightView );
	XMMATRIX proj = XMLoadFloat4x4( &m_LightProj );
	XMMATRIX viewProj = XMMatrixMultiply( view, proj );

	m_spShadowMapShader->SetEyePosW( RendererCore::Instance()->GetEyePosW() );
	m_spShadowMapShader->SetViewProj( viewProj );

	// These properties could be set per object if needed.
	m_spShadowMapShader->SetHeightScale( 0.07f );
	m_spShadowMapShader->SetMaxTessDistance( 1.0f );
	m_spShadowMapShader->SetMinTessDistance( 25.0f );
	m_spShadowMapShader->SetMinTessFactor( 1.0f );
	m_spShadowMapShader->SetMaxTessFactor( 5.0f );

	int iCount = m_vDrawShadowElements.size();
	for ( int i = 0; i < iCount; ++i )
	{
		DrawElement* pShadowElem = m_vDrawShadowElements[i];
		if ( !pShadowElem )
			continue;

		UINT stride = pShadowElem->stride;
		UINT offset = pShadowElem->offset;

		GetD3D11DeviceImmContext()->IASetInputLayout( pShadowElem->m_pInputLayout );
		GetD3D11DeviceImmContext()->IASetPrimitiveTopology( pShadowElem->ePrimitiveTopology );

		m_spShadowMapShader->SetDirLight( m_DirLights[0] );
		m_spShadowMapShader->SetDirLights( m_DirLights );
		m_spShadowMapShader->SetPointLight( m_PointLight );
		m_spShadowMapShader->SetSpotLight( m_SpotLight );
		m_spShadowMapShader->SetEyePosW( RendererCore::Instance()->GetEyePosW() );
		m_spShadowMapShader->SetFogStart( 15.0f );
		m_spShadowMapShader->SetFogRange( 300.0f );
		m_spShadowMapShader->SetFogColor( Colors::Silver );


		for ( auto itor = pShadowElem->m_vecSubElement.begin(); itor != pShadowElem->m_vecSubElement.end(); ++itor )
		{
			ID3DX11EffectTechnique* pTech = m_spShadowMapShader->GetTech( (*itor).m_iTechIndex );
			if ( !pTech )
				continue;

			D3DX11_TECHNIQUE_DESC techDesc;
			pTech->GetDesc( &techDesc );

			ID3D11Buffer* pVB = pShadowElem->m_spVB->GetVB();
			GetD3D11DeviceImmContext()->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );

			if ( pShadowElem->m_spIB )
				GetD3D11DeviceImmContext()->IASetIndexBuffer( pShadowElem->m_spIB->GetIB(), DXGI_FORMAT_R32_UINT, 0 );

			if ( pShadowElem->m_pRasterS )
				GetD3D11DeviceImmContext()->RSSetState( pShadowElem->m_pRasterS );
			if ( pShadowElem->m_pBlendS )
				GetD3D11DeviceImmContext()->OMSetBlendState( pShadowElem->m_pBlendS, blendFactor, 0xffffffff );
			if ( pShadowElem->m_pDepthStencilS )
				GetD3D11DeviceImmContext()->OMSetDepthStencilState( pShadowElem->m_pDepthStencilS, pShadowElem->m_uiStencilRef );

			TexturePtr spDiffuseMap = (*itor).m_spDiffuseMap;
			TexturePtr spCubeMap = (*itor).m_spCubeMap;
			TexturePtr spNormalMap = (*itor).m_spNormalMap;
			TexturePtr spShadowMap = (*itor).m_spShadowMap;

			XMMATRIX world = XMLoadFloat4x4( &(*itor).m_World );

			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose( world );
			XMMATRIX viewProj = view*proj;
			XMMATRIX worldViewProj = world*view*proj;

			XMMATRIX texTransform = XMLoadFloat4x4( &(*itor).m_TexTransform );

			for ( UINT p = 0; p < techDesc.Passes; ++p )
			{
				m_spShadowMapShader->SetWorld( world );
				m_spShadowMapShader->SetWorldViewProj( worldViewProj );
				m_spShadowMapShader->SetViewProj( viewProj );
				m_spShadowMapShader->SetWorldInvTranspose( worldInvTranspose );
				//				m_spShadowMapShader->SetTexTransform(texTransform);
				//				m_spShadowMapShader->SetShadowTransform(world*shadowTransform);
				m_spShadowMapShader->SetMaterial( (*itor).m_mat );

				if ( spDiffuseMap )
				{
					if ( !spDiffuseMap->IsArray() )
						m_spShadowMapShader->SetDiffuseMap( spDiffuseMap->GetSRV() );
					else
						m_spShadowMapShader->SetDiffuseMapArray( spDiffuseMap->GetSRV() );
				}

				pTech->GetPassByIndex( p )->Apply( 0, GetD3D11DeviceImmContext() );

				if ( pShadowElem->m_bDrawIndex )
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

}


void ForwardRenderer::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3( &m_DirLights[0].Direction );
	XMVECTOR lightPos = -2.0f*m_SceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3( &m_SceneBounds.Center );
	XMVECTOR up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	XMMATRIX V = XMMatrixLookAtLH( lightPos, targetPos, up );

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3( &sphereCenterLS, XMVector3TransformCoord( targetPos, V ) );

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - m_SceneBounds.Radius;
	float b = sphereCenterLS.y - m_SceneBounds.Radius;
	float n = sphereCenterLS.z - m_SceneBounds.Radius;
	float r = sphereCenterLS.x + m_SceneBounds.Radius;
	float t = sphereCenterLS.y + m_SceneBounds.Radius;
	float f = sphereCenterLS.z + m_SceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH( l, r, b, t, n, f );

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f );

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4( &m_LightView, V );
	XMStoreFloat4x4( &m_LightProj, P );
	XMStoreFloat4x4( &m_ShadowTransform, S );
}
