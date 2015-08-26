#pragma once

class RenderTarget
{
public:
	RenderTarget();
	virtual ~RenderTarget();

	static shared_ptr<RenderTarget> Create(UINT uiWidth, UINT uiHeight);

	bool CreateRT(UINT uiWidth, UINT uiHeight);
	void Resize( UINT uiWidth, UINT uiHeight );

	ID3D11Texture2D* GetTex2D() const { return m_pTex2D; }
	ID3D11RenderTargetView* GetRTV() const { return m_pRTV; }
	ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }
protected:
	UINT		m_uiWidth;
	UINT		m_uiHeight;

	ID3D11Texture2D*			m_pTex2D;
	ID3D11RenderTargetView*		m_pRTV;
	ID3D11ShaderResourceView*	m_pSRV;
};


typedef shared_ptr<RenderTarget> RenderTargetPtr;

