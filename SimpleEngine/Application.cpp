#include "stdafx.h"
#include "Application.h"
#include "GeometryGenerator.h"
#include "VertexDefine.h"
#include "VertexBuffer.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Application theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}


Application::Application(HINSTANCE hInstance)
: 
D3DApp(hInstance), 
mTheta(1.3f*MathHelper::Pi),
mPhi(0.4f*MathHelper::Pi),
mRadius(200.0f),
// mTheta(1.24f*MathHelper::Pi),
// mPhi(0.42f*MathHelper::Pi),
// mRadius(12.0f),

m_pDrawWave(0),
m_WaterTexOffset(0.0f, 0.0f)
{
	m_wsMainWndCaption = L"Rendering Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	m_Camera.SetPosition(0.0f, 20.0f, -20.0f);
	m_Camera.Pitch( XMConvertToRadians(45.0f) );

	RendererCore::Create();

}

Application::~Application()
{
	RendererCore::Destroy();
}

bool Application::Init()
{
	if (!D3DApp::Init())
		return false;

	RendererCore::Instance()->InsertTexture("grass",	Texture::Create(L"Textures/grass.dds"));
	RendererCore::Instance()->InsertTexture("Wall",		Texture::Create(L"Textures/brick01.dds"));
	RendererCore::Instance()->InsertTexture("Mirror",	Texture::Create(L"Textures/ice.dds"));
	RendererCore::Instance()->InsertTexture("floor2", Texture::Create(L"Textures/floor.dds"));
	RendererCore::Instance()->InsertTexture( "stones", Texture::Create( L"Textures/stones.dds" ) );
	RendererCore::Instance()->InsertTexture( "bricks", Texture::Create( L"Textures/bricks.dds" ) );
	RendererCore::Instance()->InsertTexture("desertcube1024", Texture::Create(L"Textures/desertcube1024.dds"));

	RendererCore::Instance()->InsertTexture("bricks_nmap", Texture::Create(L"Textures/bricks_nmap.dds"));
	RendererCore::Instance()->InsertTexture("floor_nmap", Texture::Create(L"Textures/floor_nmap.dds"));
	RendererCore::Instance()->InsertTexture("stones_nmap", Texture::Create(L"Textures/stones_nmap.dds"));

	//depthstecil
	RendererCore::Instance()->InsertTexture("depthstencil", DepthStencilTexture::Create(2048, 2048));

// 	std::vector<std::wstring> treeFilenames;
// 	treeFilenames.push_back(L"Textures/tree0.dds");
// 	treeFilenames.push_back(L"Textures/tree1.dds");
// 	treeFilenames.push_back(L"Textures/tree2.dds");
// 	treeFilenames.push_back(L"Textures/tree3.dds");
// 
// 	TexturePtr spTreeArray = Texture::Create(treeFilenames, DXGI_FORMAT_R8G8B8A8_UNORM);
// 	Renderer::Instance()->InsertTexture("TreeArray", spTreeArray);
	//Renderer::Instance()->InsertTexture("DepthStencil", Texture::Create(Renderer::Instance()->GetDepthStencilBuffer()));


	RendererCore::Instance()->InsertRenderState( "wireframe", RenderStates::Create( RenderStates::ERS_Wireframe, RenderStates::EBS_None, RenderStates::EDSS_Default ) );
	RendererCore::Instance()->InsertRenderState( "nocull", RenderStates::Create( RenderStates::ERS_NoCull, RenderStates::EBS_None, RenderStates::EDSS_Default ) );
	RendererCore::Instance()->InsertRenderState( "cullclockwise", RenderStates::Create(RenderStates::ERS_CullClockwise, RenderStates::EBS_None));
	RendererCore::Instance()->InsertRenderState( "cull",			 RenderStates::Create(RenderStates::ERS_Cull, RenderStates::EBS_None, RenderStates::EDSS_Default));

	RendererCore::Instance()->InsertRenderState("alphatocoverage",		RenderStates::Create(RenderStates::ERS_None, RenderStates::EBS_AlphatoCoverage));
	RendererCore::Instance()->InsertRenderState("transparent",			RenderStates::Create(RenderStates::ERS_None, RenderStates::EBS_Transparent));
	RendererCore::Instance()->InsertRenderState("norrendertargetwrite", RenderStates::Create(RenderStates::ERS_None, RenderStates::EBS_NoRenderTargetWrite));
	RendererCore::Instance()->InsertRenderState( "stencilvolumemask", RenderStates::Create( RenderStates::ERS_NoCull, RenderStates::EBS_None, RenderStates::EDSS_StencilVolumeMask ) );
	RendererCore::Instance()->InsertRenderState( "stencilclippedpixels", RenderStates::Create( RenderStates::ERS_CullClockwise, RenderStates::EBS_ADD_ONE, RenderStates::EDSS_StencilClippedPixels ) );

	RendererCore::Instance()->InsertRenderState( "dss_default", RenderStates::Create( RenderStates::ERS_None, RenderStates::EBS_None, RenderStates::EDSS_Default ) );
	RendererCore::Instance()->InsertRenderState("markmirror",		RenderStates::Create(RenderStates::ERS_None, RenderStates::EBS_None, RenderStates::EDSS_MarkMirror));
	RendererCore::Instance()->InsertRenderState("drawrelection",	RenderStates::Create(RenderStates::ERS_None, RenderStates::EBS_None, RenderStates::EDSS_DrawRelection));
	RendererCore::Instance()->InsertRenderState("nodoubleblend",	RenderStates::Create(RenderStates::ERS_None, RenderStates::EBS_None, RenderStates::EDSS_NoDoubleBlend));

	RendererCore::Instance()->InsertRenderState( "ambientlight", RenderStates::Create( RenderStates::ERS_Cull, RenderStates::EBS_ADD_ONE, RenderStates::EDSS_None ) );
	RendererCore::Instance()->InsertRenderState( "geometrypass", RenderStates::Create( RenderStates::ERS_Cull, RenderStates::EBS_None, RenderStates::EDSS_Default ) );
	RendererCore::Instance()->InsertRenderState( "mrtpass", RenderStates::Create( RenderStates::ERS_Cull, RenderStates::EBS_None, RenderStates::EDSS_DepthWRDisable ) );

	if ( RendererCore::Instance()->GetRDRType() == ERDR_Forward )
	{ 
		RendererCore::Instance()->InsertShader( "Sky", FXShader::Create( L"HLSLs/Sky.cso" ) );
		RendererCore::Instance()->InsertShader( "Basic", FXShader::Create( L"HLSLs/Basic.cso" ) );
		RendererCore::Instance()->InsertShader( "NormalMap", FXShader::Create( L"HLSLs/NormalMap.cso" ) );
		RendererCore::Instance()->InsertShader( "BuildShadowMap", BuildShadowMapShader::Create( L"HLSLs/BuildShadowMap.cso" ) );
		RendererCore::Instance()->InsertShader( "DebugTexture", BuildShadowMapShader::Create( L"HLSLs/DebugTexture.cso" ) );
		RendererCore::Instance()->InsertShader( "ScreenQuad", BuildShadowMapShader::Create( L"HLSLs/ScreenQuad.cso" ) );
	}
	else if ( RendererCore::Instance()->GetRDRType() == ERDR_Deferred )
	{
		RendererCore::Instance()->InsertShader( "DebugTexture", BuildShadowMapShader::Create( L"HLSLs/DebugTexture.cso" ) );
	}

	BitVDeclPtr spBitVNT(new bitset<VertexDeclaration::EVDecl_Max>);
	spBitVNT->set(VertexDeclaration::EVDecl_POSITION);
	spBitVNT->set(VertexDeclaration::EVDecl_NORMAL);
	spBitVNT->set(VertexDeclaration::EVDecl_TEXCOORD);
	RendererCore::Instance()->InsertBitVDecl("PNUV", spBitVNT);

	BitVDeclPtr spBitVT(new bitset<VertexDeclaration::EVDecl_Max>);
	spBitVT->set(VertexDeclaration::EVDecl_POSITION);
	spBitVT->set(VertexDeclaration::EVDecl_TEXCOORD);
	RendererCore::Instance()->InsertBitVDecl("PUV", spBitVT);

	BitVDeclPtr spBitVS(new bitset<VertexDeclaration::EVDecl_Max>);
	spBitVS->set(VertexDeclaration::EVDecl_POSITION);
	spBitVS->set(VertexDeclaration::EVDecl_SIZE);
	RendererCore::Instance()->InsertBitVDecl("PS", spBitVS);

	BitVDeclPtr spBitV(new bitset<VertexDeclaration::EVDecl_Max>);
	spBitV->set(VertexDeclaration::EVDecl_POSITION);
	RendererCore::Instance()->InsertBitVDecl("P", spBitV);

	BitVDeclPtr spBitPNTUV(new bitset<VertexDeclaration::EVDecl_Max>);
	spBitPNTUV->set(VertexDeclaration::EVDecl_POSITION);
	spBitPNTUV->set(VertexDeclaration::EVDecl_NORMAL);
	spBitPNTUV->set(VertexDeclaration::EVDecl_TEXCOORD);
	spBitPNTUV->set(VertexDeclaration::EVDecl_TANGENT);
	RendererCore::Instance()->InsertBitVDecl("PNTUV", spBitPNTUV);

	ShadowmapScene();

// 	shared_ptr<VertexBuffer> spVB ( new VertexBuffer() );
// 	shared_ptr<VertexBuffer> spVB2 = spVB;
// 	shared_ptr<VertexBuffer> spVB3 = spVB2;
// 	weak_ptr<VertexBuffer> spwVB1 = spVB;
// 	spVB3 = 0;
// 	spVB2.reset();
// 	spwVB1.lock();

	RendererCore::Instance()->InitRenderer();
	
	return true;
}

