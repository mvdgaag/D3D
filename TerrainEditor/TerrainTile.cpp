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
	
	pMesh mesh = MAKE_NEW(Mesh);
	mesh->InitPlane(mNumSegments, float2(inScale));
	
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