#include "TextureUtil.h"
#include "RenderContext.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "ComputeShader.h"
#include "ConstantBuffer.h"


namespace TextureUtil
{
	pComputeShader gTextureAddShader;
	pComputeShader gTextureAddConstantShader;

	pComputeShader gTextureBlendShader;
	pComputeShader gTextureBlendConstantShader;

	pComputeShader gTextureClampShader;
	pComputeShader gTextureClampConstantShader;

	pComputeShader gTextureCopyShader;
	
	pComputeShader gTextureDivShader;
	pComputeShader gTextureDivConstantShader;
	
	pComputeShader gTextureMinShader;
	pComputeShader gTextureMinConstantShader;

	pComputeShader gTextureMaxShader;
	pComputeShader gTextureMaxConstantShader;

	pComputeShader gTextureMulShader;
	pComputeShader gTextureMulConstantShader;

	pComputeShader gTextureSetShader;
	
	pComputeShader gTextureSubShader;
	pComputeShader gTextureSubConstantShader;

	pComputeShader gTextureTresholdShader;
	pComputeShader gTextureTresholdConstantShader;

	pComputeShader gTextureBoxBlurShader;
	pComputeShader gTextureGaussianBlurShader;

	pComputeShader gTextureStitchNorthShader;
	pComputeShader gTextureStitchEastShader;
	pComputeShader gTextureStitchSouthShader;
	pComputeShader gTextureStitchWestShader;

	pComputeShader gTextureMipNearestShader;
	pComputeShader gTextureMipAvgShader;
	pComputeShader gTextureMipAvgNormalShader;
	pComputeShader gTextureMipMinShader;
	pComputeShader gTextureMipMaxShader;

	void InitTextureUtil()
	{
		gTextureAddShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureAdd.hlsl");
		gTextureAddConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureAddConstant.hlsl");

		gTextureBlendShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureBlend.hlsl");
		gTextureBlendConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureBlendConstant.hlsl");

		gTextureClampShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureClamp.hlsl");
		gTextureClampConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureClampConstant.hlsl");

		gTextureCopyShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureCopy.hlsl");
		
		gTextureDivShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureDiv.hlsl");
		gTextureDivConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureDivConstant.hlsl");

		gTextureMaxShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMax.hlsl");
		gTextureMaxConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMaxConstant.hlsl");

		gTextureMinShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMin.hlsl");
		gTextureMinConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMinConstant.hlsl");

		gTextureMulShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMul.hlsl");
		gTextureMulConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMulConstant.hlsl");

		gTextureSetShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureSet.hlsl");
		
		gTextureSubShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureSub.hlsl");
		gTextureSubConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureSubConstant.hlsl");
		
		gTextureTresholdShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureTreshold.hlsl");
		gTextureTresholdConstantShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureTresholdConstant.hlsl");

		gTextureBoxBlurShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureBoxBlur.hlsl");
		gTextureGaussianBlurShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureGaussianBlur.hlsl");

		gTextureStitchNorthShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureStitchNorth.hlsl");
		gTextureStitchEastShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureStitchEast.hlsl");
		gTextureStitchSouthShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureStitchSouth.hlsl");
		gTextureStitchWestShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureStitchWest.hlsl");

		gTextureMipAvgShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMipAvg.hlsl");
		gTextureMipAvgNormalShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMipAvgNormal.hlsl");
		gTextureMipNearestShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMipNearest.hlsl");
		gTextureMipMinShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMipMin.hlsl");
		gTextureMipMaxShader = theResourceFactory.LoadComputeShader("../md3dFramework/TextureFuncShaders/TextureMipMax.hlsl");
	}