void Application::OnResize()
{
	D3DApp::OnResize();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	//XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	//Renderer::Instance()->SetProj(P);

	m_Camera.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 10000.0f);
}

void Application::UpdateScene(float dt)
{
	RendererCore::Instance()->UpdateScene(dt);


	if (GetAsyncKeyState('W') & 0x8000)
		m_Camera.Walk(10.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		m_Camera.Walk(-10.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		m_Camera.Strafe(-10.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		m_Camera.Strafe(10.0f*dt);

	m_Camera.UpdateViewMatrix();

	RendererCore::Instance()->SetEyePosW(m_Camera.GetPosition());
	RendererCore::Instance()->SetView(m_Camera.View());
	RendererCore::Instance()->SetProj(m_Camera.Proj());

	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

// 	Renderer::Instance()->SetEyePos(XMFLOAT3(x, y, z));

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

// 	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
// 	Renderer::Instance()->S`etView(V);
 	//
 	// Every quarter second, generate a random wave.
 	//
//  	static float t_base = 0.0f;
//  	if ((m_Timer.TotalTime() - t_base) >= 0.25f)
//  	{
// 		t_base += 0.25f;
// 
// 		DWORD i = 5 + rand() % (m_Waves.RowCount() - 10);
// 		DWORD j = 5 + rand() % (m_Waves.ColumnCount() - 10);
// 
// 		float r = MathHelper::RandF(1.0f, 2.0f);
// 
//  		m_Waves.Disturb(i, j, r);
//  	}
//  
//  	m_Waves.Update(dt);
// 
// 	VertexBufferPtr spWaveVB = Renderer::Instance()->GetVB("Wave");
// 	if (spWaveVB)
// 	{
// 		void* pData = Renderer::Instance()->MapbyVB(spWaveVB);
// 		if (pData)
// 		{
// 			VertexPNUV* v = reinterpret_cast<VertexPNUV*>(pData);
// 			for (UINT i = 0; i < m_Waves.VertexCount(); ++i)
// 			{
// 				v[i].Pos = m_Waves[i];
// 				v[i].Normal = m_Waves.Normal(i);
// 
// 				// Derive tex-coords in [0,1] from position.
// 				v[i].UV.x = 0.5f + m_Waves[i].x / m_Waves.Width();
// 				v[i].UV.y = 0.5f - m_Waves[i].z / m_Waves.Depth();
// 			}
// 			Renderer::Instance()->UnmapbyVB(spWaveVB);
// 		}
// 
// 		XMMATRIX wavesScale = XMMatrixScaling(3.0f, 3.0f, 0.0f);
// 
// 		m_WaterTexOffset.y += 0.05f*dt;
// 		m_WaterTexOffset.x += 0.1f*dt;
// 		XMMATRIX wavesOffset = XMMatrixTranslation(m_WaterTexOffset.x, m_WaterTexOffset.y, 0.0f);
// 		XMStoreFloat4x4(&m_pDrawWave->m_TexTransform, wavesScale*wavesOffset);
// 	}



// 	XMFLOAT3 PointLitPos;
// 	// Circle light over the land surface.
// 	PointLitPos.x = 70.0f*cosf(0.2f*m_Timer.TotalTime());
// 	PointLitPos.z = 70.0f*sinf(0.2f*m_Timer.TotalTime());
// 	PointLitPos.y = MathHelper::Max(GetHeight(PointLitPos.x,
// 		PointLitPos.z), -3.0f) + 10.0f;
// 
// 	RendererCore::Instance()->SetPointLightPos(PointLitPos);
// 	RendererCore::Instance()->SetSpotLightPos(XMFLOAT3(x, y, z), XMVector3Normalize(target - pos));
}

void Application::DrawScene()
{
	RendererCore::Instance()->DrawScene();
}

void Application::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void Application::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Application::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.2f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 300.0f);
	}

	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		m_Camera.Pitch(dy);
		m_Camera.RotateY(dx);
	}


	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

