#include "DirectLightingRenderer.h"
#include "FrameWork.h"
#include "ComputeShader.h"
#include "GBuffer.h"
#include "RenderTarget.h"
#include "Texture.h"
#include <assert.h>


void DirectLightingRenderer::Render(GBuffer* inSource, RenderTarget* inTarget)
{
	assert(mInitialized == true);

	assert(inSource != nullptr);
	assert(inTarget != nullptr);

	ID3D11DeviceContext* context;
	theFramework.GetDevice()->GetImmediateContext(&context);

	// We now set up the shader and run it
	context->CSSetShader(mShader->GetHandle(), NULL, 0);
	ID3D11ShaderResourceView* source_views[] = { inSource->GetTexture(GBuffer::LINEAR_DEPTH)->GetShaderResourceView(),
		inSource->GetTexture(GBuffer::NORMAL)->GetShaderResourceView(),
		inSource->GetTexture(GBuffer::DIFFUSE)->GetShaderResourceView(),
		inSource->GetTexture(GBuffer::MATERIAL)->GetShaderResourceView() };
	context->CSSetShaderResources(0, 4, source_views);
	
	ID3D11UnorderedAccessView* targets[] = { inTarget->GetUnorderedAccessView() };
	unsigned int initial_counts[] = { -1 };
	context->CSSetUnorderedAccessViews(0, 1, targets, initial_counts);

	int groups_x = 1 + (inTarget->GetTexture()->GetWidth() - 1) / 8;
	int groups_y = 1 + (inTarget->GetTexture()->GetHeight() - 1) / 8;
	context->Dispatch(groups_x, groups_y, 1);

	// TODO: required?
	context->Flush();

	// clear state
	ID3D11UnorderedAccessView* uav_null[] = { NULL };
	ID3D11ShaderResourceView* srv_null[] = { NULL, NULL, NULL, NULL };
	context->CSSetShaderResources(0, 4, srv_null);
	context->CSSetUnorderedAccessViews(0, 1, uav_null, NULL);
	context->CSSetShader(NULL, NULL, 0);
}


void DirectLightingRenderer::Init()
{
	CleanUp();
	mShader = new ComputeShader("DirectLightingCompute");
	mShader->InitFromFile("Shaders/DirectLightingCompute.hlsl");
	mInitialized = true;
}


void DirectLightingRenderer::CleanUp()
{
	if (mShader)
	{
		delete mShader;
		mShader = NULL;
	}
	mInitialized = false;
}