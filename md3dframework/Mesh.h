#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"

struct ID3D11Buffer;

REGISTERCLASS(Mesh);


struct SimpleVertex
{
	float3 Position;
	float3 Normal;
	float3 Tangent;
	float2 TexCoord;
};


class Mesh : public BaseResource
{
	friend class ResourceFactory;
	friend class RenderContext;

public:
	void InitFromData(SimpleVertex* inVertexData, int inNumVerts, unsigned short* inIndexData, int inNumIndices);
	void InitCube(float3 inScale = { 1, 1, 1 });
	void InitPlane(int2 inNumVertices, float2 inScale = { 1, 1 });
	void InitFullscreenTriangle();
	void InitFromFile(std::string inFileName);
	
	AABB GetAABB() { return mAABB; }

	void CleanUp() override;
	
	ResourceType GetResourceType() const override { return ResourceType::MESH; }

protected:

	AABB mAABB;
	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mIndexBuffer = nullptr;
	unsigned int mStride = 0;
	unsigned int mOffset = 0;
	unsigned int mNumVerts = 0;
	unsigned int mNumIndices = 0;

private:
	Mesh() : BaseResource() {}
	~Mesh() { CleanUp(); }

	Mesh(Mesh const&) = delete;
	void operator=(Mesh const&) = delete;
};
