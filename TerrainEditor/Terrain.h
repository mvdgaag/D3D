#pragma once
#include "Gaag.h"
#include "TerrainTile.h"
#include "Layer.h"


REGISTERCLASS(Terrain);

class Terrain
{
public:
	Terrain() {};
	~Terrain() {};
	void Init(int2 inNumTiles, int2 inTileSegments, float3 inTileScale, pMaterial inMaterial, pMaterial inShadowMaterial);
	void CleanUp();

	int2			GetNumTiles()				{ return mNumTiles; }
	int2			GetTileSegments()			{ return mTileSegments; }
	float3			GetTileScale()				{ return mTileScale; }
	

	pLayer GetLayer(int inIndex);
	void SetLayer(pLayer inLayer, int inIndex);	
	float2 WorldToTileSpace(float2 inWorldCoord);
	pTerrainTile GetTile(const int2& inTileIndex);
	pRenderTarget GetLayerRenderTarget(const int2& inTileIndex, const int inLayerID);
	pTexture GetLayerTexture(const int2& inTileIndex, const int inLayerID);
	pTerrainTile GetTile(const float2& inWorldCoord);
	pRenderTarget GetLayerRenderTarget(const float2& inWorldCoord, const int inLayerID);
	pTexture GetLayerTexture(const float2& inWorldCoord, const int inLayerID);
	std::vector<int2> GetTiles(const rect& inWorldRect);


private:
	int2 mNumTiles;
	int2 mTileSegments;
	float3 mTileScale;
	pTerrainTile* mTiles = nullptr;
	apLayer mLayers;
};

