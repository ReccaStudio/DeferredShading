#pragma once

class Texture
{
public:
	Texture();
	virtual ~Texture();

	static shared_ptr<Texture> Create(const wstring& filename);
	static shared_ptr<Texture> Create(ID3D11Texture2D* pTexture2D);
	static shared_ptr<Texture> Create(const vector<wstring>& filelist, 
		DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE, UINT filter = D3DX11_FILTER_NONE, UINT mipFilter = D3DX11_FILTER_LINEAR);

	ID3D11ShaderResourceView* GetSRV() const { return m_pTexShaderRV; }

	bool CreateFile(const wstring& filename);
	bool CreateTexture2D(ID3D11Texture2D* pTexture2D);
	bool CreateTexture2DArraySRV(const vector<wstring>& filenames,
		DXGI_FORMAT format, UINT filter, UINT mipFilter);

	bool IsArray() const { return m_bArray; }
	void IsArray(bool val) { m_bArray = val; }

protected:
	bool m_bArray;

	UINT		m_uiWidth;
	UINT		m_uiHeight;

	ID3D11ShaderResourceView* m_pTexShaderRV;
};


class DepthStencilTexture : public Texture
{
public:
	DepthStencilTexture();
	virtual ~DepthStencilTexture();

	static shared_ptr<Texture> Create(UINT uiWidth, UINT uiHeight);

	bool CreateDepthStencil(UINT uiWidth, UINT uiHeight);

	void BindDsvAndSetNullRenderTarget();

	ID3D11DepthStencilView* GetDSView() const { return m_pDepthStencilView;  }

protected:

	D3D11_VIEWPORT			m_Viewport;
	ID3D11DepthStencilView* m_pDepthStencilView;
};



typedef shared_ptr<Texture> TexturePtr;
typedef shared_ptr<DepthStencilTexture> DepthStencilTexturePtr;
