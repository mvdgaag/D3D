#include "Mesh.h"
#include <assert.h>
#include "LinearAlgebra.h"
#include "main.h"
#include "Framework.h"
#include <iostream>
#include <fstream>
#include <sstream>


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



std::vector<std::string> StringSplit(const std::string &s, char delim) 
{
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) 
		elems.push_back(item);
	return elems;
}


void Mesh::InitFromFile(std::string inFileName)
{
	CleanUp();

	std::vector<float3> positions;
	std::vector<float3> normals;
	std::vector<float2> texcoords;
	
	std::vector<SimpleVertex> vertices;
	std::vector<WORD> indices;

	std::ifstream file;
	file.open(inFileName.c_str(), std::ifstream::in);
	while (file.good())
	{
		std::string line;
		std::getline(file, line);
		if (line.length() < 3)
			continue;
		char c1 = line[0];
		char c2 = line[1];
		line = line.substr(2, line.length());
		std::stringstream ss(line);
		switch (c1)
		{
		case 'v':
		{
			switch (c2)
			{
			case ' ':
			{
				float3 vert;
				for (int i = 0; i < 3; i++)
					assert(ss >> vert[i]);
				positions.push_back(vert);
				break;
			}
			case 't':
			{
				float2 texcoord;
				for (int i = 0; i < 2; i++)
					assert(ss >> texcoord[i]);
				texcoords.push_back(texcoord);
				break;
			}
			case 'n':
			{
				float3 normal;
				for (int i = 0; i < 3; i++)
					assert(ss >> normal[i]);
				normals.push_back(normal);
				break;
			}
			default:
				break;
			}
			break;
		}
		case 'f':
		{
			SimpleVertex vert;
			std::vector<std::string> face_strings = StringSplit(line, ' ');
			assert(face_strings.size() == 3);
			for (int i = 0; i < 3; i++)
			{
				std::vector<std::string> index_data = StringSplit(face_strings[i], '/');
				assert(index_data.size() == 3);
				
				int idx = std::stoi(index_data[0]) - 1;
				assert(idx >= 0 && idx < positions.size());
				vert.Position = positions[idx];
				
				idx = std::stoi(index_data[1]) - 1;
				assert(idx >= 0 && idx < texcoords.size());
				vert.TexCoord = texcoords[idx];

				idx = std::stoi(index_data[2]) - 1;
				assert(idx >= 0 && idx < normals.size());
				vert.Normal = normals[idx];

				// search backwards (likely to be near the end for faces sharing verts)
				// could be optimized by using a hash table
				for (int j = vertices.size()-1; j >= -1; j--)
				{
					// nothing found means new unique vertex
					if (j == -1)
					{
						indices.push_back(vertices.size());
						vertices.push_back(vert);
					}
					// check if this vertex is the same
					else if (	vertices[j].Position == vert.Position &&
								vertices[j].TexCoord == vert.TexCoord &&
								vertices[j].Normal == vert.Normal)
					{
						indices.push_back(j);
						break;
					}
				}
			}
			break;
		}
		default:
			break;
		}
	}
	file.close();

	// calculate tangents
	std::vector<float3> tan1;
	std::vector<float3> tan2;
	for (int i = 0; i < vertices.size(); i++)
	{
		tan1.push_back(float3(0, 0, 0));
		tan2.push_back(float3(0, 0, 0));
	}

	for (int i = 0; i < indices.size(); i += 3)
	{
		int idx1 = indices[i];
		int idx2 = indices[i+1];
		int idx3 = indices[i+2];

		float3 pos1 = vertices[idx1].Position;
		float3 pos2 = vertices[idx2].Position;
		float3 pos3 = vertices[idx3].Position;

		float2 uv1 = vertices[idx1].TexCoord;
		float2 uv2 = vertices[idx2].TexCoord;
		float2 uv3 = vertices[idx3].TexCoord;

		float3 edge_pos1 = pos2 - pos1;
		float3 edge_pos2 = pos3 - pos1;

		float2 edge_uv1 = uv2 - uv1;
		float2 edge_uv2 = uv3 - uv1;

		float r = 1.0 / (edge_uv1.x * edge_uv2.y - edge_uv2.x * edge_uv1.y);
		
		float3 sdir((edge_uv2.y * edge_pos1.x - edge_uv1.y * edge_pos2.x) * r, (edge_uv2.y * edge_pos1.y - edge_uv1.y * edge_pos2.y) * r,
			(edge_uv2.y * edge_pos1.z - edge_uv1.y * edge_pos2.z) * r);
		float3 tdir((edge_uv1.x * edge_pos2.x - edge_uv2.x * edge_pos1.x) * r, (edge_uv1.x * edge_pos2.y - edge_uv2.x * edge_pos1.y) * r,
			(edge_uv1.x * edge_pos2.z - edge_uv2.x * edge_pos1.z) * r);

		tan1[idx1] += sdir;
		tan1[idx2] += sdir;
		tan1[idx3] += sdir;

		tan2[idx1] += tdir;
		tan2[idx2] += tdir;
		tan2[idx3] += tdir;
	}
	for (int i = 0; i < vertices.size(); i++)
	{
		const float3& n = vertices[i].Normal;
		const float3& t = tan1[i];

		// Gram-Schmidt orthogonalize
		vertices[i].Tangent = normalize(t - n * dot(n, t));

		// Calculate handedness
		vertices[i].Tangent[i] *= (dot(cross(n, t), tan2[i]) < 0.0) ? -1.0 : 1.0;
	}

	mNumVerts = vertices.size();
	mNumIndices = indices.size();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * mNumVerts;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices.data();
	D3DCall(theFramework.GetDevice()->CreateBuffer(&bd, &InitData, &mVertexBuffer));

	mStride = sizeof(SimpleVertex);
	mOffset = 0;

	// Create index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * mNumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices.data();
	D3DCall(theFramework.GetDevice()->CreateBuffer(&bd, &InitData, &mIndexBuffer));

	assert(mVertexBuffer != nullptr);
	assert(mIndexBuffer != nullptr);
}


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

