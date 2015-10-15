#pragma once
#include "BaseResource.h"
#include "float2.h"
#include "float3.h"
#include <d3d11_1.h>


struct SimpleVertex
{
	float3 Position;
	float3 Normal;
	float3 Tangent;
	float2 TexCoord;
};


class Mesh : public BaseResource
{
public:
	Mesh() : BaseResource() {}
	~Mesh() { CleanUp(); }

	void InitFromData(SimpleVertex* inVertexData, int inNumVerts, WORD* inIndexData, int inNumIndices);
	void InitCube(float3 inScale = { 1, 1, 1 });
	void InitPlane(int inSubX, int inSubY, float2 inScale = { 1, 1 });
	void InitFullscreenTriangle();
	void InitFromFile(std::string inFileName);

	ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return mIndexBuffer; }
	UINT GetStride() { return mStride; }
	UINT GetOffset() { return mOffset; }
	UINT GetNumVerts() { return mNumVerts; }
	UINT GetNumIndices() { return mNumIndices; }

	void CleanUp();
	ResourceType GetResourceType() const { return ResourceType::MESH; };

private:

	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mIndexBuffer = nullptr;
	UINT mStride = 0;
	UINT mOffset = 0;
	UINT mNumVerts = 0;
	UINT mNumIndices = 0;
};

