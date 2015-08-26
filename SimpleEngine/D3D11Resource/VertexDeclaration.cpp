#include "stdafx.h"
#include "VertexDeclaration.h"
#include "RendererCore.h"


VertexDeclaration::VertexDeclaration()
{
	
}


VertexDeclaration::~VertexDeclaration()
{
	ReleaseCOM(m_pInputLayout);
}

shared_ptr<VertexDeclaration> VertexDeclaration::Create(bitset<EVDecl_Max>& bitVDecls, ID3DX11EffectTechnique* pTech)
{
	VertexDeclarationPtr spVD(new VertexDeclaration);
	spVD->CreateVD(bitVDecls, pTech);

	return spVD;
}

bool VertexDeclaration::CreateVD(bitset<EVDecl_Max>& bitVDecls, ID3DX11EffectTechnique* pTech)
{
	UINT uInputSignatureSize = 0;
	UINT uiIndex = 0;
	UINT uiOffsetSize = 0;
	UINT uiFormatSize = 0;

	m_uiHash = bitVDecls.to_ulong();
	
	int iCount = bitVDecls.count();
	for (int i = 0; i < EVDecl_Max; ++i)
	{
		bool bTurnon = bitVDecls.test(i);
		if (!bTurnon)
			continue;

		D3D11_INPUT_ELEMENT_DESC vertexDesc;

		uiOffsetSize += uiFormatSize;

		vertexDesc.SemanticName = GetSematics(EVDecl(i), vertexDesc.Format, uiFormatSize).c_str();
		vertexDesc.SemanticIndex = 0;
		vertexDesc.InputSlot = 0;
		vertexDesc.AlignedByteOffset = uiOffsetSize;
		vertexDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexDesc.InstanceDataStepRate = 0;
		uiIndex++;

		m_vecInputElements.push_back(vertexDesc);
	}

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	HR( pTech->GetPassByIndex(0)->GetDesc(&passDesc));
	HR(GetD3D11Device()->CreateInputLayout(&m_vecInputElements[0], uiIndex, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_pInputLayout));

	return true;
}

const string& VertexDeclaration::GetSematics(EVDecl eVDecl, DXGI_FORMAT& dfFormat, UINT& uiFormatSize)
{
	m_strSemantic[eVDecl] = "NONE";

	switch (eVDecl)
	{
	case EVDecl_POSITION:
		m_strSemantic[eVDecl] = "POSITION";
		dfFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		uiFormatSize = 12;
		break;
	case EVDecl_COLOR:
		m_strSemantic[eVDecl] = "COLOR";
		dfFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		uiFormatSize = 12;
		break;
	case EVDecl_NORMAL:
		m_strSemantic[eVDecl] = "NORMAL";
		dfFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		uiFormatSize = 12;
		break;
	case EVDecl_TEXCOORD:
		m_strSemantic[eVDecl] = "TEXCOORD";
		dfFormat = DXGI_FORMAT_R32G32_FLOAT;
		uiFormatSize = 8;
		break;
	case EVDecl_TANGENT:
		m_strSemantic[eVDecl] = "TANGENT";
		dfFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		uiFormatSize = 12;
		break;
	case EVDecl_SIZE:
		m_strSemantic[eVDecl] = "SIZE";
		dfFormat = DXGI_FORMAT_R32G32_FLOAT;
		uiFormatSize = 8;
		break;
	}

	return m_strSemantic[eVDecl];
}
