#pragma once
#include "LinearAlgebra.h"

// predeclarations
class ConstantBuffer;
class ComputeShader;
class RenderTarget;
class Texture;


class TAARenderer
{
public:
	TAARenderer() { mInitialized = false; }
	~TAARenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(Texture* inSource, RenderTarget* inHistory, Texture* inMotionVectors, RenderTarget* inTarget);
	
	static float2 GetJitterOffset(int inFrameID);

private:
	struct ConstantBufferData
	{
		float2 mJitterOffset;
		float2 mTargetSize;
	};
	ConstantBufferData mConstantBufferData;
	ConstantBuffer* mConstantBuffer;
	ComputeShader* mShader = nullptr;
	bool mInitialized = false;
};