#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include "GAAGCommon.h"


PREDEFINE(Texture, pTexture);


namespace TextureUtil
{
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
}

#endif
