#include "ReflectionRenderer.h"
#include "RenderContext.h"
#include "GaagFramework.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "Camera.h"


void ReflectionRenderer::Render(pTexture inSource, pRenderTarget inTarget, pTexture inNormal, pTexture inMaterial, pTexture inMinDepthPyramid, pTexture inCubemap)
{
	assert(mInitialized == true);

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();
	pSampler linear_sampler = theResourceFactory.GetDefaultLinearSampler();

	assert(inSource != nullptr);
	assert(inTarget != nullptr);
	assert(inNormal != nullptr);
	assert(inMinDepthPyramid != nullptr);
	assert(inCubemap != nullptr);

	// set general constant buffer data
	pCamera cam = Gaag.GetCamera();
	mConstantBufferData.viewspaceReconstructionVector.x = 1.0 / cam->GetProjectionMatrix()[0][0];
	mConstantBufferData.viewspaceReconstructionVector.y = 1.0 / cam->GetProjectionMatrix()[1][1];
	mConstantBufferData.targetSize = (float2)inTarget->GetTexture()->GetDimensions();
	mConstantBufferData.params = float4(Gaag.GetFrameID(), inCubemap->GetMipLevels(), 0, 0);
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantBufferData);
	theRenderContext.CSSetConstantBuffer(mConstantBuffer, 0);

	theRenderContext.CSSetShader(mShader);
	theRenderContext.CSSetRWTexture(inTarget, 0, 0);
	theRenderContext.CSSetTextureAndSampler(inSource, point_sampler, 0);
	theRenderContext.CSSetTextureAndSampler(inNormal, point_sampler, 1);
	theRenderContext.CSSetTextureAndSampler(inMaterial, point_sampler, 2);
	theRenderContext.CSSetTextureAndSampler(inMinDepthPyramid, point_sampler, 3);
	theRenderContext.CSSetTextureAndSampler(inCubemap, linear_sampler, 4);
	theRenderContext.CSSetTextureAndSampler(mBRDFLookupTarget->GetTexture(), linear_sampler, 5);

	int2 groups = (inTarget->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 1);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 1);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 2);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 3);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 4);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 5);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
}


// TODO: this does not seem to do anything!
void ReflectionRenderer::FilterCubemap(pRenderTarget inOutCubemap) const
{
	assert(mInitialized == true);

	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();
	//pSampler linear_sampler = theResourceFactory.GetDefaultLinearSampler();

	assert(inOutCubemap != nullptr);

	int max_mip = min(log2(inOutCubemap->GetDimensions().x), log2(inOutCubemap->GetDimensions().y));
	for (int i = 1; i <= max_mip; i++)
	{
		int2 dim = inOutCubemap->GetDimensions() >> i;

		// TODO: calculate based on pixel's solid angle
		float roughness = (float)i / (float)max_mip;

		// set general constant buffer data
		CubemapFilterConstantBufferData data;
		data.params = float4(float(dim.x), float(dim.y), roughness, 0.0);

		theRenderContext.UpdateSubResource(*mCubemapFilterConstantBuffer, &data);
		theRenderContext.CSSetConstantBuffer(mCubemapFilterConstantBuffer, 0);

		theRenderContext.CSSetShader(mCubemapFilterShader);
		theRenderContext.CSSetRWTexture(inOutCubemap, i, 0);
		theRenderContext.CSSetTextureAndSampler(inOutCubemap->GetTexture(), point_sampler, 0);

		int2 groups = (dim + 7) / 8;
		theRenderContext.Dispatch(groups.x, groups.y, 6);
		theRenderContext.Flush();
	}

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
	theRenderContext.CSSetTextureAndSampler(NULL, NULL, 0);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
}


void ReflectionRenderer::GenerateBRDFLookup(pRenderTarget outLookup) const
{
	pSampler point_sampler = theResourceFactory.GetDefaultPointSampler();

	assert(outLookup != nullptr);

	// set general constant buffer data
	BRDFLookupConstantBufferData data;
	data.params = float4(outLookup->GetDimensions(), 0.0, 0.0);

	theRenderContext.UpdateSubResource(*mGenerateBRDFLookupConstantBuffer, &data);
	theRenderContext.CSSetConstantBuffer(mGenerateBRDFLookupConstantBuffer, 0);

	theRenderContext.CSSetShader(mGenerateBRDFLookupShader);
	theRenderContext.CSSetRWTexture(outLookup, 0, 0);

	int2 groups = (outLookup->GetDimensions() + 7) / 8;
	theRenderContext.Dispatch(groups.x, groups.y, 6);
	theRenderContext.Flush();

	// clear state
	theRenderContext.CSSetShader(NULL);
	theRenderContext.CSSetRWTexture(NULL, 0, 0);
	theRenderContext.CSSetConstantBuffer(NULL, 0);
}


void ReflectionRenderer::Init()
{
	assert(mInitialized == false);
	
	mShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/ReflectionCompute.hlsl");
	mCubemapFilterShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/CubemapFilter.hlsl");
	mGenerateBRDFLookupShader = theResourceFactory.LoadComputeShader("../md3dFramework/Shaders/GenerateBRDFLookup.hlsl");

	mBRDFLookupTarget = theResourceFactory.MakeRenderTarget(int2(256, 256), 1, FORMAT_R16G16_FLOAT);

	mConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(ConstantBufferData));
	mCubemapFilterConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(CubemapFilterConstantBufferData));
	mGenerateBRDFLookupConstantBuffer = theResourceFactory.MakeConstantBuffer(sizeof(BRDFLookupConstantBufferData));
	
	GenerateBRDFLookup(mBRDFLookupTarget);

	mInitialized = true;
}


void ReflectionRenderer::CleanUp()
{
	theResourceFactory.DestroyItem(mShader);	
	mShader = nullptr;
	
	theResourceFactory.DestroyItem(mCubemapFilterShader);
	mCubemapFilterShader = nullptr;

	theResourceFactory.DestroyItem(mGenerateBRDFLookupShader);
	mGenerateBRDFLookupShader = nullptr;

	theResourceFactory.DestroyItem(mBRDFLookupTarget);
	mBRDFLookupTarget = nullptr;

	theResourceFactory.DestroyItem(mConstantBuffer);
	mConstantBuffer = nullptr;

	theResourceFactory.DestroyItem(mCubemapFilterConstantBuffer);
	mCubemapFilterConstantBuffer = nullptr;

	theResourceFactory.DestroyItem(mGenerateBRDFLookupConstantBuffer);
	mGenerateBRDFLookupConstantBuffer = nullptr;

	mInitialized = false;
}

