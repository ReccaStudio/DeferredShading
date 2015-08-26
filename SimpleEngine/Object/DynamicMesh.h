#pragma once

#include "d3dUtil.h"
#include "VertexDefine.h"
#include "VertexDeclaration.h"

class VertexBuffer;
class IndexBuffer;
class FXShader;
class VertexDeclaration;

template<typename VertexType>
class DynamicMesh
{
public:
	DynamicMesh() {}
	virtual ~DynamicMesh() {}

	void Update(float dt) {}

	VertexType& GetMesh() { return m_MeshData; }

	const void* GetData() {
		return &m_MeshData.RealV[0];
	}

protected:
	VertexType m_MeshData;
};


typedef DynamicMesh<MeshDataP>	DynamicMeshP;
typedef DynamicMesh<MeshDataPN>	DynamicMeshPN;
typedef DynamicMesh<MeshDataPNUV> DynamicMeshPNUV;
typedef DynamicMesh<MeshDataPS>	DynamicMeshPS;
typedef DynamicMesh<MeshDataPNTUV>	DynamicMeshPNTUV;
typedef DynamicMesh<MeshInstanceData>	InstanceMesh;
