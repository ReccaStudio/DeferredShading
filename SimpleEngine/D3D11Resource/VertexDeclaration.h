#pragma once

class VertexDeclaration
{
public:
	VertexDeclaration();
	virtual ~VertexDeclaration();

	enum EVDecl
	{
		EVDecl_POSITION,
		EVDecl_COLOR,
		EVDecl_NORMAL,
		EVDecl_TEXCOORD,
		EVDecl_TANGENT,
		EVDecl_SIZE,
		EVDecl_Max = 16,
	};

	static shared_ptr<VertexDeclaration> Create(bitset<EVDecl_Max>& bitVDecls, ID3DX11EffectTechnique* pTech);

	bool CreateVD(bitset<EVDecl_Max>& bitVDecls, ID3DX11EffectTechnique* pTech);

	UINT				GetHas() const { return m_uiHash; }
	const string&		GetSematics(EVDecl eVDecl, DXGI_FORMAT& dfFormat, UINT& uiFormatSize);
	ID3D11InputLayout*	GetInLayout() const { return m_pInputLayout; }

protected:
	UINT		m_uiHash;
	string		m_strSemantic[16];

	ID3D11InputLayout* m_pInputLayout;

	bitset<EVDecl_Max>					m_bitDecls;
	vector<D3D11_INPUT_ELEMENT_DESC>	m_vecInputElements;
};

typedef shared_ptr<VertexDeclaration> VertexDeclarationPtr;
typedef shared_ptr<bitset<VertexDeclaration::EVDecl_Max>> BitVDeclPtr;
