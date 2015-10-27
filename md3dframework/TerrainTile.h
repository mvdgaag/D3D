#pragma once
#include "DrawableObject.h"

PREDEFINE(Mesh, pMesh);
PREDEFINE(Material, pMaterial);
PREDEFINE(Texture, pTexture);
PREDEFINE(ConstantBuffer, pConstantBuffer);


class TerrainTile : public DrawableObject
{
public:
	TerrainTile() : DrawableObject() { mInitialized = false; }
	~TerrainTile() {};

	void Init(float3 inPosition, float3 inScale, pTexture inHeightMap, int inWidthSegments, int inHeightSegments, pMaterial inMaterial);
	void CleanUp();
	void Draw();

private:
	struct ConstantBufferData
	{
		unsigned int widthSegments;
		unsigned int heightSegments;
		float heightScale;
		float bogus_padding;
	};

	ConstantBufferData	mConstantBufferData;
	pConstantBuffer		mConstantBuffer		= nullptr;
	pTexture			mHeightMap			= nullptr;
	bool				mInitialized		= false;
};

