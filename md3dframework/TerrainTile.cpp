#include "TerrainTile.h"
#include "RenderContext.h"
#include "Framework.h"
#include "Texture.h"
#include "Sampler.h"
#include "Mesh.h"
#include "Material.h"
#include "ConstantBuffer.h"


void TerrainTile::Init(float3 inPosition, float3 inScale, pTexture inHeightMap, int inWidthSegments, int inHeightSegments, pMaterial inMaterial)
{
	CleanUp();

	mHeightMap = inHeightMap;
	
	pMesh mesh = std::make_shared<Mesh>();
	mesh->InitPlane(inWidthSegments, inHeightSegments, inScale.XY());
	
	DrawableObject::Init(mesh, inMaterial);
	Translate(inPosition);

	mConstantBuffer = std::make_shared<ConstantBuffer>();
	mConstantBuffer->Init(sizeof(mConstantBufferData));
	
	mConstantBufferData.widthSegments = inWidthSegments;
	mConstantBufferData.heightSegments = inHeightSegments;
	mConstantBufferData.heightScale = inScale.z;
	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);

	mInitialized = true; 
}


void TerrainTile::CleanUp()
{
	DrawableObject::CleanUp();
	mHeightMap = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}
