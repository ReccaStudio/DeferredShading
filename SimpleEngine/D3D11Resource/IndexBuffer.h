#pragma once

class IndexBuffer
{
public:
	IndexBuffer();
	virtual ~IndexBuffer();

	static shared_ptr<IndexBuffer> Create(UINT uiSize, const void* pData, bool bDynamic);

	bool CreateIB(UINT uiSize, const void* pData, bool bDynamic);

	ID3D11Buffer* GetIB() const { return m_pIB; }

protected:
	ID3D11Buffer* m_pIB;
};

typedef shared_ptr<IndexBuffer> IndexBufferPtr;