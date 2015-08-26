#pragma once

#include "d3dApp.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "Waves.h"
#include "Camera.h"

class DrawElement;

class Application : public D3DApp
{
public:
	Application(HINSTANCE hInstance);
	virtual ~Application();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	float GetHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;

	void ShadowmapScene();

	void BuildGridBuffers();
	void BuildWavesGeometryBuffers();
	void BuildCrateGeomtryBuffers();
	void BuildRoomGeometryBuffers();
	void BuildSkullGeometryBuffers();
	void BuildScreenQuadBuffers();
	void BuildTreeSpriteBuffers();
	void BuildTessellationBuffers();
	void BuildInstancedBuffer();
	void BuildShapeGeometryBuffers();
	void BuildSkyBuffers();
	void BuildSkullGeometryBuffers2();
	void BuildFloorGeometryBuffers();
	void BulidColumnGeomtryBuffers();

protected:
	Waves m_Waves;
	StaticMeshPNTUV m_GridMesh;
	StaticMeshPNTUV m_CrateMesh;
	StaticMeshPNTUV m_RoomMesh;
	StaticMeshPNUV m_SkullMesh;
	StaticMeshPNTUV m_ShapeMesh;
	StaticMeshPNTUV m_tgShapeMesh;
	StaticMeshPUV m_ScreenQuad;
	StaticMeshPS m_TreeSprite;
	StaticMeshP  m_TesselMesh;
	InstanceMesh m_InstanceMesh;
	StaticMeshP  m_SkyMesh;


	DynamicMeshPNUV m_WaveMesh;

	DrawElement* m_pDrawWave;
	XMFLOAT2 m_WaterTexOffset;

	float mTheta;
	float mPhi;
	float mRadius;

	Camera m_Camera;

	POINT mLastMousePos;
};

