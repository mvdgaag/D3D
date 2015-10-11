#include "TerrainTile.h"
#include "Texture.h"
#include "Sampler.h"
#include "Mesh.h"
#include "PixelShader.h"
#include "VertexShader.h"


void TerrainTile::Init(float3 inScale, Texture* inHeightMap, int inWidth, int inHeight,
	PixelShader* inPixelShader, VertexShader* inVertexShader,
	Texture* inDiffuseTexture, Sampler* inDiffuseSampler,
	Texture* inNormalTexture, Sampler* inNormalSampler,
	Texture* inMaterialTexture, Sampler* inMaterialSampler)
{
	CleanUp();

	mScale = inScale;
	mWidth = inWidth;
	mHeight = inHeight;
	mHeightMap = inHeightMap;
	
	mMesh = new Mesh();
	mMesh->InitPlane(inWidth, mHeight, mScale.XY());
	
	mPixelShader = inPixelShader;
	mVertexShader = inVertexShader;

	mDiffuseTexture = inDiffuseTexture;
	mDiffuseSampler = inDiffuseSampler;
	mNormalTexture = inNormalTexture;
	mNormalSampler = inNormalSampler;
	mMaterialTexture = inMaterialTexture;
	mMaterialSampler = inMaterialSampler;

	mInitialized = true; 
}


void TerrainTile::CleanUp()
{
	delete mMesh;
	delete mPixelShader;
	delete mVertexShader;
	delete mHeightMap;
	delete mDiffuseTexture;
	delete mDiffuseSampler;
	delete mNormalTexture;
	delete mNormalSampler;
	delete mMaterialTexture;
	delete mMaterialSampler;
	mInitialized = false;
}