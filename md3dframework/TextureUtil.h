#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include "GaagCommon.h"


REGISTERCLASS(Texture);
REGISTERCLASS(Sampler);
REGISTERCLASS(RenderTarget);


namespace TextureUtil
{
	enum TextureMipMode
	{
		TEXTURE_MIP_AVG,
		TEXTURE_MIP_AVG_NORMAL,
		TEXTURE_MIP_NEAREST,
		TEXTURE_MIP_MIN,
		TEXTURE_MIP_MAX,
	};

	void InitTextureUtil();

	void CleanUpTextureUtil();

	void TextureDownSample(pRenderTarget target, pTexture source, pSampler sampler);
	void TextureDownSample(pTexture target, pTexture source, pSampler sampler);

	void TextureBlend(pRenderTarget inDst, pTexture inTop, pTexture inBottom, float inOpacity);
	void TextureBlend(pTexture inDst, pTexture inTop, pTexture inBottom, float inOpacity); 

	void TextureBlend(pRenderTarget inDst, pTexture inTop, pTexture inBottom, pTexture inOpacity);
	void TextureBlend(pTexture inDst, pTexture inTop, pTexture inBottom, pTexture inOpacity);

	void TextureAlphaBlend(pRenderTarget inDst, pTexture inTop, pTexture inBottom);
	void TextureAlphaBlend(pTexture inDst, pTexture inTop, pTexture inBottom);

	void TextureCopy(pRenderTarget inDst, pTexture inSrc);
	void TextureCopy(pTexture inDst, pTexture inSrc);

	void TextureCopyOffset(pRenderTarget inDst, pTexture inSrc, int2 inOffset);
	void TextureCopyOffset(pTexture inDst, pTexture inSrc, int2 inOffset);

	void TextureSet(pRenderTarget inDst, float4 inVal);
	void TextureSet(pTexture inDst, float4 inVal);

	void TextureMul(pRenderTarget inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureMul(pTexture inDst, pTexture inSrc1, pTexture inSrc2);

	void TextureMul(pRenderTarget inDst, pTexture inSrc, float4 inVal);
	void TextureMul(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureDiv(pRenderTarget inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureDiv(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	
	void TextureDiv(pRenderTarget inDst, pTexture inSrc, float4 inVal);
	void TextureDiv(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureAdd(pRenderTarget inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureAdd(pTexture inDst, pTexture inSrc1, pTexture inSrc2);

	void TextureAdd(pRenderTarget inDst, pTexture inSrc, float4 inVal);
	void TextureAdd(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureSub(pRenderTarget inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureSub(pTexture inDst, pTexture inSrc1, pTexture inSrc2);

	void TextureSub(pRenderTarget inDst, pTexture inSrc, float4 inVal);
	void TextureSub(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureMax(pRenderTarget inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureMax(pTexture inDst, pTexture inSrc1, pTexture inSrc2);

	void TextureMax(pRenderTarget inDst, pTexture inSrc, float4 inVal);
	void TextureMax(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureMin(pRenderTarget inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureMin(pTexture inDst, pTexture inSrc1, pTexture inSrc2);

	void TextureMin(pRenderTarget inDst, pTexture inSrc, float4 inVal);
	void TextureMin(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureClamp(pRenderTarget inDst, pTexture inLoSrc, pTexture inHiSrc);
	void TextureClamp(pTexture inDst, pTexture inLoSrc, pTexture inHiSrc);

	void TextureClamp(pRenderTarget inDst, float4 inLoVal, float4 inHiVal);
	void TextureClamp(pTexture inDst, float4 inLoVal, float4 inHiVal);

	void TextureTreshold(pRenderTarget inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureTreshold(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	
	void TextureTreshold(pRenderTarget inDst, pTexture inSrc, float4 inVal);
	void TextureTreshold(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureBoxBlur(pRenderTarget inDst, pTexture inSrc, int radius);
	void TextureBoxBlur(pTexture inDst, pTexture inSrc, int radius);

	void TextureGaussianBlur(pRenderTarget inDst, pTexture inSrc, int radius);
	void TextureGaussianBlur(pTexture inDst, pTexture inSrc, int radius);

	void TextureStitchNorth(pTexture inDst, pTexture inSrc);
	void TextureStitchEast(pTexture inDst, pTexture inSrc);
	void TextureStitchSouth(pTexture inDst, pTexture inSrc);
	void TextureStitchWest(pTexture inDst, pTexture inSrc);

	void TextureGenerteMip(pRenderTarget inDst, int intDstLevel, pTexture inSrc, int inSrcLevel, TextureMipMode inMode);
}

#endif
