#include "TerrainTile.h"


void TerrainTile::Init(float3 inPosition, float3 inScale, int2 inNumSegments, pMaterial inMaterial, pTexture inHeightTexture)
{
	CleanUp();

	mNumSegments = inNumSegments;

	mHeightMapRenderTarget = MAKE_NEW(RenderTarget);
	mHeightMapRenderTarget->Init(inHeightTexture);
	mHeightMapTexture = mHeightMapRenderTarget->GetTexture();

	mHeightScale = inScale.z;
	mPixelsPerMeter = float2(mHeightMapTexture->GetWidth(), mHeightMapTexture->GetHeight()) / float2(inScale);
	
	std::vector<SimpleVertex> vertices;
	std::vector<WORD> indices;

	SimpleVertex vert;
	vert.Normal = float3(0, 1, 0);
	vert.Tangent = float3(0, 0, 1);

	// generate plane
	for (int y = 0; y <= inNumSegments.y; y++)
	{
		float v = float(y) / float(inNumSegments.y);
		for (int x = 0; x <= inNumSegments.x; x++)
		{
			float u = float(x) / float(inNumSegments.x);
			vert.TexCoord = float2(u, v);
			vert.Position = float3((u - 0.5) * inScale.x, 0, (v - 0.5) * inScale.y);
			vertices.push_back(vert);
		}
	}

	int row_offset = inNumSegments.x + 1;
	for (int y = 0; y < inNumSegments.y; y++)
	{
		for (int x = 0; x < inNumSegments.x; x++)
		{
			int idx = y * row_offset + x;
			indices.push_back(idx);
			indices.push_back(idx + row_offset);
			indices.push_back(idx + 1);
			indices.push_back(idx + 1);
			indices.push_back(idx + row_offset);
			indices.push_back(idx + row_offset + 1);
		}
	}

	pMesh mesh = MAKE_NEW(Mesh);
	mesh->InitFromData(vertices.data(), vertices.size(), indices.data(), indices.size());
	
	DrawableObject::Init(mesh, inMaterial);
	Translate(inPosition);

	mConstantBuffer = MAKE_NEW(ConstantBuffer);
	mConstantBuffer->Init(sizeof(mConstantBufferData));
	
	mConstantBufferData.scale = float4(inScale, 0);
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);

	mInitialized = true; 
}


void TerrainTile::CleanUp()
{
	DrawableObject::CleanUp();
	mHeightMapTexture = nullptr;
	mHeightMapRenderTarget = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}


void TerrainTile::SetTexture(pTexture inTexture)
{

}


void TerrainTile::PrepareToDraw()
{
	theRenderContext.VSSetConstantBuffer(mConstantBuffer, 2);
	theRenderContext.VSSetTextureAndSampler(mHeightMapTexture, Gaag.GetPointSampler(), 0);
}


void TerrainTile::FinalizeAfterDraw()
{
	theRenderContext.VSSetConstantBuffer(NULL, 2);
	theRenderContext.VSSetTextureAndSampler(NULL, NULL, 0);
}