#pragma once
#include "LinearAlgebra.h"


// predeclarations
class ComputeShader;
class RenderTarget;
class Texture;
class ConstantBuffer;


class PostProcessRenderer
{
public:
	PostProcessRenderer() { mInitialized = false; }
	~PostProcessRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(Texture* inSource, Texture* inMotionVectors, RenderTarget* inTarget);

private:
	struct ConstantBufferData
	{
		float4 mTargetSize;
	};
	ConstantBufferData mConstantBufferData;

	ConstantBuffer* mConstantBuffer = nullptr;
	ComputeShader* mShader = nullptr;
	bool mInitialized = false;
};