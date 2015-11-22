#include "TextureUtil.h"
#include "RenderContext.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "ComputeShader.h"
#include "ConstantBuffer.h"


namespace TextureUtil
{
	ComputeShader gTextureAddShader;
	ComputeShader gTextureAddConstantShader;

	ComputeShader gTextureBlendShader;
	ComputeShader gTextureBlendConstantShader;

	ComputeShader gTextureClampShader;
	ComputeShader gTextureClampConstantShader;

	ComputeShader gTextureCopyShader;
	
	ComputeShader gTextureDivShader;
	ComputeShader gTextureDivConstantShader;
	
	ComputeShader gTextureMinShader;
	ComputeShader gTextureMinConstantShader;

	ComputeShader gTextureMaxShader;
	ComputeShader gTextureMaxConstantShader;

	ComputeShader gTextureMulShader;
	ComputeShader gTextureMulConstantShader;

	ComputeShader gTextureSetShader;
	
	ComputeShader gTextureSubShader;
	ComputeShader gTextureSubConstantShader;

	ComputeShader gTextureTresholdShader;
	ComputeShader gTextureTresholdConstantShader;

	ComputeShader gTextureStitchNorthShader;
	ComputeShader gTextureStitchEastShader;
	ComputeShader gTextureStitchSouthShader;
	ComputeShader gTextureStitchWestShader;

	void InitTextureUtil()
	{
		gTextureAddShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureAdd.hlsl");
		gTextureAddConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureAddConstant.hlsl");

		gTextureBlendShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureBlend.hlsl");
		gTextureBlendConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureBlendConstant.hlsl");

		gTextureClampShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureClamp.hlsl");
		gTextureClampConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureClampConstant.hlsl");

		gTextureCopyShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureCopy.hlsl");
		
		gTextureDivShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureDiv.hlsl");
		gTextureDivConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureDivConstant.hlsl");

		gTextureMaxShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureMax.hlsl");
		gTextureMaxConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureMaxConstant.hlsl");

		gTextureMinShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureMin.hlsl");
		gTextureMinConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureMinConstant.hlsl");

		gTextureMulShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureMul.hlsl");
		gTextureMulConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureMulConstant.hlsl");

		gTextureSetShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureSet.hlsl");
		
		gTextureSubShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureSub.hlsl");
		gTextureSubConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureSubConstant.hlsl");
		
		gTextureTresholdShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureTreshold.hlsl");
		gTextureTresholdConstantShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureTresholdConstant.hlsl");

		gTextureStitchNorthShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureStitchNorth.hlsl");
		gTextureStitchEastShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureStitchEast.hlsl");
		gTextureStitchSouthShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureStitchSouth.hlsl");
		gTextureStitchWestShader.InitFromFile("../md3dFramework/TextureFuncShaders/TextureStitchWest.hlsl");
	}


	void CleanUpTextureUtil()
	{
		gTextureAddShader.CleanUp();
		gTextureAddConstantShader.CleanUp();
		
		gTextureBlendShader.CleanUp();
		gTextureBlendConstantShader.CleanUp();
		
		gTextureClampShader.CleanUp();
		gTextureClampConstantShader.CleanUp();

		gTextureCopyShader.CleanUp();

		gTextureDivShader.CleanUp();
		gTextureDivConstantShader.CleanUp();

		gTextureMaxShader.CleanUp();
		gTextureMaxConstantShader.CleanUp();

		gTextureMinShader.CleanUp();
		gTextureMinConstantShader.CleanUp();

		gTextureMulShader.CleanUp();
		gTextureMulConstantShader.CleanUp();

		gTextureSetShader.CleanUp();
		
		gTextureSubShader.CleanUp();
		gTextureSubConstantShader.CleanUp();
		
		gTextureTresholdShader.CleanUp();
		gTextureTresholdConstantShader.CleanUp();

		gTextureStitchNorthShader.CleanUp();
		gTextureStitchEastShader.CleanUp();
		gTextureStitchSouthShader.CleanUp();
		gTextureStitchWestShader.CleanUp();
	}


