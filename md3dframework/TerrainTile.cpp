#include "TerrainTile.h"
#include "RenderContext.h"
#include "Framework.h"
#include "Texture.h"
#include "Sampler.h"
#include "Mesh.h"
#include "Material.h"
#include "ConstantBuffer.h"


void TerrainTile::Init(float3 inPosition, float3 inScale, Texture* inHeightMap, int inWidth, int inHeight, Material* inMaterial)
{
	CleanUp();

	mHeightMap = inHeightMap;
	
	Mesh* mesh = new Mesh();
	mesh->InitPlane(inWidth, inHeight);
	
	DrawableObject::Init(mesh, inMaterial);

	mConstantBuffer = new ConstantBuffer();
	mConstantBuffer->Init(sizeof(mConstantBufferData));
	
	mConstantBufferData.MVP = DirectX::XMMatrixScaling(inScale.x, inScale.y, inScale.z);
	mConstantBufferData.MVP *= DirectX::XMMatrixTranslation(inPosition.x, inPosition.y, inPosition.z);
	mConstantBufferData.width = inWidth;
	mConstantBufferData.height = inHeight;
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
