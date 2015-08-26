#pragma once

struct VertexP
{
	XMFLOAT3 Pos;
};

struct VertexPUV
{
	XMFLOAT3 Pos;
	XMFLOAT2 UV;
};

struct VertexPN
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
};

struct VertexPNUV
{
	VertexPNUV() : Pos(0.0f, 0.0f, 0.0f), Normal(0.0f, 0.0f, 0.0f), UV(0.0f, 0.0f) {}
	VertexPNUV(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
		: Pos(px, py, pz), Normal(nx, ny, nz), UV(u, v) {}
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

struct VertexPC
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct VertexPS
{
	XMFLOAT3 Pos;
	XMFLOAT2 Size;
};

struct InstancedData
{
	XMFLOAT4X4 World;
	XMFLOAT4 Color;
};

struct VertexPNTUV
{
	VertexPNTUV(){}
	VertexPNTUV(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
		: Position(p), Normal(n), TangentU(t), TexC(uv){}
	VertexPNTUV(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: Position(px, py, pz), Normal(nx, ny, nz),
		TangentU(tx, ty, tz), TexC(u, v){}

	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	XMFLOAT3 TangentU;
};

template<typename VertexType>
struct TMeshData
{
	std::vector<VertexPNTUV> Vertices;
	std::vector<UINT> Indices;

	std::vector<VertexType> RealV;
};

typedef TMeshData<VertexP> MeshDataP;
typedef TMeshData<VertexPUV> MeshDataPUV;
typedef TMeshData<VertexPN> MeshDataPN;
typedef TMeshData<VertexPNUV> MeshDataPNUV;
typedef TMeshData<VertexPS> MeshDataPS;
typedef TMeshData<VertexPNTUV> MeshDataPNTUV;
typedef TMeshData<InstancedData> MeshInstanceData;