#pragma once

#include "d3dUtil.h"
#include "VertexDefine.h"
#include "VertexDeclaration.h"

class VertexBuffer;
class IndexBuffer;
class FXShader;
class VertexDeclaration;

template<typename VertexType>
class StaticMesh
{
public:
	StaticMesh() {}
	virtual ~StaticMesh() {}

	VertexType& GetMesh() { return m_MeshData; }

	const void* GetData() {
		return &m_MeshData.RealV[0];
	}

protected:
	VertexType m_MeshData;
};

typedef StaticMesh<MeshDataP>	StaticMeshP;
typedef StaticMesh<MeshDataPUV>	StaticMeshPUV;
typedef StaticMesh<MeshDataPN>	StaticMeshPN;
typedef StaticMesh<MeshDataPNUV> StaticMeshPNUV;
typedef StaticMesh<MeshDataPS>	StaticMeshPS;
typedef StaticMesh<MeshDataPNTUV>	StaticMeshPNTUV;
