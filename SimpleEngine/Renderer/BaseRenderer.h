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

struct BoundingSphere
{
	BoundingSphere() : Center( 0.0f, 0.0f, 0.0f ), Radius( 0.0f ) {}
	XMFLOAT3 Center;
	float Radius;
};


class BaseRenderer
{
public:
	BaseRenderer();
	virtual ~BaseRenderer();

	virtual void Init();
	virtual void Resize();
	virtual void Update( float dt );
	virtual void DrawScene();

	void AddDrawElem( DrawElement* pElem ) {
		m_vDrawElements.push_back( pElem );
	}
	void AddDrawScreenElem( DrawElement* pElem ) {
		m_vDrawScreenElements.push_back( pElem );
	}
	void AddDrawShadowElem( DrawElement* pElem ) {
		m_vDrawShadowElements.push_back( pElem );
	}
	void AddStencilElem( DrawElement* pElem ) {
		m_vDrawStencilElements.push_back( pElem );
	}

protected:

protected:
	vector<DrawElement*>	m_vDrawElements;
	vector<DrawElement*>	m_vDrawShadowElements;
	vector<DrawElement*>	m_vDrawScreenElements;
	vector<DrawElement*>	m_vDrawStencilElements;
	vector<DrawElement*>	m_vDrawLightingElements;

};