float Application::GetHeight(float x, float z)const
{
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}

XMFLOAT3 Application::GetHillNormal(float x, float z)const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void Application::ShadowmapScene()
{
	if ( RendererCore::Instance()->GetRDRType() == ERDR_Forward )
	{
		BuildSkyBuffers();
		BuildSkullGeometryBuffers2();
		BuildShapeGeometryBuffers();
		BuildScreenQuadBuffers();
	}
	else if ( RendererCore::Instance()->GetRDRType() == ERDR_Deferred )
	{
		BuildScreenQuadBuffers();
	}
}


void Application::BuildGridBuffers()
{
	MeshDataPNTUV& grid = m_GridMesh.GetMesh();
	GeometryGenerator geoGen;

	geoGen.CreateGrid(20.0f, 30.0f, 50, 40, grid);


	grid.RealV.resize(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHeight(p.x, p.z);

		grid.RealV[i].Position = p;
		grid.RealV[i].Normal = GetHillNormal(p.x, p.z);
		grid.RealV[i].TexC = grid.Vertices[i].TexC;
		grid.RealV[i].TangentU = grid.Vertices[i].TangentU;

	}

	DrawElement* pElem = GenerateMeshElement<MeshDataPNTUV>(sizeof(VertexPNTUV)* grid.RealV.size(),
		"Grid", m_GridMesh, "DeferredShading", "CreateMRTPerPixelNormal", "cull", "PNTUV");

	pElem->stride = sizeof(VertexPNTUV);

	pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	TexturePtr spDiffuseMap = RendererCore::Instance()->GetTexture("stones");
	TexturePtr spNormalMap = RendererCore::Instance()->GetTexture( "stones_nmap" );
	TexturePtr spShadowMap = RendererCore::Instance()->GetTexture( "depthstencil" );

	pElem->m_vecSubElement[0].m_spDiffuseMap = spDiffuseMap;
	pElem->m_vecSubElement[0].m_spNormalMap = spNormalMap;
	pElem->m_vecSubElement[0].m_spShadowMap = spShadowMap;


	XMMATRIX mat = XMMatrixScaling(10.0f, 10.0f, 0.0f);
	XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_TexTransform, mat);

	RendererCore::Instance()->AddDrawElem(pElem);

}

void Application::BuildWavesGeometryBuffers()
{
	m_WaveMesh.GetMesh().RealV.resize(m_Waves.VertexCount());

	vector<UINT> indices(3 * m_Waves.TriangleCount());
	m_WaveMesh.GetMesh().Indices.resize(3 * m_Waves.TriangleCount());

	// Iterate over each quad.
	UINT m = m_Waves.RowCount();
	UINT n = m_Waves.ColumnCount();
	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			m_WaveMesh.GetMesh().Indices[k] = i*n + j;
			m_WaveMesh.GetMesh().Indices[k + 1] = i*n + j + 1;
			m_WaveMesh.GetMesh().Indices[k + 2] = (i + 1)*n + j;

			m_WaveMesh.GetMesh().Indices[k + 3] = (i + 1)*n + j;
			m_WaveMesh.GetMesh().Indices[k + 4] = i*n + j + 1;
			m_WaveMesh.GetMesh().Indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}


	m_pDrawWave = GenerateDMeshElement<MeshDataPNUV>(sizeof(VertexPNUV)* m_WaveMesh.GetMesh().RealV.size(),
		"Wave", m_WaveMesh, "LightTex", "TexLightTech", "transparent", "PNUV");

	m_pDrawWave->stride = sizeof(VertexPNUV);

	m_pDrawWave->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	m_pDrawWave->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	m_pDrawWave->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

	TexturePtr spTex = RendererCore::Instance()->GetTexture("water");
	m_pDrawWave->m_vecSubElement[0].m_spDiffuseMap = spTex;

	XMMATRIX wavesOffset = XMMatrixTranslation(0.0f, -3.0f, 0.0f);
	XMStoreFloat4x4(&m_pDrawWave->m_vecSubElement[0].m_World, wavesOffset);

	RendererCore::Instance()->AddDrawElem(m_pDrawWave);
}

void Application::BuildCrateGeomtryBuffers()
{
	MeshDataPNTUV& box = m_CrateMesh.GetMesh();

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	box.RealV.resize(box.Vertices.size());

	for (UINT i = 0; i < box.Vertices.size(); ++i)
	{
		box.RealV[i].Position = box.Vertices[i].Position;
		box.RealV[i].Normal = box.Vertices[i].Normal;
		box.RealV[i].TexC = box.Vertices[i].TexC;
	}


	DrawElement* pElem = GenerateMeshElement(sizeof(VertexPNUV)* box.RealV.size(),
		"Crate", m_CrateMesh, "LightTex", "TexLightTech", "alphatocoverage", "PNUV");

	pElem->stride = sizeof(VertexPNUV);

	pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

	RenderStatesPtr spRS = RendererCore::Instance()->GetRenderState("nocull");
	if (spRS)
	{
		pElem->m_pRasterS = spRS->GetRS();
	}
	TexturePtr spTex = RendererCore::Instance()->GetTexture("WireFence");
	pElem->m_vecSubElement[0].m_spDiffuseMap = spTex;

	XMMATRIX boxScale = XMMatrixScaling(15.0f, 15.0f, 15.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(8.0f, 5.0f, -15.0f);
	XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_World, boxScale*boxOffset);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_TexTransform, I);


	RendererCore::Instance()->AddDrawElem(pElem);

}

