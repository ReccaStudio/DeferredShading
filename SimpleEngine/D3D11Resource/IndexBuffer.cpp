#include "stdafx.h"
#include "IndexBuffer.h"
#include "RendererCore.h"

IndexBuffer::IndexBuffer()
: m_pIB(0)
{
}


IndexBuffer::~IndexBuffer()
{
	ReleaseCOM(m_pIB);
}

shared_ptr<IndexBuffer> IndexBuffer::Create(UINT uiSize, const void* pData, bool bDynamic)
{
	IndexBufferPtr spIB(new IndexBuffer());
	spIB->CreateIB(uiSize, pData, bDynamic);

	return spIB;
}

bool IndexBuffer::CreateIB(UINT uiSize, const void* pData, bool bDynamic)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = uiSize;
	ibd.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	ibd.MiscFlags = 0;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pData;
	HR(GetD3D11Device()->CreateBuffer(&ibd, &initData, &m_pIB));

	return true;
}
