#pragma once

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	static shared_ptr<VertexBuffer> Create(UINT uiSize, const void* pData, bool bDynamic);

	bool CreateVB(UINT uiSize, const void* pData, bool bDynamic);

	ID3D11Buffer* GetVB() const { return m_pVB; }

protected:
	ID3D11Buffer* m_pVB;

};

typedef shared_ptr<VertexBuffer> VertexBufferPtr;