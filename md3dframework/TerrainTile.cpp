#include "TerrainTile.h"
#include "RenderContext.h"
#include "Framework.h"
#include "Texture.h"
#include "Sampler.h"
#include "Mesh.h"
#include "Material.h"
#include "ConstantBuffer.h"


void TerrainTile::Init(float3 inPosition, float3 inScale, Texture* inHeightMap, int inWidthSegments, int inHeightSegments, Material* inMaterial)
{
	CleanUp();

	mHeightMap = inHeightMap;
	
	Mesh* mesh = new Mesh();
	mesh->InitPlane(inWidthSegments, inHeightSegments, inScale.XY());
	
	DrawableObject::Init(mesh, inMaterial);

	mConstantBuffer = new ConstantBuffer();
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

	delete mHeightMap;
	mHeightMap = nullptr;

	delete mConstantBuffer;
	mConstantBuffer = nullptr;
	
	mInitialized = false;
}
