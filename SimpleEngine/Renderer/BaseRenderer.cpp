#include "stdafx.h"
#include "BaseRenderer.h"


BaseRenderer::BaseRenderer()
{
}


BaseRenderer::~BaseRenderer()
{
	int iCount = m_vDrawShadowElements.size();
	for ( int i = 0; i < iCount; ++i )
	{
		DrawElement* pInput = m_vDrawShadowElements[i];
		SafeDelete( pInput );
	}
	m_vDrawShadowElements.clear();

	iCount = m_vDrawElements.size();
	for ( int i = 0; i < iCount; ++i )
	{
		DrawElement* pInput = m_vDrawElements[i];
		SafeDelete( pInput );
	}
	m_vDrawElements.clear();

	iCount = m_vDrawScreenElements.size();
	for ( int i = 0; i < iCount; ++i )
	{
		DrawElement* pInput = m_vDrawScreenElements[i];
		SafeDelete( pInput );
	}
	m_vDrawScreenElements.clear();

	iCount = m_vDrawStencilElements.size();
	for ( int i = 0; i < iCount; ++i )
	{
		DrawElement* pInput = m_vDrawStencilElements[i];
		SafeDelete( pInput );
	}
	m_vDrawStencilElements.clear();

	iCount = m_vDrawLightingElements.size();
	for ( int i = 0; i < iCount; ++i )
	{
		DrawElement* pInput = m_vDrawLightingElements[i];
		SafeDelete( pInput );
	}
	m_vDrawLightingElements.clear();
}

void BaseRenderer::Init()
{

}

void BaseRenderer::Resize()
{

}

void BaseRenderer::Update( float dt )
{

}

void BaseRenderer::DrawScene()
{

}
