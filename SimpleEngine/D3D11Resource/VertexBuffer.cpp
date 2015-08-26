#include "stdafx.h"
#include "VertexBuffer.h"
#include "RendererCore.h"


VertexBuffer::VertexBuffer()
: m_pVB(NULL)
{
}

VertexBuffer::~VertexBuffer()
{
	ReleaseCOM(m_pVB);
}

shared_ptr<VertexBuffer> VertexBuffer::Create(UINT uiSize, const void* pData, bool bDynamic)
{
	VertexBufferPtr spVB(new VertexBuffer());
	spVB->CreateVB(uiSize, pData, bDynamic);

	return spVB;
}

bool VertexBuffer::CreateVB(UINT uiSize, const void* pData, bool bDynamic)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = uiSize;
	vbd.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	vbd.MiscFlags = 0;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.StructureByteStride = 0;

	if (bDynamic)
	{
		HR(GetD3D11Device()->CreateBuffer(&vbd, NULL, &m_pVB));
	}
	else
	{
		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pData;
		HR(GetD3D11Device()->CreateBuffer(&vbd, &initData, &m_pVB));
	}


	return true;
}