	void CleanUpTextureUtil()
	{
		gTextureAddShader= nullptr;
		gTextureAddConstantShader= nullptr;
		
		gTextureBlendShader= nullptr;
		gTextureBlendConstantShader= nullptr;
		
		gTextureClampShader= nullptr;
		gTextureClampConstantShader= nullptr;

		gTextureCopyShader= nullptr;

		gTextureDivShader= nullptr;
		gTextureDivConstantShader= nullptr;

		gTextureMaxShader= nullptr;
		gTextureMaxConstantShader= nullptr;

		gTextureMinShader= nullptr;
		gTextureMinConstantShader= nullptr;

		gTextureMulShader= nullptr;
		gTextureMulConstantShader= nullptr;

		gTextureSetShader= nullptr;
		
		gTextureSubShader= nullptr;
		gTextureSubConstantShader= nullptr;
		
		gTextureTresholdShader= nullptr;
		gTextureTresholdConstantShader= nullptr;

		gTextureBoxBlurShader = nullptr;
		gTextureGaussianBlurShader = nullptr;

		gTextureStitchNorthShader= nullptr;
		gTextureStitchEastShader= nullptr;
		gTextureStitchSouthShader= nullptr;
		gTextureStitchWestShader= nullptr;

		gTextureMipAvgShader = nullptr;
		gTextureMipAvgNormalShader = nullptr;
		gTextureMipNearestShader = nullptr;
		gTextureMipMinShader = nullptr;
		gTextureMipMaxShader = nullptr;
	}


	void GPUTextureFunc(pTexture inDst, apTexture inSources, float4 inValues, ComputeShader& inShader)
	{
		pRenderTarget rt = theResourceFactory.MakeRenderTarget(inDst);
		pConstantBuffer cb = theResourceFactory.MakeConstantBuffer(sizeof(float4));
		pComputeShader cs(&inShader);

		theRenderContext.CSSetShader(cs);
		theRenderContext.CSSetRWTexture(rt, 0);

		for (int i = 0; i < inSources.size(); i++)
			theRenderContext.CSSetTexture(inSources[i], i);

		theRenderContext.UpdateSubResource(*cb, &inValues);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_x = (inDst->GetWidth() + 7) / 8;
		int threads_y = (inDst->GetHeight() + 7) / 8;
		theRenderContext.Dispatch(threads_x, threads_y, 1);

		theRenderContext.CSSetConstantBuffer(nullptr, 0);

		for (int i = 0; i < inSources.size(); i++)
			theRenderContext.CSSetTexture(nullptr, i);

		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);

