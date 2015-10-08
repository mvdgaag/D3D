#pragma once
#include <d3d11_1.h>
#include "float4.h"

// predeclarations
class ComputeShader;
class RenderTarget;
class GBuffer;
class ConstantBuffer;



class DirectLightingRenderer
{
public:
	DirectLightingRenderer() { mInitialized = false; }
	~DirectLightingRenderer() { CleanUp(); }

	void Init();
	void CleanUp();
	void Render(GBuffer* inSource, RenderTarget* inTarget);

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