void Application::BuildRoomGeometryBuffers()
{
// 	// Create and specify geometry.  For this sample we draw a floor
// 	// and a wall with a mirror on it.  We put the floor, wall, and
// 	// mirror geometry in one vertex buffer.
// 	//
// 	//   |--------------|
// 	//   |              |
// 	//   |----|----|----|
// 	//   |Wall|Mirr|Wall|
// 	//   |    | or |    |
// 	//   /--------------/
// 	//  /   Floor      /
// 	// /--------------/
// 
// 
// 	VertexPNTUV v[30];
// 
// 	// Floor: Observe we tile texture coordinates.
// 	v[0] = VertexPNUV(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f);
// 	v[1] = VertexPNUV(-3.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
// 	v[2] = VertexPNUV(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f);
// 
// 	v[3] = VertexPNUV(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f);
// 	v[4] = VertexPNUV(7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f);
// 	v[5] = VertexPNUV(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f);
// 
// 	// Wall: Observe we tile texture coordinates, and that we
// 	// leave a gap in the middle for the mirror.
// 	v[6] = VertexPNUV(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
// 	v[7] = VertexPNUV(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
// 	v[8] = VertexPNUV(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);
// 
// 	v[9] = VertexPNUV(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
// 	v[10] = VertexPNUV(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);
// 	v[11] = VertexPNUV(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f);
// 
// 	v[12] = VertexPNUV(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
// 	v[13] = VertexPNUV(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
// 	v[14] = VertexPNUV(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
// 
// 	v[15] = VertexPNUV(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f);
// 	v[16] = VertexPNUV(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f);
// 	v[17] = VertexPNUV(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f);
// 
// 	v[18] = VertexPNUV(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 	v[19] = VertexPNUV(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
// 	v[20] = VertexPNUV(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);
// 
// 	v[21] = VertexPNUV(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 	v[22] = VertexPNUV(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f);
// 	v[23] = VertexPNUV(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f);
// 
// 	// Mirror
// 	v[24] = VertexPNUV(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 	v[25] = VertexPNUV(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
// 	v[26] = VertexPNUV(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
// 
// 	v[27] = VertexPNUV(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 	v[28] = VertexPNUV(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
// 	v[29] = VertexPNUV(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
// 
// 	MeshDataPNTUV& meshdata = m_RoomMesh.GetMesh();
// 	meshdata.RealV.resize(30);
// 	for (int i = 0; i < 30; ++i)
// 	{
// 		meshdata.RealV[i] = v[i];
// 	}
// 
// 	DrawElement* pElem = GenerateMeshElement(sizeof(MeshDataPNTUV)* meshdata.RealV.size(),
// 		"Room", m_RoomMesh, "LightTex", "TexLightTech", "nocull", "PNUV", false);
// 
// 	pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
// 	pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
// 	pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
// 
// 	TexturePtr spTex1 = Renderer::Instance()->GetTexture("Floor");
// 	TexturePtr spTex2 = Renderer::Instance()->GetTexture("Wall");
// 	pElem->m_vecSubElement[0].m_spDiffuseMap = spTex1;
// 	pElem->m_vecSubElement[0].m_VertexCount = 6;
// 	pElem->m_vecSubElement[0].m_StartVertexLocation = 0;
// 
// 	DrawElement::SubElement subElem2;
// 	pElem->m_vecSubElement.push_back(subElem2);
// 	pElem->m_vecSubElement[1].m_spDiffuseMap = spTex2;
// 	pElem->m_vecSubElement[1].m_VertexCount = 18;
// 	pElem->m_vecSubElement[1].m_StartVertexLocation = 6;
// 
// // 	XMMATRIX scaling = XMMatrixScaling(10.0f, 10.0f, 10.0f);
// // 	XMMATRIX I = XMMatrixIdentity();
// // 	XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_Wolrd, I*scaling);
// 
// 	Renderer::Instance()->AddDrawElem(pElem);
}

