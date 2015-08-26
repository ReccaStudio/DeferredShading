
template<typename VertexType>
DrawElement* GenerateMeshElement(UINT uiSize, const string& strObjName, StaticMesh<VertexType>& rMesh, const string& strShaderName,
	const string& strTechname, const string& strRSName, const string& strVDeclName, bool bDrawIndex)
{
	DrawElement* pElem = new DrawElement();

	VertexType& meshData = rMesh.GetMesh();

	pElem->m_bDrawIndex = bDrawIndex;

	VertexBufferPtr spVB = RendererCore::Instance()->GetVB(strObjName);
	if (!spVB)
	{
		spVB = VertexBuffer::Create(uiSize, rMesh.GetData(), false);
		RendererCore::Instance()->InsertVB(strObjName, spVB);
	}
	pElem->m_spVB = spVB;

	if (bDrawIndex)
	{
		IndexBufferPtr spIB = RendererCore::Instance()->GetIB(strObjName);
		if (!spIB)
		{
			spIB = IndexBuffer::Create(sizeof(UINT)* meshData.Indices.size(), &meshData.Indices[0], false);
			RendererCore::Instance()->InsertIB(strObjName, spIB);
		}
		pElem->m_spIB = spIB;
	}

	RenderStatesPtr spRS = RendererCore::Instance()->GetRenderState(strRSName);
	if (spRS)
	{
		pElem->m_pBlendS = spRS->GetBS();
		pElem->m_pRasterS = spRS->GetRS();
		pElem->m_pDepthStencilS = spRS->GetDSS();
	}

	pElem->m_spShader = RendererCore::Instance()->GetShader(strShaderName);
	pElem->m_spShader->SetTech(strTechname);

	pElem->ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	BitVDeclPtr spBitVDecl = RendererCore::Instance()->GetBitVDecl(strVDeclName);
	bitset<16>& rbitVDecl = *spBitVDecl.get();
	VertexDeclarationPtr spVDecl = VertexDeclaration::Create(rbitVDecl, pElem->m_spShader->GetTech());
	RendererCore::Instance()->InsertVDecl(strTechname+strVDeclName, spVDecl);

	pElem->m_pInputLayout = spVDecl->GetInLayout();

	DrawElement::SubElement subElem;
	subElem.m_IndexCount = rMesh.GetMesh().Indices.size();
	pElem->m_vecSubElement.push_back(subElem);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_World, I);

	return pElem;
}

template<typename VertexType>
DrawElement* GenerateDFMeshElement( UINT uiSize, const string& strObjName, StaticMesh<VertexType>& rMesh, 
	const string& strRSName, const string& strVDeclName, bool bDrawIndex )
{
	DrawElement* pElem = new DrawElement();

	VertexType& meshData = rMesh.GetMesh();

	pElem->m_bDrawIndex = bDrawIndex;

	VertexBufferPtr spVB = RendererCore::Instance()->GetVB( strObjName );
	if ( !spVB )
	{
		spVB = VertexBuffer::Create( uiSize, rMesh.GetData(), false );
		RendererCore::Instance()->InsertVB( strObjName, spVB );
	}
	pElem->m_spVB = spVB;

	if ( bDrawIndex )
	{
		IndexBufferPtr spIB = RendererCore::Instance()->GetIB( strObjName );
		if ( !spIB )
		{
			spIB = IndexBuffer::Create( sizeof( UINT )* meshData.Indices.size(), &meshData.Indices[0], false );
			RendererCore::Instance()->InsertIB( strObjName, spIB );
		}
		pElem->m_spIB = spIB;
	}

	RenderStatesPtr spRS = RendererCore::Instance()->GetRenderState( strRSName );
	if ( spRS )
	{
		pElem->m_pBlendS = spRS->GetBS();
		pElem->m_pRasterS = spRS->GetRS();
		pElem->m_pDepthStencilS = spRS->GetDSS();
	}

	pElem->m_spRenderState = spRS;
	pElem->ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

// 	BitVDeclPtr spBitVDecl = RendererCore::Instance()->GetBitVDecl( strVDeclName );
// 	bitset<16>& rbitVDecl = *spBitVDecl.get();
// 	VertexDeclarationPtr spVDecl = VertexDeclaration::Create( rbitVDecl, pElem->m_spShader->GetTech() );
// 	RendererCore::Instance()->InsertVDecl( strObjName + strVDeclName, spVDecl );

//	pElem->m_pInputLayout = spVDecl->GetInLayout();

	DrawElement::SubElement subElem;
	subElem.m_IndexCount = rMesh.GetMesh().Indices.size();
	pElem->m_vecSubElement.push_back( subElem );

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4( &pElem->m_vecSubElement[0].m_World, I );

	return pElem;
}

template<typename VertexType>
DrawElement* GenerateDMeshElement(UINT uiSize, const string& strObjName, DynamicMesh<VertexType>& rMesh, const string& strShaderName,
	const string& strTechname, const string& strRSName, const string& strVDeclName)
{
	DrawElement* pElem = new DrawElement();

	VertexType& meshData = rMesh.GetMesh();

	VertexBufferPtr spVB = VertexBuffer::Create(uiSize, NULL, true);
	IndexBufferPtr spIB = IndexBuffer::Create(sizeof(UINT)* meshData.Indices.size(), &meshData.Indices[0], false);

	RendererCore::Instance()->InsertVB(strObjName, spVB);
	RendererCore::Instance()->InsertIB(strObjName, spIB);

	pElem->m_spVB = spVB;
	pElem->m_spIB = spIB;

	RenderStatesPtr spRS = RendererCore::Instance()->GetRenderState(strRSName);
	if (spRS)
	{
		pElem->m_pBlendS = spRS->GetBS();
		pElem->m_pRasterS = spRS->GetRS();
	}

	pElem->m_spShader = RendererCore::Instance()->GetShader(strShaderName);
	pElem->m_spShader->SetTech(strTechname);

	pElem->ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	BitVDeclPtr spBitVDecl = RendererCore::Instance()->GetBitVDecl(strVDeclName);
 	bitset<16>& rbitVDecl = *spBitVDecl.get();
 	VertexDeclarationPtr spVDecl = VertexDeclaration::Create(rbitVDecl, pElem->m_spShader->GetTech());
	RendererCore::Instance()->InsertVDecl(strTechname + strVDeclName, spVDecl);

	pElem->m_pInputLayout = spVDecl->GetInLayout();

	DrawElement::SubElement subElem;
	subElem.m_IndexCount = rMesh.GetMesh().Indices.size();
	pElem->m_vecSubElement.push_back(subElem);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_World, I);

	return pElem;
}
