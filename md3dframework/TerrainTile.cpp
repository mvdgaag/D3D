#include "TerrainTile.h"
#include "Texture.h"
#include "Sampler.h"
#include "Mesh.h"
#include "PixelShader.h"
#include "VertexShader.h"


void TerrainTile::Init(float3 inScale, Texture* inHeightMap, 
	Mesh* inMesh, PixelShader* inPixelShader, VertexShader* inVertexShader,
	Texture* inDiffuseTexture, Sampler* inDiffuseSampler,
	Texture* inNormalTexture, Sampler* inNormalSampler,
	Texture* inMaterialTexture, Sampler* inMaterialSampler)
{


	// TODO
	mScale = inScale;
	mHeightMap = inHeightMap;
	mInitialized = true;
}
