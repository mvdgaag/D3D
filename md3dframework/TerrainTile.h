#pragma once
#include "DrawableObject.h"

class Mesh;
class Material;
class Texture;
class ConstantBuffer;


class TerrainTile : public DrawableObject
{
public:
	TerrainTile() : DrawableObject() { mInitialized = false; }
	~TerrainTile() {};

	void Init(float3 inPosition, float3 inScale, Texture* inHeightMap, int inWidthSegments, int inHeightSegments, Material* inMaterial);
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
	ConstantBuffer*		mConstantBuffer		= nullptr;
	Texture*			mHeightMap			= nullptr;
	bool				mInitialized		= false;
};

