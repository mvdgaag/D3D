#pragma once
#include "BaseResource.h"
#include "GAAGCommon.h"

struct ID3D11Buffer;

PREDEFINE(Mesh, pMesh);


struct SimpleVertex
{
	float3 Position;
	float3 Normal;
	float3 Tangent;
	float2 TexCoord;
};


class Mesh : public BaseResource
{
	friend class RenderContext;

public:
	Mesh() : BaseResource() {}
	~Mesh() { CleanUp(); }

	void InitFromData(SimpleVertex* inVertexData, int inNumVerts, unsigned short* inIndexData, int inNumIndices);
	void InitCube(float3 inScale = { 1, 1, 1 });
	void InitPlane(int inSubX, int inSubY, float2 inScale = { 1, 1 });
	void InitFullscreenTriangle();
	void InitFromFile(std::string inFileName);

	void CleanUp() override;
	
	ResourceType GetResourceType() const override { return ResourceType::MESH; };

protected:

	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mIndexBuffer = nullptr;
	unsigned int mStride = 0;
	unsigned int mOffset = 0;
	unsigned int mNumVerts = 0;
	unsigned int mNumIndices = 0;
};
