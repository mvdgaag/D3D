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
	TerrainTile() : BaseDrawable() { mInitialized = false; }
	~TerrainTile() {};

	void Init(float3 inScale, Texture* inHeightMap, int inWidth, int inHeight,
		PixelShader* inPixelShader, VertexShader* inVertexShader,
		Texture* inDiffuseTexture, Sampler* inDiffuseSampler,
		Texture* inNormalTexture, Sampler* inNormalSampler,
		Texture* inMaterialTexture, Sampler* inMaterialSampler);
	void CleanUp();
	void Draw() {}

private:
	PixelShader* mPixelShader;
	VertexShader* mVertexShader;
	Texture* mDiffuseTexture;
	Sampler* mDiffuseSampler;
	Texture* mNormalTexture;
	Sampler* mNormalSampler;
	Texture* mMaterialTexture;
	Sampler* mMaterialSampler;
	Mesh* mMesh;
	int mWidth, mHeight;
	float3 mScale = { 0.0f, 0.0f, 0.0f };
	Texture* mHeightMap = nullptr;
	bool mInitialized = false;
};

