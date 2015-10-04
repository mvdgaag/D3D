#include "DepthPyramidRenderer.h"
#include "FrameWork.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "LinearAlgebra.h"
#include <assert.h>


void DepthPyramidRenderer::Render(Texture* inSource, RenderTarget* inTarget)
{
	assert(mInitialized);
	assert(inSource != nullptr);
	assert(inTarget != nullptr);
	assert(inTarget->GetTexture()->GetMipLevels() == kNumMipLevels);

	ID3D11DeviceContext* context;
	theFramework.GetDevice()->GetImmediateContext(&context);

	// We now set up the shader and run it
	context->CSSetShader(mShader->GetHandle(), NULL, 0);
	ID3D11ShaderResourceView* source_view = inSource->GetShaderResourceView();
	context->CSSetShaderResources(0, 1, &source_view);
	ID3D11UnorderedAccessView* targets[] = {	inTarget->GetUnorderedAccessView(0),
												inTarget->GetUnorderedAccessView(1),
												inTarget->GetUnorderedAccessView(2) };
	unsigned int initial_counts[] = { -1 };
	context->CSSetUnorderedAccessViews(0, 3, targets, initial_counts);

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	context->Dispatch(groups_x, groups_y, 1);
	
	// TODO: required?
	context->Flush();

	// clear state
	ID3D11UnorderedAccessView* uav_null[] = { NULL, NULL, NULL };
	ID3D11ShaderResourceView* srv_null = NULL;
	context->CSSetShaderResources(0, 1, &srv_null);
	context->CSSetUnorderedAccessViews(0, 3, uav_null, NULL);
	context->CSSetShader(NULL, NULL, 0);
}


void DepthPyramidRenderer::Init(int inSourceWidth, int inSourceHeight)
{
	mShader = new ComputeShader("DepthPyramidShader");
	mShader->InitFromFile("Shaders/DepthPyramidCompute.hlsl");
	assert(mShader != nullptr);
	mInitialized = true;
}


void DepthPyramidRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = nullptr;
	}
	mInitialized = false;
}