		theResourceFactory.DestroyItem(cb);
		theResourceFactory.DestroyItem(rt);
	}


	void TextureBlend(pTexture inDst, pTexture inTop, pTexture inBottom, pTexture inOpacity)
	{
		apTexture sources;
		sources.push_back(inTop);
		sources.push_back(inBottom);
		sources.push_back(inOpacity);
		GPUTextureFunc(inDst, sources, float4(), *gTextureBlendShader);
	}

	
	void TextureCopy(pTexture inDst, pTexture inSrc)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, float4(), *gTextureCopyShader);
	}
	
	
	void TextureSet(pTexture inDst, float4 inVal)
	{
		apTexture sources;
		GPUTextureFunc(inDst, sources, inVal, *gTextureSetShader);
	}

	
	void TextureMul(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		apTexture sources;
		sources.push_back(inSrc1);
		sources.push_back(inSrc2);
		GPUTextureFunc(inDst, sources, float4(), *gTextureMulShader);
	}
	
	
	void TextureMul(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, inVal, *gTextureMulShader);
	}

	
	void TextureDiv(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		apTexture sources;
		sources.push_back(inSrc1);
		sources.push_back(inSrc2);
		GPUTextureFunc(inDst, sources, float4(), *gTextureDivShader);
	}
	
	
	void TextureDiv(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, inVal, *gTextureDivShader);
	}

	
	void TextureAdd(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		apTexture sources;
		sources.push_back(inSrc1);
		sources.push_back(inSrc2);
		GPUTextureFunc(inDst, sources, float4(), *gTextureAddShader);
	}
	
	
	void TextureAdd(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, inVal, *gTextureAddShader);
	}

	
	void TextureSub(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		apTexture sources;
		sources.push_back(inSrc1);
		sources.push_back(inSrc2);
		GPUTextureFunc(inDst, sources, float4(), *gTextureSubShader);
	}
	
	
	void TextureSub(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, inVal, *gTextureSubShader);
	}

	
	void TextureMax(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		apTexture sources;
		sources.push_back(inSrc1);
		sources.push_back(inSrc2);
		GPUTextureFunc(inDst, sources, float4(), *gTextureMaxShader);
	}
	
	
	void TextureMax(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, float4(), *gTextureMaxShader);
	}

	
	void TextureMin(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		apTexture sources;
		sources.push_back(inSrc1);
		sources.push_back(inSrc2);
		GPUTextureFunc(inDst, sources, float4(), *gTextureMinShader);
	}
	
	
	void TextureMin(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, inVal, *gTextureMinShader);
	}

	
	void TextureClamp(pTexture inDst, pTexture inSrc, pTexture inLoVal, pTexture inHiVal)
	{
		apTexture sources;
		sources.push_back(inSrc);
		sources.push_back(inLoVal);
		sources.push_back(inHiVal);
		GPUTextureFunc(inDst, sources, float4(), *gTextureClampShader);
	}
	
	
	void TextureClamp(pTexture inDst, pTexture inSource, float4 inLoVal, float4 inHiVal)
	{
		apTexture sources;
		sources.push_back(inSource);
		// TODO: hival 
		GPUTextureFunc(inDst, sources, inHiVal, *gTextureClampShader);
	}


	void TextureTreshold(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		apTexture sources;
		sources.push_back(inSrc1);
		sources.push_back(inSrc2);
		GPUTextureFunc(inDst, sources, float4(), *gTextureTresholdShader);
	}


	void TextureTreshold(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, inVal, *gTextureTresholdShader);
	}


	void TextureBoxBlur(pTexture inDst, pTexture inSrc, int inRadius)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, float4(inRadius, 0,0,0), *gTextureBoxBlurShader);
	}


	void TextureGaussianBlur(pTexture inDst, pTexture inSrc, int inRadius)
	{
		apTexture sources;
		sources.push_back(inSrc);
		GPUTextureFunc(inDst, sources, float4(inRadius, 0, 0, 0), *gTextureGaussianBlurShader);
	}


	void TextureStitchNorth(pTexture inDst, pTexture inSrc)
	{
		float4 params(inSrc->GetWidth() - 1, inSrc->GetHeight() - 1, 0, 0);
		assert(params.x == inDst->GetWidth() - 1);

		pRenderTarget rt = theResourceFactory.MakeRenderTarget(inDst);
		pConstantBuffer cb = theResourceFactory.MakeConstantBuffer(sizeof(float4));

		theRenderContext.CSSetShader(gTextureStitchNorthShader);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.CSSetTexture(inSrc, 0);
		theRenderContext.UpdateSubResource(*cb, &params);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_x = (params.x + 31) / 32;
		theRenderContext.Dispatch(threads_x, 1, 1);

		theRenderContext.CSSetConstantBuffer(nullptr, 0);
		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);

		theResourceFactory.DestroyItem(rt);
		theResourceFactory.DestroyItem(cb);
	}


	void TextureStitchSouth(pTexture inDst, pTexture inSrc)
	{
		float4 params(inSrc->GetWidth() - 1, inSrc->GetHeight() - 1, 0, 0);
		assert(params.x == inDst->GetWidth() - 1);

		pRenderTarget rt = theResourceFactory.MakeRenderTarget(inDst);
		pConstantBuffer cb = theResourceFactory.MakeConstantBuffer(sizeof(float4));

		theRenderContext.CSSetShader(gTextureStitchSouthShader);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.CSSetTexture(inSrc, 0);
		theRenderContext.UpdateSubResource(*cb, &params);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_x = (params.x + 31) / 32;
		theRenderContext.Dispatch(threads_x, 1, 1);

		theRenderContext.CSSetConstantBuffer(nullptr, 0);
		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);

		theResourceFactory.DestroyItem(rt);
		theResourceFactory.DestroyItem(cb);
	}


	void TextureStitchEast(pTexture inDst, pTexture inSrc)
	{
		float4 params(inSrc->GetWidth() - 1, inSrc->GetHeight() - 1, 0, 0);
		assert(params.y == inDst->GetHeight() - 1);

		pRenderTarget rt = theResourceFactory.MakeRenderTarget(inDst);
		pConstantBuffer cb = theResourceFactory.MakeConstantBuffer(sizeof(float4));

		theRenderContext.CSSetShader(gTextureStitchEastShader);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.CSSetTexture(inSrc, 0);
		theRenderContext.UpdateSubResource(*cb, &params);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_y = (params.y + 31) / 32;
		theRenderContext.Dispatch(1, threads_y, 1);

		theRenderContext.CSSetConstantBuffer(nullptr, 0);
		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);

		theResourceFactory.DestroyItem(rt);
		theResourceFactory.DestroyItem(cb);
	}


	void TextureStitchWest(pTexture inDst, pTexture inSrc)
	{
		float4 params(inSrc->GetWidth() - 1, inSrc->GetHeight() - 1, 0, 0);
		assert(params.y == inDst->GetHeight() - 1);

		pRenderTarget rt = theResourceFactory.MakeRenderTarget(inDst);
		pConstantBuffer cb = theResourceFactory.MakeConstantBuffer(sizeof(float4));

		theRenderContext.CSSetShader(gTextureStitchWestShader);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.CSSetTexture(inSrc, 0);
		theRenderContext.UpdateSubResource(*cb, &params);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_y = (params.y + 31) / 32;
		theRenderContext.Dispatch(1, threads_y, 1);

		theRenderContext.CSSetConstantBuffer(nullptr, 0);
		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);

		theResourceFactory.DestroyItem(rt);
		theResourceFactory.DestroyItem(cb);
	}


	void TextureGenerteMip(pRenderTarget inDst, int inDstLevel, pTexture inSrc, int inSrcLevel, TextureMipMode inMode)
	{
		assert(inDst->GetTexture()->GetWidth() >> (inDstLevel) == inSrc->GetWidth() >> (inSrcLevel + 1));
		assert(inDst->GetTexture()->GetHeight() >> (inDstLevel) == inSrc->GetHeight() >> (inSrcLevel + 1));

		switch (inMode)
		{
		case TextureUtil::TEXTURE_MIP_AVG:
			theRenderContext.CSSetShader(gTextureMipAvgShader);
			break;
		case TextureUtil::TEXTURE_MIP_AVG_NORMAL:
			theRenderContext.CSSetShader(gTextureMipAvgNormalShader);
			break;
		case TextureUtil::TEXTURE_MIP_NEAREST:
			theRenderContext.CSSetShader(gTextureMipNearestShader);
			break;
		case TextureUtil::TEXTURE_MIP_MIN:
			theRenderContext.CSSetShader(gTextureMipMinShader);
			break;
		case TextureUtil::TEXTURE_MIP_MAX:
			theRenderContext.CSSetShader(gTextureMipMaxShader);
			break;
		default:
			theRenderContext.CSSetShader(gTextureMipAvgShader);
			break;
		}
		
		theRenderContext.CSSetRWTexture(inDst, inDstLevel);
		theRenderContext.CSSetTexture(inSrc, 0);

		pConstantBuffer cb = theResourceFactory.MakeConstantBuffer(sizeof(float4));
		float4 values(inSrcLevel, 0, 0, 0);
		theRenderContext.UpdateSubResource(*cb, &values);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_x = ((inDst->GetTexture()->GetWidth() >> inDstLevel) + 7) / 8;
		int threads_y = ((inDst->GetTexture()->GetHeight() >> inDstLevel) + 7) / 8;
		theRenderContext.Dispatch(threads_x, threads_y, 1);
		
		theRenderContext.CSSetConstantBuffer(nullptr, 0);
		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);

		theResourceFactory.DestroyItem(cb);
	}
}