void Application::BuildSkullGeometryBuffers()
{
	std::ifstream fin("Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	MeshDataPNUV& meshData = m_SkullMesh.GetMesh();
	meshData.RealV.resize(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> meshData.RealV[i].Pos.x >> meshData.RealV[i].Pos.y >> meshData.RealV[i].Pos.z;
		fin >> meshData.RealV[i].Normal.x >> meshData.RealV[i].Normal.y >> meshData.RealV[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	meshData.Indices.resize(3 * tcount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> meshData.Indices[i * 3 + 0] >> meshData.Indices[i * 3 + 1] >> meshData.Indices[i * 3 + 2];
	}

	fin.close();

	DrawElement* pElem = GenerateMeshElement(sizeof(VertexPNUV)* meshData.RealV.size(),
		"Skull", m_SkullMesh, "LightTex", "NoTexLightTech", "nocull", "PNUV");
	if (pElem)
	{
		pElem->stride = sizeof(VertexPNUV);

		pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

		// 	XMMATRIX scaling = XMMatrixScaling(10.0f, 10.0f, 10.0f);
		// 	XMMATRIX I = XMMatrixIdentity();
		// 	XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_Wolrd, I*scaling);
		// Update the new world matrix.

		XMFLOAT3 mSkullTranslation = XMFLOAT3(0.0f, 1.0f, -5.0f);

		XMMATRIX skullRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
		XMMATRIX skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
		XMMATRIX skullOffset = XMMatrixTranslation(mSkullTranslation.x, mSkullTranslation.y, mSkullTranslation.z);
		XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_World, skullRotate*skullScale*skullOffset);

		RendererCore::Instance()->AddDrawElem(pElem);
	}

	//
	// Draw the mirror to stencil buffer only.
	//
	DrawElement* pElem2 = GenerateMeshElement(sizeof(VertexPNUV)* m_RoomMesh.GetMesh().RealV.size(),
		"Room", m_RoomMesh, "LightTex", "TexLightTech", "norrendertargetwrite", "PNUV", false);
	if (pElem2)
	{
		pElem2->m_vecSubElement[0].m_VertexCount = 6;
		pElem2->m_vecSubElement[0].m_StartVertexLocation = 24;

		RenderStatesPtr spRS = RendererCore::Instance()->GetRenderState("markmirror");
		pElem2->m_pDepthStencilS = spRS->GetDSS();

		pElem2->m_uiStencilRef = 1;

		RendererCore::Instance()->AddDrawElem(pElem2);
	}

	//
	// Draw the skull reflection.
	//
	DrawElement* pElem3 = GenerateMeshElement(sizeof(VertexPNUV)* m_SkullMesh.GetMesh().RealV.size(),
		"Skull", m_SkullMesh, "LightTex", "NoTexLightTech", "cullclockwise", "PNUV");
	if (pElem3)
	{
		pElem3->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		pElem3->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pElem3->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
		pElem3->m_bStencilReflect = true;

		RenderStatesPtr spRS = RendererCore::Instance()->GetRenderState("drawrelection");
		pElem3->m_pDepthStencilS = spRS->GetDSS();


		XMFLOAT3 mSkullTranslation = XMFLOAT3(0.0f, 1.0f, -5.0f);

		XMMATRIX skullRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
		XMMATRIX skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
		XMMATRIX skullOffset = XMMatrixTranslation(mSkullTranslation.x, mSkullTranslation.y, mSkullTranslation.z);
		XMStoreFloat4x4(&pElem3->m_vecSubElement[0].m_World, skullRotate*skullScale*skullOffset);

		pElem3->m_uiStencilRef = 1;

		RendererCore::Instance()->AddDrawElem(pElem3);
	}

	DrawElement* pElem4 = GenerateMeshElement(sizeof(VertexPNUV)* m_RoomMesh.GetMesh().RealV.size(),
		"Room", m_RoomMesh, "LightTex", "TexLightTech", "transparent", "PNUV", false);
	if (pElem4)
	{
		pElem4->m_vecSubElement[0].m_VertexCount = 6;
		pElem4->m_vecSubElement[0].m_StartVertexLocation = 24;

		// Reflected material is transparent so it blends into mirror.
		pElem4->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		pElem4->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
		pElem4->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

		TexturePtr spTex1 = RendererCore::Instance()->GetTexture("Mirror");
		pElem4->m_vecSubElement[0].m_spDiffuseMap = spTex1;
		
		RendererCore::Instance()->AddDrawElem(pElem4);
	}

	DrawElement* pElem5 = GenerateMeshElement(sizeof(VertexPNUV)* m_SkullMesh.GetMesh().RealV.size(),
		"Skull", m_SkullMesh, "LightTex", "NoTexLightTech", "nodoubleblend", "PNUV");
	if (pElem5)
	{
		// Reflected material is transparent so it blends into mirror.
		pElem5->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		pElem5->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
		pElem5->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

		pElem5->m_uiStencilRef = 0;
		pElem5->m_bShadowmap = true;

		RenderStatesPtr spRS = RendererCore::Instance()->GetRenderState("transparent");
		pElem5->m_pBlendS = spRS->GetBS();


		XMFLOAT3 mSkullTranslation = XMFLOAT3(0.0f, 1.0f, -5.0f);

		XMMATRIX skullRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
		XMMATRIX skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
		XMMATRIX skullOffset = XMMatrixTranslation(mSkullTranslation.x, mSkullTranslation.y, mSkullTranslation.z);
		XMStoreFloat4x4(&pElem5->m_vecSubElement[0].m_World, skullRotate*skullScale*skullOffset);

		RendererCore::Instance()->AddDrawElem(pElem5);
	}

}

void Application::BuildScreenQuadBuffers()
{
	MeshDataPUV& meshData = m_ScreenQuad.GetMesh();

	MeshDataPNTUV Quad;
	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(Quad);

	meshData.RealV.resize(Quad.Vertices.size());
	for (int i = 0; i < (int)Quad.Vertices.size(); ++i)
	{
		meshData.RealV[i].Pos = Quad.Vertices[i].Position;
		meshData.RealV[i].UV = Quad.Vertices[i].TexC;
	}

	meshData.Indices.insert(meshData.Indices.end(), Quad.Indices.begin(), Quad.Indices.end());


	DrawElement* pElem = GenerateMeshElement(sizeof(VertexPUV)* meshData.RealV.size(),
		"Quad", m_ScreenQuad, "DebugTexture", "ViewRedTech", "nocull", "PUV");

	if (pElem)
	{
		pElem->stride = sizeof(VertexPUV);

		// Scale and shift quad to lower-right corner.
		XMMATRIX world(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f);

		XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_World, world);

		//pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		//pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		//pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

		pElem->m_spShader = RendererCore::Instance()->GetShader("DebugTexture");

		string subElementTechName = "ViewRedTech";
		int iTechIndex = pElem->m_spShader->GetTech(subElementTechName);
		pElem->m_vecSubElement[0].m_iTechIndex = iTechIndex;
		pElem->m_vecSubElement[0].m_strTechname = subElementTechName;

		TexturePtr spTex = RendererCore::Instance()->GetTexture("depthstencil");
		pElem->m_vecSubElement[0].m_spDiffuseMap = spTex;

		RendererCore::Instance()->AddDrawScreenElem(pElem);
	}
}

void Application::BuildTreeSpriteBuffers()
{
	UINT uiTreeCount = 16;

	MeshDataPS& meshData = m_TreeSprite.GetMesh();
	meshData.RealV.resize(uiTreeCount);

	for (UINT i = 0; i < uiTreeCount; ++i)
	{
		float x = MathHelper::RandF(-35.0f, 35.0f);
		float z = MathHelper::RandF(-35.0f, 35.0f);
		float y = GetHeight(x, z);

		// Move tree slightly above land height.
		y += 10.0f;

		meshData.RealV[i].Pos = XMFLOAT3(x, y, z);
		meshData.RealV[i].Size = XMFLOAT2(24.0f, 24.0f);
	}

	DrawElement* pElem = GenerateMeshElement<MeshDataPS>(sizeof(VertexPS)* meshData.RealV.size(),
		"Tree", m_TreeSprite, "TreeSprite", "TreeSpriteTech", "alphatocoverage", "PS", false);

	if (pElem)
	{
		pElem->stride = sizeof(VertexPS);

		pElem->ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

		pElem->m_vecSubElement[0].m_mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		pElem->m_vecSubElement[0].m_mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

		TexturePtr spTex = RendererCore::Instance()->GetTexture("TreeArray");
		spTex->IsArray(true);
		pElem->m_vecSubElement[0].m_VertexCount = 16;
		pElem->m_vecSubElement[0].m_spDiffuseMap = spTex;

		RendererCore::Instance()->AddDrawElem(pElem);
	}

}

void Application::BuildTessellationBuffers()
{
	MeshDataP& meshData = m_TesselMesh.GetMesh();

	meshData.RealV.resize(4);
	meshData.RealV[0].Pos = XMFLOAT3(-10.0f, 0.0f, +10.0f);
	meshData.RealV[1].Pos = XMFLOAT3(+10.0f, 0.0f, +10.0f);
	meshData.RealV[2].Pos = XMFLOAT3(-10.0f, 0.0f, -10.0f);
	meshData.RealV[3].Pos = XMFLOAT3(+10.0f, 0.0f, -10.0f);

	DrawElement* pElem = GenerateMeshElement<MeshDataP>(sizeof(MeshDataP)*meshData.RealV.size(),
		"Tessel", m_TesselMesh, "Tessellation", "Tessel", "wireframe", "P", false);

	if (pElem)
	{
		pElem->stride = sizeof(XMFLOAT3);
		pElem->ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

		pElem->m_vecSubElement[0].m_VertexCount = 4;

		RendererCore::Instance()->AddDrawElem(pElem);
	}
}

void Application::BuildInstancedBuffer()
{
	MeshInstanceData& mInstancedData = m_InstanceMesh.GetMesh();
	const int n = 5;
	mInstancedData.RealV.resize(n*n*n);

	float width = 200.0f;
	float height = 200.0f;
	float depth = 200.0f;

	float x = -0.5f*width;
	float y = -0.5f*height;
	float z = -0.5f*depth;
	float dx = width / (n - 1);
	float dy = height / (n - 1);
	float dz = depth / (n - 1);
	for (int k = 0; k < n; ++k)
	{
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				// Position instanced along a 3D grid.
				mInstancedData.RealV[k*n*n + i*n + j].World = XMFLOAT4X4(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					x + j*dx, y + i*dy, z + k*dz, 1.0f);

				// Random color.
				mInstancedData.RealV[k*n*n + i*n + j].Color.x = MathHelper::RandF(0.0f, 1.0f);
				mInstancedData.RealV[k*n*n + i*n + j].Color.y = MathHelper::RandF(0.0f, 1.0f);
				mInstancedData.RealV[k*n*n + i*n + j].Color.z = MathHelper::RandF(0.0f, 1.0f);
				mInstancedData.RealV[k*n*n + i*n + j].Color.w = 1.0f;
			}
		}
	}

