#pragma once
#include "DrawableObject.h"

class Texture;
class Sampler;
class PixelShader;
class VertexShader;
class Mesh;


class TerrainTile : public BaseDrawable
{
public:
	TerrainTile(std::string inName) : BaseDrawable(inName) {};
	~TerrainTile() {};

	void Init(float3 inScale, Texture* inHeightMap,
		Mesh* inMesh, PixelShader* inPixelShader, VertexShader* inVertexShader,
		Texture* inDiffuseTexture, Sampler* inDiffuseSampler,
		Texture* inNormalTexture, Sampler* inNormalSampler,
		Texture* inMaterialTexture, Sampler* inMaterialSampler);
private:
	float3 mScale = { 0.0f, 0.0f, 0.0f };
	Texture* mHeightMap = nullptr;
	bool mInitialized = false;
};

