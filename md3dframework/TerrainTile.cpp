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
	mScale = inScale;
	mWidth = inWidth;
	mHeight = inHeight;
	mHeightMap = inHeightMap;
	
	mMesh = new Mesh();
	mMesh->InitPlane(inWidth, mHeight, mScale.XY());
	
	mPixelShader = inPixelShader;

	mDiffuseTexture = inDiffuseTexture;
	mDiffuseSampler = inDiffuseSampler;
	mNormalTexture = inNormalTexture;
	mNormalSampler = inNormalSampler;
	mMaterialTexture = inMaterialTexture;
	mMaterialSampler = inMaterialSampler;

	mInitialized = true; 
}
