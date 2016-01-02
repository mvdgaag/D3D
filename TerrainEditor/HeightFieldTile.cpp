#include "HeightFieldTile.h"


void HeightFieldTile::Init(float3 inPosition, float3 inScale, int2 inNumSegments, pMaterial inMaterial, pTexture inHeightTexture)
{
	CleanUp();

	mNumSegments = inNumSegments;

	mHeightMapRenderTarget = theResourceFactory.MakeRenderTarget(inHeightTexture);
	mHeightMapTexture = mHeightMapRenderTarget->GetTexture();
	
	mNormalTexture = theResourceFactory.MakeTexture(mHeightMapTexture->GetDimensions(),	1, Format::FORMAT_R8G8B8A8_SNORM, BindFlag::BIND_COMPUTE_TARGET);
	mNormalRenderTarget = theResourceFactory.MakeRenderTarget(mNormalTexture);

	// TODO:
	//mUpdateNormalShader = theResourceFactory.Get("HeightFieldUpdateNormalShader");
	//UpdateNormals();

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
	
	DrawableObject::Init(mesh, inMaterial);
	Translate(inPosition);

	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(mConstantBufferData));
	
	mConstantBufferData.scale = float4(inScale, 0);
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);

	mInitialized = true; 
}


void HeightFieldTile::CleanUp()
{
	DrawableObject::CleanUp();
	mHeightMapTexture = nullptr;
	mHeightMapRenderTarget = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}


void HeightFieldTile::UpdateNormals()
{
	theRenderContext.CSSetShader(mUpdateNormalShader);
}



void HeightFieldTile::PrepareToDraw()
{
	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();
	theRenderContext.VSSetConstantBuffer(mConstantBuffer, 2);
	theRenderContext.VSSetTextureAndSampler(mHeightMapTexture, point_sampler, 0);
}


void HeightFieldTile::FinalizeAfterDraw()
{
	theRenderContext.VSSetConstantBuffer(NULL, 2);
	theRenderContext.VSSetTextureAndSampler(NULL, NULL, 0);
}