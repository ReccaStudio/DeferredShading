#pragma once

// #include "TRefCountPtr.h"
// 
// class RHIResource : public RefCountObj
// {
// public:
// 	virtual ~RHIResource();
// };


class D3D11Resource
{
public:
	virtual ~D3D11Resource();
};


class D3D11VertexShader : public D3D11Resource
{
public:
	D3D11VertexShader() {}
	virtual ~D3D11VertexShader() {}

protected:
	ID3D11VertexShader* m_pVS;
};

class D3D11PixelShader : public D3D11Resource
{
public:
	D3D11PixelShader() {}
	virtual ~D3D11PixelShader() {}

protected:
	ID3D11PixelShader* m_pPS;
};

class D3D11HullShader : public D3D11Resource
{
public:
	D3D11HullShader() {}
	virtual ~D3D11HullShader() {}

protected:
	ID3D11HullShader* m_pHS;
};

class D3D11DomainShader : public D3D11Resource
{
public:
	D3D11DomainShader() {}
	virtual ~D3D11DomainShader() {}

protected:
	ID3D11DomainShader* m_pDMS;
};

class D3D11GeometryShader : public D3D11Resource
{
public:
	D3D11GeometryShader() {}
	virtual ~D3D11GeometryShader() {}

protected:
	ID3D11GeometryShader* m_pGMS;
};

typedef shared_ptr<D3D11VertexShader> D3D11VertexShaderPtr;
typedef shared_ptr<D3D11PixelShader> D3D11PixelShaderPtr;
typedef shared_ptr<D3D11HullShader> D3D11HullShaderPtr;
typedef shared_ptr<D3D11DomainShader> D3D11DomainShaderPtr;
typedef shared_ptr<ID3D11GeometryShader> ID3D11GeometryShaderPtr;
