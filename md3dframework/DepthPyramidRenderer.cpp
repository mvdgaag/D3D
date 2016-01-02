#include "DepthPyramidRenderer.h"
#include "RenderContext.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "GaagCommon.h"
#include "TextureUtil.h"
#include <assert.h>


void DepthPyramidRenderer::Render(pTexture inSource, pRenderTarget inMaxTarget, pRenderTarget inMinTarget)
{
	assert(mInitialized);
	assert(inSource != nullptr);
	assert(inMaxTarget != nullptr);
	assert(inMinTarget != nullptr);
	assert(inMaxTarget->GetTexture()->GetMipLevels() == kNumMipLevels);
	assert(inMinTarget->GetTexture()->GetMipLevels() == kNumMipLevels);
	assert(inMaxTarget->GetDimensions() == inSource->GetDimensions() / 2);
	assert(inMinTarget->GetDimensions() == inSource->GetDimensions() / 2);

	// first mip from level 0 to 0 (target should be half_res)
	TextureUtil::TextureGenerteMip(inMaxTarget, 0, inSource, 0, TextureUtil::TEXTURE_MIP_MAX);
	TextureUtil::TextureGenerteMip(inMinTarget, 0, inSource, 0, TextureUtil::TEXTURE_MIP_MIN);

	// TODO: test. Are we allowed to write to a mip of the same texture?
	// or do we require a ping/pong texture?
	for (int dst_level = 1; dst_level < kNumMipLevels; dst_level++)
	{
		int src_level = dst_level - 1;
		TextureUtil::TextureGenerteMip(inMaxTarget, dst_level, inMaxTarget->GetTexture(), src_level, TextureUtil::TEXTURE_MIP_MAX);
		TextureUtil::TextureGenerteMip(inMinTarget, dst_level, inMinTarget->GetTexture(), src_level, TextureUtil::TEXTURE_MIP_MIN);
	}
}


void DepthPyramidRenderer::Init()
{
	assert(mInitialized == false);
	mInitialized = true;
}


void DepthPyramidRenderer::CleanUp()
{
	mInitialized = false;
}