// 	DrawElement* pElem = GenerateDMeshElement<MeshInstanceData>(sizeof(MeshInstanceData)*mInstancedData.RealV.size(),
// 		"Instance", m_InstanceMesh, "None", "None", "None", "P");
// 
// 	if (pElem)
// 	{
// 		pElem->stride = sizeof(InstancedData);
// 
// 		pElem->m_vecSubElement[0].m_VertexCount = 4;
// 
// 		Renderer::Instance()->AddDrawElem(pElem);
// 	}

}

void Application::BuildShapeGeometryBuffers()
{
	MeshDataPNTUV box;
	MeshDataPNTUV grid;
	MeshDataPNTUV sphere;
	MeshDataPNTUV cylinder;
	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 50, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.5f, 3.0f, 15, 15, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT mBoxVertexOffset = 0;
	UINT mGridVertexOffset = box.Vertices.size();
	UINT mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
	UINT mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	UINT mBoxIndexCount = box.Indices.size();
	UINT mGridIndexCount = grid.Indices.size();
	UINT mSphereIndexCount = sphere.Indices.size();
	UINT mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT mBoxIndexOffset = 0;
	UINT mGridIndexOffset = mBoxIndexCount;
	UINT mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	UINT mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	MeshDataPNTUV& meshData = m_tgShapeMesh.GetMesh();
	meshData.RealV.resize(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		meshData.RealV[k].Position = box.Vertices[i].Position;
		meshData.RealV[k].Normal = box.Vertices[i].Normal;
		meshData.RealV[k].TexC = box.Vertices[i].TexC;
		meshData.RealV[k].TangentU = box.Vertices[i].TangentU;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		meshData.RealV[k].Position = grid.Vertices[i].Position;
		meshData.RealV[k].Normal = grid.Vertices[i].Normal;
		meshData.RealV[k].TexC = grid.Vertices[i].TexC;
		meshData.RealV[k].TangentU = grid.Vertices[i].TangentU;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		meshData.RealV[k].Position = sphere.Vertices[i].Position;
		meshData.RealV[k].Normal = sphere.Vertices[i].Normal;
		meshData.RealV[k].TexC = sphere.Vertices[i].TexC;
		meshData.RealV[k].TangentU = sphere.Vertices[i].TangentU;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		meshData.RealV[k].Position = cylinder.Vertices[i].Position;
		meshData.RealV[k].Normal = cylinder.Vertices[i].Normal;
		meshData.RealV[k].TexC = cylinder.Vertices[i].TexC;
		meshData.RealV[k].TangentU = cylinder.Vertices[i].TangentU;
	}

	meshData.Indices.insert(meshData.Indices.end(), box.Indices.begin(), box.Indices.end());
	meshData.Indices.insert(meshData.Indices.end(), grid.Indices.begin(), grid.Indices.end());
	meshData.Indices.insert(meshData.Indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	meshData.Indices.insert(meshData.Indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	for (int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mCylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
	}

	DrawElement* pElem = GenerateMeshElement(sizeof(VertexPNTUV)* m_tgShapeMesh.GetMesh().RealV.size(),
		"Shape", m_tgShapeMesh, "NormalMap", "Light3Tex", "cull", "PNTUV");

	pElem->m_spShader = RendererCore::Instance()->GetShader("NormalMap");

	if (pElem)
	{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//GRID
		pElem->stride = sizeof(VertexPNTUV);

		pElem->m_vecSubElement[0].m_IndexCount = mGridIndexCount;
		pElem->m_vecSubElement[0].m_StartIndexLocation = mGridIndexOffset;
		pElem->m_vecSubElement[0].m_BaseVertexLocation = mGridVertexOffset;

		XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_TexTransform, XMMatrixScaling(8.0f, 10.0f, 1.0f));
		
		pElem->m_vecSubElement[0].m_mat.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		pElem->m_vecSubElement[0].m_mat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
		pElem->m_vecSubElement[0].m_mat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

		TexturePtr spFloorTex = RendererCore::Instance()->GetTexture("floor2");
		TexturePtr spBrickTex = RendererCore::Instance()->GetTexture("bricks");
		TexturePtr spStoneTex = RendererCore::Instance()->GetTexture("stones");

		TexturePtr spCube = RendererCore::Instance()->GetTexture("desertcube1024");
		TexturePtr spFloornmap = RendererCore::Instance()->GetTexture("floor_nmap");
		TexturePtr spStonenmap = RendererCore::Instance()->GetTexture("stones_nmap");
		TexturePtr spBricksnmap = RendererCore::Instance()->GetTexture("bricks_nmap");
		TexturePtr spShadowMap = RendererCore::Instance()->GetTexture("depthstencil");

		pElem->m_vecSubElement[0].m_spDiffuseMap = spFloorTex;
		pElem->m_vecSubElement[0].m_spCubeMap = spCube;
		pElem->m_vecSubElement[0].m_spNormalMap = spFloornmap;
		pElem->m_vecSubElement[0].m_spShadowMap = spShadowMap;

		string subElementTechName = "Light3Tex";
		int iTechIndex = pElem->m_spShader->GetTech(subElementTechName);
		pElem->m_vecSubElement[0].m_iTechIndex = iTechIndex;
		pElem->m_vecSubElement[0].m_strTechname = subElementTechName;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//BOX
		DrawElement::SubElement subElem2;
		pElem->m_vecSubElement.push_back(subElem2);

		pElem->m_vecSubElement[1].m_IndexCount = mBoxIndexCount;
		pElem->m_vecSubElement[1].m_StartIndexLocation = mBoxIndexOffset;
		pElem->m_vecSubElement[1].m_BaseVertexLocation = mBoxVertexOffset;

		XMStoreFloat4x4(&pElem->m_vecSubElement[1].m_TexTransform, XMMatrixScaling(2.0f, 1.0f, 1.0f));

		pElem->m_vecSubElement[1].m_mat.Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pElem->m_vecSubElement[1].m_mat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pElem->m_vecSubElement[1].m_mat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
		pElem->m_vecSubElement[1].m_mat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

		pElem->m_vecSubElement[1].m_spDiffuseMap = spBrickTex;
		pElem->m_vecSubElement[1].m_spCubeMap = spCube;
		pElem->m_vecSubElement[1].m_spNormalMap = spBricksnmap;
		pElem->m_vecSubElement[1].m_spShadowMap = spShadowMap;

		XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
		XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
		XMStoreFloat4x4(&pElem->m_vecSubElement[1].m_World, XMMatrixMultiply(boxScale, boxOffset));

		subElementTechName = "Light3Tex";
		iTechIndex = pElem->m_spShader->GetTech(subElementTechName);
		pElem->m_vecSubElement[1].m_iTechIndex = iTechIndex;
		pElem->m_vecSubElement[1].m_strTechname = subElementTechName;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Draw the cylinders.
		for (int i = 0; i < 10; ++i)
		{

			DrawElement::SubElement subElem2;
			pElem->m_vecSubElement.push_back(subElem2);

			UINT uiIndex = pElem->m_vecSubElement.size() - 1;

			pElem->m_vecSubElement[uiIndex].m_World = mCylWorld[i];
			pElem->m_vecSubElement[uiIndex].m_IndexCount = mCylinderIndexCount;
			pElem->m_vecSubElement[uiIndex].m_StartIndexLocation = mCylinderIndexOffset;
			pElem->m_vecSubElement[uiIndex].m_BaseVertexLocation = mCylinderVertexOffset;

			XMStoreFloat4x4(&pElem->m_vecSubElement[uiIndex].m_TexTransform, XMMatrixScaling(1.0f, 2.0f, 1.0f));

			pElem->m_vecSubElement[uiIndex].m_mat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			pElem->m_vecSubElement[uiIndex].m_mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			pElem->m_vecSubElement[uiIndex].m_mat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
			pElem->m_vecSubElement[uiIndex].m_mat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

			pElem->m_vecSubElement[uiIndex].m_spDiffuseMap = spBrickTex;
			pElem->m_vecSubElement[uiIndex].m_spCubeMap = spCube;
			pElem->m_vecSubElement[uiIndex].m_spNormalMap = spBricksnmap;
			pElem->m_vecSubElement[uiIndex].m_spShadowMap = spShadowMap;

			subElementTechName = "Light3Tex";
			iTechIndex = pElem->m_spShader->GetTech(subElementTechName);
			pElem->m_vecSubElement[uiIndex].m_iTechIndex = iTechIndex;
			pElem->m_vecSubElement[uiIndex].m_strTechname = subElementTechName;
		}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Draw the spheres.
		for (int i = 0; i < 10; ++i)
		{
			DrawElement::SubElement subElem2;
			pElem->m_vecSubElement.push_back(subElem2);

			UINT uiIndex = pElem->m_vecSubElement.size() - 1;

			pElem->m_vecSubElement[uiIndex].m_World = mSphereWorld[i];
			pElem->m_vecSubElement[uiIndex].m_IndexCount = mSphereIndexCount;
			pElem->m_vecSubElement[uiIndex].m_StartIndexLocation = mSphereIndexOffset;
			pElem->m_vecSubElement[uiIndex].m_BaseVertexLocation = mSphereVertexOffset;

			XMStoreFloat4x4(&pElem->m_vecSubElement[uiIndex].m_TexTransform, XMMatrixIdentity());

			pElem->m_vecSubElement[uiIndex].m_mat.Ambient  = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
			pElem->m_vecSubElement[uiIndex].m_mat.Diffuse  = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
			pElem->m_vecSubElement[uiIndex].m_mat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
			pElem->m_vecSubElement[uiIndex].m_mat.Reflect  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);

			//pElem->m_vecSubElement[uiIndex].m_spDiffuseMap = spStoneTex;
			pElem->m_vecSubElement[uiIndex].m_spCubeMap = spCube;
			//pElem->m_vecSubElement[uiIndex].m_spNormalMap = spStonenmap;
			pElem->m_vecSubElement[uiIndex].m_spShadowMap = spShadowMap;

			subElementTechName = "Light3Tex";
			iTechIndex = pElem->m_spShader->GetTech(subElementTechName);
			pElem->m_vecSubElement[uiIndex].m_iTechIndex = iTechIndex;
			pElem->m_vecSubElement[uiIndex].m_strTechname = subElementTechName;
		}

		RendererCore::Instance()->AddDrawElem(pElem);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			// add shadow draw element
			DrawElement* pShadowElem = new DrawElement();

			pShadowElem->stride = pElem->stride;

			pShadowElem->m_spVB = pElem->m_spVB;
			pShadowElem->m_spIB = pElem->m_spIB;
			pShadowElem->m_bDrawIndex = pElem->m_bDrawIndex;
			pShadowElem->m_bDynamic = pElem->m_bDynamic;
			pShadowElem->m_bShadowmap = pElem->m_bShadowmap;
			pShadowElem->m_bInstance = pElem->m_bInstance;
			pShadowElem->m_bStencilReflect = pElem->m_bStencilReflect;


			pShadowElem->ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			RenderStatesPtr spShadowRS = RendererCore::Instance()->GetRenderState("cull");
			if (spShadowRS)
			{
				pShadowElem->m_pBlendS = spShadowRS->GetBS();
				pShadowElem->m_pRasterS = spShadowRS->GetRS();
				pShadowElem->m_pDepthStencilS = spShadowRS->GetDSS();
			}

			pShadowElem->m_vecSubElement.insert(pShadowElem->m_vecSubElement.end(), 
				pElem->m_vecSubElement.begin(), pElem->m_vecSubElement.end());

			pShadowElem->m_spShader = RendererCore::Instance()->GetShader("BuildShadowMap");

			subElementTechName = "BuildShadowMapTech";
			iTechIndex = pShadowElem->m_spShader->GetTech(subElementTechName);
			for (auto itor = pShadowElem->m_vecSubElement.begin(); itor < pShadowElem->m_vecSubElement.end(); ++itor)
			{
				(*itor).m_iTechIndex = iTechIndex;
				(*itor).m_strTechname = subElementTechName;
			}

			BitVDeclPtr spBitVDecl = RendererCore::Instance()->GetBitVDecl("PNTUV");
			bitset<16>& rbitVDecl = *spBitVDecl.get();
			VertexDeclarationPtr spVDecl = VertexDeclaration::Create(rbitVDecl, pElem->m_spShader->GetTech(iTechIndex));
			RendererCore::Instance()->InsertVDecl(subElementTechName + "PNTUV", spVDecl);

			pShadowElem->m_pInputLayout = spVDecl->GetInLayout();



			RendererCore::Instance()->AddDrawShadowElem(pShadowElem);
		}
	}
}

