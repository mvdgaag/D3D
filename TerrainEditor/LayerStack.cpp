#include "LayerStack.h"


void LayerStack::UpdateComposite()
{
	for (int y = 0; y < GetNumTiles().y; y++)
	{
		for (int x = 0; x < GetNumTiles().x; x++)
		{
			int2 tile_index = int2(x, y);
			bool is_dirty = GetDirty(tile_index);
			for (int i = 0; i < mLayers.size(); i++)
			{
				is_dirty |= mLayers[i]->GetDirty(tile_index);
			}

			if (is_dirty)
			{
				pRenderTarget blend_result = theResourceFactory.MakeRenderTarget(GetTileResolution(), 1, GetTileTexture(tile_index)->GetFormat());
				pRenderTarget base = theResourceFactory.MakeRenderTarget(GetTileResolution(), 1, GetTileTexture(tile_index)->GetFormat());

				for (int i = 0; i < mLayers.size(); i++)
				{
					BlendMode blend_mode = (BlendMode)mBlendModes[i];

					switch (blend_mode)
					{
					case BlendMode::BlendCopy:
						TextureUtil::TextureCopy(blend_result, mLayers[i]->GetTileTexture(tile_index));
						break;
					case BlendMode::BlendAdd:
						TextureUtil::TextureAdd(blend_result, GetTileTexture(tile_index), mLayers[i]->GetTileTexture(tile_index));
						break;
					case BlendMode::BlendSub:
						TextureUtil::TextureAdd(blend_result, GetTileTexture(tile_index), mLayers[i]->GetTileTexture(tile_index));
						break;
					case BlendMode::BlendMul:
						TextureUtil::TextureAdd(blend_result, GetTileTexture(tile_index), mLayers[i]->GetTileTexture(tile_index));
						break;
					case BlendMode::BlendDiv:
						TextureUtil::TextureAdd(blend_result, GetTileTexture(tile_index), mLayers[i]->GetTileTexture(tile_index));
						break;
					case BlendMode::BlendMin:
						TextureUtil::TextureAdd(blend_result, GetTileTexture(tile_index), mLayers[i]->GetTileTexture(tile_index));
						break;
					case BlendMode::BlendMax:
						TextureUtil::TextureAdd(blend_result, GetTileTexture(tile_index), mLayers[i]->GetTileTexture(tile_index));
						break;
					default:
						assert(false);
						break;
					}

					TextureUtil::TextureCopy(base, GetTileTexture(tile_index));
					TextureUtil::TextureBlend(GetTileRenderTarget(tile_index), blend_result->GetTexture(), base->GetTexture(), mMasks[i]->GetTileTexture(tile_index));
				}
				
				theResourceFactory.DestroyItem(base);
				theResourceFactory.DestroyItem(blend_result);
			}
		}
	}
}
