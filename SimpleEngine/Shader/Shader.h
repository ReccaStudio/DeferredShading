#pragma once

#include "D3D11Resource.h"


class Shader
{
public:
	Shader();
	virtual ~Shader();


protected:
	D3D11VertexShaderPtr	m_spVS;
	D3D11PixelShaderPtr		m_spPS;
	D3D11HullShaderPtr		m_spHullS;
	D3D11DomainShaderPtr	m_spDomainS;
	ID3D11GeometryShaderPtr m_spGeomS;
};

