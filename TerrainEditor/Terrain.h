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
	void Init(int2 inNumTiles, int2 inTileResolution, float3 inTileScale, pMaterial inMaterial, pMaterial inShadowMaterial);
	void CleanUp();

	int2		GetNumTiles()							{ return mNumTiles; }
	int2		GetTileResolution()						{ return mTileResolution; }
	float3		GetTileScale()							{ return mTileScale; }

	pLayer		GetLayer(int inIndex);
	
	int			GetHeightLayerIndex()					{ return mHeightLayerIndex; }
	void		SetHeightLayerIndex(int inIndex)		{ assert(ValidateLayerIndex(inIndex)); mHeightLayerIndex = inIndex; }
	
	int			GetNormalLayerIndex()					{ return mNormalLayerIndex; }
	void		SetNormalLayerIndex(int inIndex)		{ assert(ValidateLayerIndex(inIndex)); mHeightLayerIndex = inIndex; }
		
	int SetLayer(pLayer inLayer, int inIndex = -1);	
	int FindLayer(pLayer inLayer);
	
	void UpdateNormals(pRenderTarget inTarget, apTexture inHeights);

	void SetDirty(int2 inTileIndex, int inLayerIndex);
	void ProcessDirtyLayers();
	
	void SetLayerAsAlbedo(int inLayerIndex);
	void SetLayerAsHeight(int inLayerIndex);

	float2 WorldToTileSpace(float2 inWorldCoord);
	float2 TileToWorldSpace(float2 inTileCoord);
	
	pTerrainTile GetTile(const int2& inTileIndex);
	pTerrainTile GetTile(const float2& inWorldCoord);

	pRenderTarget GetLayerRenderTarget(const int2& inTileIndex, const int inLayerIndex);
	pRenderTarget GetLayerRenderTarget(const float2& inWorldCoord, const int inLayerIndex);

	pTexture GetLayerTexture(const int2& inTileIndex, const int inLayerIndex);
	pTexture GetLayerTexture(const float2& inWorldCoord, const int inLayerIndex);

	std::vector<int2> GetTiles(const rect& inWorldRect);

private:
	struct ConstantBufferData
	{
		float4	scale;
		int4	textureInfo;
	};
	ConstantBufferData		mConstantBufferData;
	pConstantBuffer			mConstantBuffer = nullptr;

	int2					mNumTiles;
	int2					mTileResolution;
	float3					mTileScale;
	pTerrainTile*			mTiles = nullptr;
	apLayer					mLayers;
	int						mHeightLayerIndex;
	int						mNormalLayerIndex;

	bool					ValidateLayerIndex(int inIndex)		{ return inIndex >= 0 && inIndex < mLayers.size(); }
	bool					ValidateTileIndex(int2 inIndex)		{ return inIndex.x >= 0 && inIndex.y >= 0 && inIndex.x < mNumTiles.x && inIndex.y < mNumTiles.y; }

	static pComputeShader	sUpdateNormalShader;
};

