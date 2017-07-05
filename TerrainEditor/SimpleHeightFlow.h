#pragma once
#include "Gaag.h"

REGISTERCLASS(SimpleHeightFlow);
REGISTERCLASS(Terrain);
REGISTERCLASS(Layer);


class SimpleHeightFlow
{
public:
	SimpleHeightFlow() { mInitialized = false; };
	~SimpleHeightFlow() { CleanUp(); }

	void Init(pTerrain inTerrain);
	void CleanUp();
	void Update(float inTimeStep);

	pTerrain GetTerrain()		{ return mTerrain; }
	
	pLayer GetLayer()			{ return mLayer; }
	pLayer GetHistoryLayer()	{ return mHistoryLayer; }

	int GetLayerIndex()			{ return mLayerIndex; }
	int GetHistoryLayerIndex()	{ return mHistoryLayerIndex; }


private:
	pTerrain mTerrain;
	pLayer mLayer;
	pLayer mHistoryLayer;
	int mLayerIndex;
	int mHistoryLayerIndex;

	struct ConstantBuffertData
	{
		float4 params; // time_scale, friction, talus slope, 0
		float4 resolution; // width, height, 1/width, 1/height
		float4 scale; // pixels/m x, pixels/m y, heightscale, 0
	};

	ConstantBuffertData mConstantBufferData;
	
	float mFluidity;
	float mFriction;
	float mTalusSlope;
	int2 mResolution;
	float3 mScale;
	bool mInitialized;

	pConstantBuffer	mConstantBuffer;
	pComputeShader	mUpdateShader;
	pComputeShader	mUpdateBorderShader;
};

