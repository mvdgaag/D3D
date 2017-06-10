#include "TerrainTile.h"


// declare static variables
pComputeShader TerrainTile::sUpdateNormalShader;


void TerrainTile::Init(float3 inPosition, float3 inScale, int2 inNumSegments, pMaterial inMaterial, pMaterial inShadowMaterial, pTexture inHeightTexture, pTexture inNormalTexture)
{
	CleanUp();

	mNumSegments = inNumSegments;

	mHeightMapTexture = inHeightTexture;
	mHeightMapRenderTarget = theResourceFactory.MakeRenderTarget(mHeightMapTexture);
	
	mNormalTexture = inNormalTexture;
	mNormalRenderTarget = theResourceFactory.MakeRenderTarget(mNormalTexture);

	if (sUpdateNormalShader == nullptr)
		sUpdateNormalShader = theResourceFactory.LoadComputeShader("Shaders/TerrainUpdateNormalShader.hlsl");
	
	apTexture dummy_heights;
	dummy_heights.resize(4);
	UpdateNormals(dummy_heights);

	mHeightScale = inScale.z;
	mPixelsPerMeter = float2(mHeightMapTexture->GetDimensions()) / float2(inScale);
	
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

	pMesh mesh = theResourceFactory.MakeMesh();
	mesh->InitFromData(vertices.data(), vertices.size(), indices.data(), indices.size());
	
	MeshObject::Init(mesh, inMaterial);
	Translate(inPosition);

	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(mConstantBufferData));
	mConstantBufferData.scale = float4(inScale, 0.0);
	mConstantBufferData.textureInfo = int4(mHeightMapTexture->GetDimensions(), 0, 0);
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);

	mCustomShadowMaterial = inShadowMaterial;

	mInitialized = true; 
}


void TerrainTile::CleanUp()
{
	MeshObject::CleanUp();
	mHeightMapTexture = nullptr;
	mHeightMapRenderTarget = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}


void TerrainTile::UpdateNormals(apTexture inNeighborHeights)
{
	assert(inNeighborHeights.size() == 4);

	theRenderContext.CSSetShader(sUpdateNormalShader);
	theRenderContext.CSSetRWTexture(mNormalRenderTarget, 0, 0);
	theRenderContext.CSSetTexture(mHeightMapTexture, 0);

	theRenderContext.CSSetTexture(inNeighborHeights[0], 1); // n
	theRenderContext.CSSetTexture(inNeighborHeights[1], 2); // e
	theRenderContext.CSSetTexture(inNeighborHeights[2], 3); // s
	theRenderContext.CSSetTexture(inNeighborHeights[3], 4); // w
	
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);
	int2 num_threads = (mNormalRenderTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(num_threads.x, num_threads.y, 1);
	
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
	theRenderContext.CSSetTexture(NULL, 0);
	theRenderContext.CSSetTexture(NULL, 1);
	theRenderContext.CSSetTexture(NULL, 2);
	theRenderContext.CSSetTexture(NULL, 3);
	theRenderContext.CSSetTexture(NULL, 4);
	theRenderContext.CSSetShader(NULL);
}


void TerrainTile::PrepareToDraw()
{
	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();
	theRenderContext.VSSetConstantBuffer(mConstantBuffer, 2);
	theRenderContext.VSSetTextureAndSampler(mHeightMapTexture, point_sampler, 0);
	theRenderContext.VSSetTextureAndSampler(mNormalTexture, point_sampler, 1);
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
	theRenderContext.VSSetTextureAndSampler(mHeightMapTexture, point_sampler, 0);
}


void TerrainTile::FinalizeAfterDrawShadow()
{
	theRenderContext.VSSetConstantBuffer(NULL, 0);
	theRenderContext.VSSetTextureAndSampler(NULL, NULL, 0);
}