void Application::BuildSkyBuffers()
{
	MeshDataP& meshData = m_SkyMesh.GetMesh();

	MeshDataPNTUV sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(5000.0f, 30, 30, sphere);

	meshData.RealV.resize(sphere.Vertices.size());

	for (size_t i = 0; i < sphere.Vertices.size(); ++i)
	{
		meshData.RealV[i].Pos = sphere.Vertices[i].Position;
	}

	meshData.Indices.insert(meshData.Indices.end(), sphere.Indices.begin(), sphere.Indices.end());


	DrawElement* pElem = GenerateMeshElement<MeshDataP>(sizeof(VertexP)*meshData.RealV.size(),
		"Sky", m_SkyMesh, "Sky", "SkyTech", "cull", "P");

	if (pElem)
	{
		pElem->stride = sizeof(XMFLOAT3);
		pElem->ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		pElem->IBFormat = DXGI_FORMAT_R16_UINT;

		pElem->m_spShader = RendererCore::Instance()->GetShader("Sky");

		string subElementTechName = "SkyTech";
		int iTechIndex = pElem->m_spShader->GetTech(subElementTechName);
		pElem->m_vecSubElement[0].m_iTechIndex = iTechIndex;
		pElem->m_vecSubElement[0].m_strTechname = subElementTechName;

		TexturePtr spTex = RendererCore::Instance()->GetTexture("desertcube1024");
		pElem->m_vecSubElement[0].m_spCubeMap = spTex;

		RendererCore::Instance()->AddDrawElem(pElem);
	}

}

