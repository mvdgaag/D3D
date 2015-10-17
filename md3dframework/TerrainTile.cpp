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

	mConstantBufferData.MVP = DirectX::XMMatrixScaling(inScale.x, inScale.y, inScale.z);
	mConstantBufferData.MVP *= DirectX::XMMatrixTranslation(inPosition.x, inPosition.y, inPosition.z);
	mConstantBufferData.width = inWidth;
	mConstantBufferData.height = inHeight;
	theRenderContext.UpdateSubResource(mConstantBuffer, &mConstantBufferData);

	mHeightMap = inHeightMap;
	
	mMesh = new Mesh();
	mMesh->InitPlane(inWidth, inHeight);
	
	mMaterial = inMaterial;

	mConstantBuffer = new ConstantBuffer();
	mConstantBuffer->Init(sizeof(mConstantBufferData));

	mInitialized = true; 
}


void TerrainTile::CleanUp()
{
	delete mMesh;
	mMesh = nullptr;

	delete mMaterial;
	mMaterial = nullptr;

	delete mHeightMap;
	mHeightMap = nullptr;

	delete mConstantBuffer;
	mConstantBuffer = nullptr;
	
	mInitialized = false;
}


void TerrainTile::Draw()
{
	assert(mInitialized);

	theFramework.SetMaterial(mMaterial);
	theRenderContext.VSSetTextureAndSampler(mHeightMap, theFramework.GetPointSampler(), 0);
	theRenderContext.VSSetConstantBuffer(mConstantBuffer, 0);
	theRenderContext.DrawMesh(mMesh);
}