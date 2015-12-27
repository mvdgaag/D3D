#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include "GaagCommon.h"


REGISTERCLASS(Texture);
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

	void TextureBlend(pTexture inDst, pTexture inTop, pTexture inBottom, pTexture inOpacity);
	
	void TextureCopy(pTexture inDst, pTexture inSrc);

	void TextureSet(pTexture inDst, float4 inVal);

	void TextureMul(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureMul(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureDiv(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureDiv(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureAdd(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureAdd(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureSub(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureSub(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureMax(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureMax(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureMin(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureMin(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureClamp(pTexture inDst, pTexture inLoSrc, pTexture inHiSrc);
	void TextureClamp(pTexture inDst, float4 inLoVal, float4 inHiVal);

	void TextureTreshold(pTexture inDst, pTexture inSrc1, pTexture inSrc2);
	void TextureTreshold(pTexture inDst, pTexture inSrc, float4 inVal);

	void TextureBoxBlur(pTexture inDst, pTexture inSrc, int radius);
	void TextureGaussianBlur(pTexture inDst, pTexture inSrc, int radius);

	void TextureStitchNorth(pTexture inDst, pTexture inSrc);
	void TextureStitchEast(pTexture inDst, pTexture inSrc);
	void TextureStitchSouth(pTexture inDst, pTexture inSrc);
	void TextureStitchWest(pTexture inDst, pTexture inSrc);

	void TextureGenerteMip(pRenderTarget inDst, int intDstLevel, pTexture inSrc, int inSrcLevel, TextureMipMode inMode);
}

#endif
