#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>


class Mesh : public BaseResource
{
public:
	Mesh(std::string inName) : BaseResource(inName) {}
	~Mesh() { CleanUp(); }

	void Init() { InitCube(); } // initializes to cube as default
	void InitCube();
	void InitTriangle();
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

