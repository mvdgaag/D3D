#pragma once
#include "Gaag.h"

REGISTERCLASS(Terrain);
REGISTERCLASS(TerrainTile);

class TerrainTile : public MeshObject
{
public:
	TerrainTile() : MeshObject() { mInitialized = false; }
	~TerrainTile() {};

	void Init(pTerrain inTerrain, int2 inIndex, pMaterial inMaterial, pMaterial inShadowMaterial);
	void CleanUp();
	void PrepareToDraw() override;
	void FinalizeAfterDraw() override;
	void PrepareToDrawShadow() override;
	void FinalizeAfterDrawShadow() override;

private:
	struct ConstantBufferData
	{
		float4	scale;
		int4	textureInfo;
	};

	pTerrain			mTerrain;
	int2				mIndex;

	ConstantBufferData	mConstantBufferData;
	pConstantBuffer		mConstantBuffer			= nullptr;

	pMaterial			mCustomShadowMaterial	= nullptr;
	bool				mInitialized			= false;
};