	void GPUTextureFunc(pTexture inDst, pTexture inSrc1, pTexture inSrc2, pTexture inSrc3, ComputeShader& inShader)
	{
		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pComputeShader cs(&inShader);

		theRenderContext.CSSetShader(cs);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.CSSetTexture(inSrc1, 0);
		theRenderContext.CSSetTexture(inSrc2, 1);
		theRenderContext.CSSetTexture(inSrc3, 2);

		int threads_x = (inDst->GetWidth() + 7) / 8;
		int threads_y = (inDst->GetHeight() + 7) / 8;
		theRenderContext.Dispatch(threads_x, threads_y, 1);

		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetTexture(nullptr, 1);
		theRenderContext.CSSetTexture(nullptr, 2);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);
	}


	void GPUTextureFunc(pTexture inDst, pTexture inSrc1, pTexture inSrc2, ComputeShader& inShader)
	{
		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);
		
		pComputeShader cs(&inShader);

		theRenderContext.CSSetShader(cs);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.CSSetTexture(inSrc1, 0);
		theRenderContext.CSSetTexture(inSrc2, 1);

		int threads_x = (inDst->GetWidth() + 7) / 8;
		int threads_y = (inDst->GetHeight() + 7) / 8;
		theRenderContext.Dispatch(threads_x, threads_y, 1);

		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetTexture(nullptr, 1);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);
	}


	void GPUTextureFunc(pTexture inDst, pTexture inSrc, float4 inVal, ComputeShader& inShader)
	{
		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pConstantBuffer cb = MAKE_NEW(ConstantBuffer);
		cb->Init(sizeof(float4));

		pComputeShader cs(&inShader);

		theRenderContext.CSSetShader(cs);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.CSSetTexture(inSrc, 0);
		theRenderContext.UpdateSubResource(*cb, &inVal);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_x = (inDst->GetWidth() + 7) / 8;
		int threads_y = (inDst->GetHeight() + 7) / 8;
		theRenderContext.Dispatch(threads_x, threads_y, 1);

		theRenderContext.CSSetConstantBuffer(nullptr, 0);
		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);
	}


	void GPUTextureFunc(pTexture inDst, pTexture inSrc, ComputeShader& inShader)
	{
		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pComputeShader cs(&inShader);

		theRenderContext.CSSetShader(cs);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.CSSetTexture(inSrc, 0);

		int threads_x = (inDst->GetWidth() + 7) / 8;
		int threads_y = (inDst->GetHeight() + 7) / 8;
		theRenderContext.Dispatch(threads_x, threads_y, 1);

		theRenderContext.CSSetTexture(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);
	}


	void GPUTextureFunc(pTexture inDst, float4 inVal, ComputeShader& inShader)
	{
		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pConstantBuffer cb = MAKE_NEW(ConstantBuffer);
		cb->Init(sizeof(float4));

		pComputeShader cs(&inShader);

		theRenderContext.CSSetShader(cs);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.UpdateSubResource(*cb, &inVal);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_x = (inDst->GetWidth() + 7) / 8;
		int threads_y = (inDst->GetHeight() + 7) / 8;
		theRenderContext.Dispatch(threads_x, threads_y, 1);

		theRenderContext.CSSetConstantBuffer(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);
	}


	void GPUTextureFunc(pTexture inDst, pTexture inSource, float4 inVal1, float4 inVal2, ComputeShader& inShader)
	{
		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pConstantBuffer cb = MAKE_NEW(ConstantBuffer);
		cb->Init(sizeof(float4) * 2);

		float4 cb_data[2] = { inVal1, inVal2 };
		pComputeShader cs(&inShader);

		theRenderContext.CSSetShader(cs);
		theRenderContext.CSSetRWTexture(rt, 0);
		theRenderContext.UpdateSubResource(*cb, &cb_data);
		theRenderContext.CSSetConstantBuffer(cb, 0);

		int threads_x = (inDst->GetWidth() + 7) / 8;
		int threads_y = (inDst->GetHeight() + 7) / 8;
		theRenderContext.Dispatch(threads_x, threads_y, 1);

		theRenderContext.CSSetConstantBuffer(nullptr, 0);
		theRenderContext.CSSetRWTexture(nullptr, 0);
		theRenderContext.CSSetShader(nullptr);
	}


	void TextureBlend(pTexture inDst, pTexture inTop, pTexture inBottom, pTexture inOpacity)
	{
		GPUTextureFunc(inDst, inTop, inBottom, inOpacity, gTextureBlendShader);
	}

	
	void TextureCopy(pTexture inDst, pTexture inSrc)
	{
		GPUTextureFunc(inDst, inSrc, gTextureCopyShader);
	}
	
	
	void TextureSet(pTexture inDst, float4 inVal)
	{
		GPUTextureFunc(inDst, inVal, gTextureSetShader);
	}

	
	void TextureMul(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		GPUTextureFunc(inDst, inSrc1, inSrc2, gTextureMulShader);
	}
	
	
	void TextureMul(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		GPUTextureFunc(inDst, inSrc, inVal, gTextureMulShader);
	}

	
	void TextureDiv(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		GPUTextureFunc(inDst, inSrc1, inSrc2, gTextureDivShader);
	}
	
	
	void TextureDiv(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		GPUTextureFunc(inDst, inSrc, inVal, gTextureDivShader);
	}

	
	void TextureAdd(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		GPUTextureFunc(inDst, inSrc1, inSrc2, gTextureAddShader);
	}
	
	
	void TextureAdd(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		GPUTextureFunc(inDst, inSrc, inVal, gTextureAddShader);
	}

	
	void TextureSub(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		GPUTextureFunc(inDst, inSrc1, inSrc2, gTextureSubShader);
	}
	
	
	void TextureSub(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		GPUTextureFunc(inDst, inSrc, inVal, gTextureSubShader);
	}

	
	void TextureMax(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		GPUTextureFunc(inDst, inSrc1, inSrc2, gTextureMaxShader);
	}
	
	
	void TextureMax(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		GPUTextureFunc(inDst, inSrc, inVal, gTextureMaxShader);
	}

	
	void TextureMin(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		GPUTextureFunc(inDst, inSrc1, inSrc2, gTextureMinShader);
	}
	
	
	void TextureMin(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		GPUTextureFunc(inDst, inSrc, inVal, gTextureMinShader);
	}

	
	void TextureClamp(pTexture inDst, pTexture inSrc, pTexture inLoVal, pTexture inHiVal)
	{
		GPUTextureFunc(inDst, inSrc, inLoVal, inHiVal, gTextureClampShader);
	}
	
	
	void TextureClamp(pTexture inDst, pTexture inSource, float4 inLoVal, float4 inHiVal)
	{
		GPUTextureFunc(inDst, inSource, inLoVal, inHiVal, gTextureClampShader);
	}


	void TextureTreshold(pTexture inDst, pTexture inSrc1, pTexture inSrc2)
	{
		GPUTextureFunc(inDst, inSrc1, inSrc2, gTextureTresholdShader);
	}


	void TextureTreshold(pTexture inDst, pTexture inSrc, float4 inVal)
	{
		GPUTextureFunc(inDst, inSrc, inVal, gTextureTresholdShader);
	}


	void TextureStitchNorth(pTexture inDst, pTexture inSrc)
	{
		float4 params(inSrc->GetWidth() - 1, inSrc->GetHeight() - 1, 0, 0);
		assert(params.x == inDst->GetWidth() - 1);

		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pConstantBuffer cb = MAKE_NEW(ConstantBuffer);
		cb->Init(sizeof(float4));

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
	}


	void TextureStitchSouth(pTexture inDst, pTexture inSrc)
	{
		float4 params(inSrc->GetWidth() - 1, inSrc->GetHeight() - 1, 0, 0);
		assert(params.x == inDst->GetWidth() - 1);

		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pConstantBuffer cb = MAKE_NEW(ConstantBuffer);
		cb->Init(sizeof(float4));

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
	}


	void TextureStitchEast(pTexture inDst, pTexture inSrc)
	{
		float4 params(inSrc->GetWidth() - 1, inSrc->GetHeight() - 1, 0, 0);
		assert(params.y == inDst->GetHeight() - 1);

		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pConstantBuffer cb = MAKE_NEW(ConstantBuffer);
		cb->Init(sizeof(float4));

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
	}


	void TextureStitchWest(pTexture inDst, pTexture inSrc)
	{
		float4 params(inSrc->GetWidth() - 1, inSrc->GetHeight() - 1, 0, 0);
		assert(params.y == inDst->GetHeight() - 1);

		pRenderTarget rt = MAKE_NEW(RenderTarget);
		rt->Init(inDst);

		pConstantBuffer cb = MAKE_NEW(ConstantBuffer);
		cb->Init(sizeof(float4));

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
	}
}
