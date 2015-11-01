#include "TerrainTile.h"


void TerrainTile::Init(float3 inPosition, float3 inScale, int2 inNumSegments, pMaterial inMaterial)
{
	CleanUp();

	mHeightMapRenderTarget = MAKE_NEW(RenderTarget);
	mHeightMapRenderTarget->Init(inNumSegments.x, inNumSegments.y, 1, 41); // 41 = DXGI_FORMAT_R32_FLOAT
	mHeightMapTexture = mHeightMapRenderTarget->GetTexture();

	mHeightScale = inScale.z;
	mPixelsPerMeter = float2(mHeightMapTexture->GetWidth(), mHeightMapTexture->GetHeight()) / inScale.XY();
	mNumSegments = inNumSegments;
	
	pMesh mesh = MAKE_NEW(Mesh);
	mesh->InitPlane(mNumSegments, inScale.XY());
	
	DrawableObject::Init(mesh, inMaterial);
	Translate(inPosition);

	mConstantBuffer = MAKE_NEW(ConstantBuffer);
	mConstantBuffer->Init(sizeof(mConstantBufferData));
	
	mConstantBufferData.widthSegments = mNumSegments.x;
	mConstantBufferData.heightSegments = mNumSegments.y;
	mConstantBufferData.heightScale = mHeightScale;
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


void TerrainTile::PrepareToDraw()
{
	theRenderContext.VSSetTextureAndSampler(GetMaterial()->GetDiffuseTexture(), Gaag.GetPointSampler(), 0);
	// TODO: set constant buffer
	// set heightmap texture
}