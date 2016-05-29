#include "SkyLightRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "TextureUtil.h"


void SkyLightRenderer::Render(pTexture inNormal, pTexture inLinearDepth, pTexture inDiffuse, pRenderTarget inTarget)
{
	assert(mInitialized == true);
	assert(inNormal != nullptr);
	assert(inLinearDepth != nullptr);
	assert(inDiffuse != nullptr);
	assert(inTarget != nullptr);

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetTextureAndSampler(inNormal, point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inLinearDepth, point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inDiffuse, point_sampler, 2);
	theRenderContext.CSSetRWTexture(inTarget, 0, 0);

	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = 1.0 / cam->GetProjectionMatrix()[0][0];
	mConstantBufferData.viewspaceReconstructionVector.y = 1.0 / cam->GetProjectionMatrix()[1][1];
	mConstantBufferData.targetSize = inTarget->GetDimensions();
	mConstantBufferData.frameData = float4(Gaag.WorldToCameraNormal(float3(0.0, 1.0, 0.0)), Gaag.GetRandom());

	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	int2 groups = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetConstantBuffer(NULL, 0);
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
}


void SkyLightRenderer::Init()
{
	assert(mInitialized == false);
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/SkyLightCompute.hlsl");
	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferData));
	mInitialized = true;
}


void SkyLightRenderer::CleanUp()
{
	theResourceFactory.DestroyItem(mShader);
	theResourceFactory.DestroyItem(mConstantBuffer);
	mShader = nullptr;
	mConstantBuffer = nullptr;
	mInitialized = false;
}