void Application::BuildSkullGeometryBuffers2()
{
	std::ifstream fin("Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	MeshDataPNUV& meshData = m_SkullMesh.GetMesh();
	meshData.RealV.resize(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> meshData.RealV[i].Pos.x >> meshData.RealV[i].Pos.y >> meshData.RealV[i].Pos.z;
		fin >> meshData.RealV[i].Normal.x >> meshData.RealV[i].Normal.y >> meshData.RealV[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	meshData.Indices.resize(3 * tcount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> meshData.Indices[i * 3 + 0] >> meshData.Indices[i * 3 + 1] >> meshData.Indices[i * 3 + 2];
	}

	fin.close();

	DrawElement* pElem = GenerateMeshElement(sizeof(VertexPNUV)* meshData.RealV.size(),
		"Skull", m_SkullMesh, "Basic", "Light3Reflect", "cull", "PNUV");
	if (pElem)
	{
		pElem->stride = sizeof(VertexPNUV);

		XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_TexTransform, XMMatrixIdentity());

		pElem->m_vecSubElement[0].m_mat.Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		pElem->m_vecSubElement[0].m_mat.Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
		pElem->m_vecSubElement[0].m_mat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
		pElem->m_vecSubElement[0].m_mat.Reflect  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

		XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
		XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

		XMStoreFloat4x4(&pElem->m_vecSubElement[0].m_World, XMMatrixMultiply(skullScale, skullOffset));

		//XMFLOAT3 mSkullTranslation = XMFLOAT3(0.0f, 1.0f, -5.0f);
		//XMMATRIX skullRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
		//XMMATRIX skullScale = XMMatrixScaling(0.45f, 0.45f, 0.45f);
		//XMMATRIX skullOffset = XMMatrixTranslation(mSkullTranslation.x, mSkullTranslation.y, mSkullTranslation.z);

		TexturePtr spCube = RendererCore::Instance()->GetTexture("desertcube1024");
		pElem->m_vecSubElement[0].m_spCubeMap = spCube;
		TexturePtr spShadowMap = RendererCore::Instance()->GetTexture("depthstencil");
		pElem->m_vecSubElement[0].m_spShadowMap = spShadowMap;

		pElem->m_spShader = RendererCore::Instance()->GetShader("Basic");

		string subElementTechName = "Light3Reflect";
		int iTechIndex = pElem->m_spShader->GetTech(subElementTechName);
		pElem->m_vecSubElement[0].m_iTechIndex = iTechIndex;
		pElem->m_vecSubElement[0].m_strTechname = subElementTechName;

		RendererCore::Instance()->AddDrawElem(pElem);

		{
			// add shadow draw element
			DrawElement* pShadowElem = new DrawElement();

			pShadowElem->stride = pElem->stride;

			pShadowElem->m_spVB = pElem->m_spVB;
			pShadowElem->m_spIB = pElem->m_spIB;
			pShadowElem->m_bDrawIndex = pElem->m_bDrawIndex;
			pShadowElem->m_bDynamic = pElem->m_bDynamic;
			pShadowElem->m_bShadowmap = pElem->m_bShadowmap;
			pShadowElem->m_bInstance = pElem->m_bInstance;
			pShadowElem->m_bStencilReflect = pElem->m_bStencilReflect;


			pShadowElem->ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			RenderStatesPtr spShadowRS = RendererCore::Instance()->GetRenderState("cull");
			if (spShadowRS)
			{
				pShadowElem->m_pBlendS = spShadowRS->GetBS();
				pShadowElem->m_pRasterS = spShadowRS->GetRS();
				pShadowElem->m_pDepthStencilS = spShadowRS->GetDSS();
			}


			pShadowElem->m_vecSubElement.insert(pShadowElem->m_vecSubElement.end(),
				pElem->m_vecSubElement.begin(), pElem->m_vecSubElement.end());

			pShadowElem->m_spShader = RendererCore::Instance()->GetShader("BuildShadowMap");

			subElementTechName = "BuildShadowMapTech";
			iTechIndex = pShadowElem->m_spShader->GetTech(subElementTechName);
			for (auto itor = pShadowElem->m_vecSubElement.begin(); itor < pShadowElem->m_vecSubElement.end(); ++itor)
			{
				(*itor).m_iTechIndex = iTechIndex;
				(*itor).m_strTechname = subElementTechName;
			}

			BitVDeclPtr spBitVDecl = RendererCore::Instance()->GetBitVDecl("PNUV");
			bitset<16>& rbitVDecl = *spBitVDecl.get();
			VertexDeclarationPtr spVDecl = VertexDeclaration::Create(rbitVDecl, pElem->m_spShader->GetTech(iTechIndex));
			RendererCore::Instance()->InsertVDecl(subElementTechName + "PNUV", spVDecl);

			pShadowElem->m_pInputLayout = spVDecl->GetInLayout();

			RendererCore::Instance()->AddDrawShadowElem(pShadowElem);
		}

	}
}

void Application::BuildFloorGeometryBuffers()
{

}

void Application::BulidColumnGeomtryBuffers()
{

}

// 


// void Application::BuildFX()
// {
// 	/*
// 	DWORD shaderFlags = 0;
// #if defined( DEBUG ) || defined( _DEBUG )
// 	shaderFlags |= D3D10_SHADER_DEBUG;
// 	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
// #endif
// 
// 	ID3D10Blob* compiledShader = 0;
// 	ID3D10Blob* compilationMsgs = 0;
// 	HRESULT hr = D3DX11CompileFromFile(L"Media/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
// 		0, 0, &compiledShader, &compilationMsgs, 0);
// 	// compilationMsgs can store errors or warnings.
// 	if (compilationMsgs != 0)
// 	{
// 		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
// 		ReleaseCOM(compilationMsgs);
// 	}
// 
// 	// Even if there are no compilationMsgs, check to make sure there were no other errors.
// 	if (FAILED(hr))
// 	{
// 		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
// 	}
// 
// 	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
// 		0, m_pd3dDevice, &m_pFX));
// 
// 	// Done with compiled shader.
// 	ReleaseCOM(compiledShader);
// 	/**/
// 
// }
// 
// void Application::BuildVertexLayout()
// {
// // 	// Create the vertex input layout.
// // 	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
// // 	{
// // 		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
// // 		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
// // 	};
// // 
// // 	// Create the input layout
// // 	D3DX11_PASS_DESC passDesc;
// // 	m_pTech->GetPassByIndex(0)->GetDesc(&passDesc);
// // 	HR(m_pd3dDevice->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
// // 		passDesc.IAInputSignatureSize, &m_pInputLayout));
// }
