#pragma once
#include "LinearAlgebra.h"


// predeclarations
class ComputeShader;
class RenderTarget;
class Texture;
class ConstantBuffer;


class IndirectLightingRenderer
{
public:
	IndirectLightingRenderer() { mInitialized = false; }
	~IndirectLightingRenderer() { CleanUp(); }
	
	void Init();
	void CleanUp();
	void Render(Texture* inSource, Texture* inNormal, Texture* inLinearDepth, RenderTarget* inTarget);

private:
	struct ConstantBufferData
	{
		float2 viewspaceReconstructionVector;
		float2 targetSize;
		float4 frameData;
	};
	ConstantBufferData mConstantBufferData;
	ConstantBuffer* mConstantBuffer = nullptr;
	ComputeShader* mShader = nullptr;
	bool mInitialized = false;
};