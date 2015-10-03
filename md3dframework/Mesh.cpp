#include "Mesh.h"
#include <assert.h>
#include "LinearAlgebra.h"
#include "main.h"
#include "FrameWork.h"


struct SimpleVertex
{
	float3 Position;
	float3 Normal;
	float3 Tangent;
	float2 TexCoord;
};


void Mesh::InitCube()
{
	CleanUp();

	// Create vertex buffer
	mNumVerts = 24;
	SimpleVertex vertices[] =
	{
		// +Y
		{ float3(-1.0f, 1.0f, -1.0f), float3(0.0f, 1.0f, 0.0f),  float3(1.0f, 0.0f, 0.0f),  float2(1.0f, 0.0f) },
		{ float3( 1.0f, 1.0f, -1.0f), float3(0.0f, 1.0f, 0.0f),  float3(1.0f, 0.0f, 0.0f),  float2(0.0f, 0.0f) },
		{ float3( 1.0f, 1.0f,  1.0f), float3(0.0f, 1.0f, 0.0f),  float3(1.0f, 0.0f, 0.0f),  float2(0.0f, 1.0f) },
		{ float3(-1.0f, 1.0f,  1.0f), float3(0.0f, 1.0f, 0.0f),  float3(1.0f, 0.0f, 0.0f),  float2(1.0f, 1.0f) },

		// -Y
		{ float3(-1.0f, -1.0f, -1.0f), float3(0.0f, -1.0f, 0.0f), float3(1.0f, 0.0f, 0.0f),  float2(0.0f, 0.0f) },
		{ float3( 1.0f, -1.0f, -1.0f), float3(0.0f, -1.0f, 0.0f), float3(1.0f, 0.0f, 0.0f),  float2(1.0f, 0.0f) },
		{ float3( 1.0f, -1.0f,  1.0f), float3(0.0f, -1.0f, 0.0f), float3(1.0f, 0.0f, 0.0f),  float2(1.0f, 1.0f) },
		{ float3(-1.0f, -1.0f,  1.0f), float3(0.0f, -1.0f, 0.0f), float3(1.0f, 0.0f, 0.0f),  float2(0.0f, 1.0f) },

		// -X
		{ float3(-1.0f, -1.0f,  1.0f), float3(-1.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f),  float2(0.0f, 1.0f) },
		{ float3(-1.0f, -1.0f, -1.0f), float3(-1.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f),  float2(1.0f, 1.0f) },
		{ float3(-1.0f,  1.0f, -1.0f), float3(-1.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f),  float2(1.0f, 0.0f) },
		{ float3(-1.0f,  1.0f,  1.0f), float3(-1.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f),  float2(0.0f, 0.0f) },

		// +X
		{ float3(1.0f, -1.0f,  1.0f),  float3(1.0f, 0.0f, 0.0f),  float3(0.0f, 0.0f, 1.0f),  float2(1.0f, 1.0f) },
		{ float3(1.0f, -1.0f, -1.0f),  float3(1.0f, 0.0f, 0.0f),  float3(0.0f, 0.0f, 1.0f),  float2(0.0f, 1.0f) },
		{ float3(1.0f,  1.0f, -1.0f),  float3(1.0f, 0.0f, 0.0f),  float3(0.0f, 0.0f, 1.0f),  float2(0.0f, 0.0f) },
		{ float3(1.0f,  1.0f,  1.0f),  float3(1.0f, 0.0f, 0.0f),  float3(0.0f, 0.0f, 1.0f),  float2(1.0f, 0.0f) },

		// -Z
		{ float3(-1.0f, -1.0f, -1.0f), float3(0.0f, 0.0f, -1.0f), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 1.0f) },
		{ float3( 1.0f, -1.0f, -1.0f), float3(0.0f, 0.0f, -1.0f), float3(-1.0f, 0.0f, 0.0f), float2(1.0f, 1.0f) },
		{ float3( 1.0f,  1.0f, -1.0f), float3(0.0f, 0.0f, -1.0f), float3(-1.0f, 0.0f, 0.0f), float2(1.0f, 0.0f) },
		{ float3(-1.0f,  1.0f, -1.0f), float3(0.0f, 0.0f, -1.0f), float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) },

		// +Z
		{ float3(-1.0f, -1.0f, 1.0f), float3(0.0f, 0.0f, 1.0f),  float3(-1.0f, 0.0f, 0.0f), float2(1.0f, 1.0f) },
		{ float3( 1.0f, -1.0f, 1.0f), float3(0.0f, 0.0f, 1.0f),  float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 1.0f) },
		{ float3( 1.0f,  1.0f, 1.0f), float3(0.0f, 0.0f, 1.0f),  float3(-1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) },
		{ float3(-1.0f,  1.0f, 1.0f), float3(0.0f, 0.0f, 1.0f),  float3(-1.0f, 0.0f, 0.0f), float2(1.0f, 0.0f) }
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * mNumVerts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	D3DCall(theFramework.GetDevice()->CreateBuffer(&bd, &InitData, &mVertexBuffer));
	
	mStride = sizeof(SimpleVertex);
	mOffset = 0;
	
	// Create index buffer
	mNumIndices = 36;
	WORD indices[] =
	{
		3, 1, 0, 		2, 1, 3,		
		6, 4, 5,		7, 4, 6,		
		11, 9, 8,		10, 9, 11,	
		14, 12, 13,		15, 12, 14,		
		19, 17, 16,		18, 17, 19,		
		22, 20, 21,		23, 20, 22
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * mNumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	D3DCall(theFramework.GetDevice()->CreateBuffer(&bd, &InitData, &mIndexBuffer));

	assert(mVertexBuffer != nullptr);
	assert(mIndexBuffer != nullptr);
}


void Mesh::InitTriangle()
{
	CleanUp();

	// Create vertex buffer
	mNumVerts = 3;
	SimpleVertex vertices[] =
	{
		{ float3(0.0f, 1.0f, 0.0f),  float3(0.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, 0.0f), float2(0.5f, 1.0f) },
		{ float3(-0.5f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, 0.0f), float2(0.0f, 0.0f) },
		{ float3(0.5f, 0.0f, 0.0f),	 float3(0.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, 0.0f), float2(1.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * mNumVerts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	D3DCall(theFramework.GetDevice()->CreateBuffer(&bd, &InitData, &mVertexBuffer));

	// Set vertex buffer
	mStride = sizeof(SimpleVertex);
	mOffset = 0;

	// Create index buffer
	mNumIndices = 3;
	WORD indices[] = { 0, 1, 2 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * mNumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;
	D3DCall(theFramework.GetDevice()->CreateBuffer(&bd, &InitData, &mIndexBuffer));

	assert(mVertexBuffer != nullptr);
	assert(mIndexBuffer != nullptr);
}


void InitFromFile(std::string inFileName)
{}


void Mesh::CleanUp()
{
	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = NULL;
	}
	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = NULL;
	}
}
