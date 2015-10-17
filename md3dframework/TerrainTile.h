#pragma once
#include "DrawableObject.h"

class Mesh;
class Material;
class Texture;
class ConstantBuffer;


class TerrainTile : public BaseDrawable
{
public:
	TerrainTile() : BaseDrawable() { mInitialized = false; }
	~TerrainTile() {};

	void Init(float3 inPosition, float3 inScale, Texture* inHeightMap, int inWidth, int inHeight, Material* inMaterial);
	void CleanUp();
	void Draw();

private:
	struct ConstantBufferData
	{
		DirectX::XMMATRIX MVP;
		unsigned int width;
		unsigned int height;
	};

	ConstantBufferData	mConstantBufferData;
	ConstantBuffer*		mConstantBuffer		= nullptr;
	Mesh*				mMesh				= nullptr;
	Material*			mMaterial			= nullptr;
	Texture*			mHeightMap			= nullptr;
	bool				mInitialized		= false;
};

