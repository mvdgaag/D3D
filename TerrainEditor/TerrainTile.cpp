#include "TerrainTile.h"
#include "Terrain.h"


void TerrainTile::Init(pTerrain inTerrain, int2 inIndex, pMaterial inMaterial, pMaterial inShadowMaterial)
{
	CleanUp();

	assert(inTerrain != nullptr);
	assert(inIndex.x >= 0 && inIndex.y >= 0 && inIndex.x < inTerrain->GetNumTiles().x && inIndex.y < inTerrain->GetNumTiles().y);

	mTerrain = inTerrain;
	mIndex = inIndex;

	int2 resolution = inTerrain->GetTileResolution();
	float3 scale = inTerrain->GetTileScale();
	
	std::vector<SimpleVertex> vertices;
	std::vector<WORD> indices;

	SimpleVertex vert;
	vert.Normal = float3(0, 1, 0);
	vert.Tangent = float3(0, 0, 1);

	// generate plane
	for (int y = 0; y < resolution.y; y++)
	{
		float v = float(y) / float(resolution.y - 1);
		for (int x = 0; x < resolution.x; x++)
		{
			float u = float(x) / float(resolution.x - 1);
			// DEVHACK: use actual pixel coordinates for uvs
			//vert.TexCoord = float2(u, v);
			vert.TexCoord = float2(x, y); 
			vert.Position = float3((u - 0.5) * scale.x, 0, (v - 0.5) * scale.y);
			vertices.push_back(vert);
		}
	}

	for (int y = 0; y < resolution.y - 1; y++)
	{
		for (int x = 0; x < resolution.x - 1; x++)
		{
			int idx = y * resolution.x + x;
			indices.push_back(idx);
			indices.push_back(idx + resolution.x);
			indices.push_back(idx + 1);
			indices.push_back(idx + 1);
			indices.push_back(idx + resolution.x);
			indices.push_back(idx + resolution.x + 1);
		}
	}

	pMesh mesh = theResourceFactory.MakeMesh();
	mesh->InitFromData(vertices.data(), vertices.size(), indices.data(), indices.size());
	
	MeshObject::Init(mesh, inMaterial);

	float3 tile_pos;
	tile_pos.x = (float(inIndex.x) - float(inTerrain->GetNumTiles().x - 1) / 2.0) * scale.x;
	tile_pos.y = 0.0;
	tile_pos.z = (float(inIndex.y) - float(inTerrain->GetNumTiles().y - 1) / 2.0) * scale.y;
	Translate(tile_pos);

	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(mConstantBufferData));
	mConstantBufferData.scale = float4(scale, 0.0);
	mConstantBufferData.textureInfo = int4(resolution, 0, 0);
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
	
	mCustomShadowMaterial = inShadowMaterial;

	mInitialized = true; 
}


void TerrainTile::CleanUp()
{
	MeshObject::CleanUp();
	mConstantBuffer = nullptr;
	mInitialized = false;
}


void TerrainTile::PrepareToDraw()
{
	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();
	theRenderContext.VSSetConstantBuffer(mConstantBuffer, 2);
	theRenderContext.VSSetTextureAndSampler(mTerrain->GetLayerTexture(mIndex, mTerrain->GetHeightLayerIndex()), point_sampler, 0);
	theRenderContext.VSSetTextureAndSampler(mTerrain->GetLayerTexture(mIndex, mTerrain->GetNormalLayerIndex()), point_sampler, 1);
}


void TerrainTile::FinalizeAfterDraw()
{
	theRenderContext.VSSetConstantBuffer(NULL, 2);
	theRenderContext.VSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.VSSetTextureAndSampler(NULL, NULL, 1);
}


void TerrainTile::PrepareToDrawShadow()
{
	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();
	theRenderContext.VSSetShader(mCustomShadowMaterial->GetVertexShader());
	theRenderContext.PSSetShader(mCustomShadowMaterial->GetPixelShader());
	theRenderContext.VSSetConstantBuffer(mConstantBuffer, 1);
	theRenderContext.VSSetTextureAndSampler(mTerrain->GetLayerTexture(mIndex, mTerrain->GetHeightLayerIndex()), point_sampler, 0);
}


void TerrainTile::FinalizeAfterDrawShadow()
{
	theRenderContext.VSSetConstantBuffer(NULL, 0);
	theRenderContext.VSSetTextureAndSampler(NULL, NULL, 0);
}