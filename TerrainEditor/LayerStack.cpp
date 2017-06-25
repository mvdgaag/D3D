#include "LayerStack.h"


void LayerStack::Update()
{
	for (int y = 0; y < mNumTiles.y; y++)
	{
		for (int x = 0; x < mNumTiles.x; x++)
		{
			int2 tile_index = int2(x, y);

			bool is_dirty = Layer::GetDirty(tile_index);
			for (int i = 0; i < mLayers.size(); i++)
			{
				if (mLayers[i]->GetDirty(tile_index))
				{
					is_dirty = true;
					mLayers[i]->Update();
				}
			}

			if (is_dirty)
			{
				pRenderTarget blend_result = theResourceFactory.MakeRenderTarget(GetTileResolution(), 1, GetTileTexture(tile_index)->GetFormat());
				pRenderTarget base = theResourceFactory.MakeRenderTarget(GetTileResolution(), 1, GetTileTexture(tile_index)->GetFormat());

				for (int i = mLayers.size() - 1; i >= 0; i--)
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

					if (mMasks[i] != nullptr)
						TextureUtil::TextureBlend(GetTileRenderTarget(tile_index), blend_result->GetTexture(), base->GetTexture(), mMasks[i]->GetTileTexture(tile_index));
					else
						TextureUtil::TextureBlend(GetTileRenderTarget(tile_index), blend_result->GetTexture(), base->GetTexture(), mOpacities[i]);
				}
				
				theResourceFactory.DestroyItem(base);
				theResourceFactory.DestroyItem(blend_result);

				SetDirty(tile_index, false);
			}
		}
	}
}


bool LayerStack::GetDirty(int2 inTileIndex)
{
	if (Layer::GetDirty(inTileIndex))
		return true;

	for (int i = 0; i < mLayers.size(); i++)
	{
		if (mLayers[i]->GetDirty(inTileIndex))
			return true;
	}

	return false;
}


void LayerStack::AddLayer(pLayer inLayer, pLayer inMask, BlendMode inBlendMode, float inOpacity)
{
	assert(inLayer != nullptr);

	mLayers.push_back(inLayer);
	mMasks.push_back(inMask);
	mBlendModes.push_back(inBlendMode);
	mOpacities.push_back(inOpacity);

	SetAllDirty();
}


void LayerStack::RemoveLayer(pLayer inLayer)
{
	assert(inLayer != nullptr);

	for (int i = 0; i < mLayers.size(); i++)
	{
		if (mLayers[i] == inLayer)
		{
			mLayers.erase(mLayers.begin() + i);
			mMasks.erase(mMasks.begin() + i);
			mBlendModes.erase(mBlendModes.begin() + i);
			mOpacities.erase(mOpacities.begin() + i);
			return;
		}
	}

	SetAllDirty();
}


void LayerStack::MoveLayerUp(pLayer inLayer)
{
	assert(inLayer != nullptr);

	for (int i = 1; i < mLayers.size(); i++)
	{
		if (mLayers[i] == inLayer)
		{
			pLayer tmp = mLayers[i - 1];
			mLayers[i - 1] = mLayers[i];
			mLayers[i] = tmp;

			tmp = mMasks[i - 1];
			mMasks[i - 1] = mMasks[i];
			mMasks[i] = tmp;

			int tmpi = mBlendModes[i - 1];
			mBlendModes[i - 1] = mBlendModes[i];
			mBlendModes[i] = tmpi;

			float tmpf = mOpacities[i - 1];
			mOpacities[i - 1] = mOpacities[i];
			mOpacities[i] = tmpf;

			return;
		}
	}

	SetAllDirty();
}


void LayerStack::MoveLayerDown(pLayer inLayer)
{
	assert(inLayer != nullptr);

	for (int i = 0; i < mLayers.size() - 1; i++)
	{
		if (mLayers[i] == inLayer)
		{
			pLayer tmp = mLayers[i + 1];
			mLayers[i + 1] = mLayers[i];
			mLayers[i] = tmp;

			tmp = mMasks[i + 1];
			mMasks[i + 1] = mMasks[i];
			mMasks[i] = tmp;

			int tmpi = mBlendModes[i + 1];
			mBlendModes[i + 1] = mBlendModes[i];
			mBlendModes[i] = tmpi;

			float tmpf = mOpacities[i + 1];
			mOpacities[i + 1] = mOpacities[i];
			mOpacities[i] = tmpf;

			return;
		}
	}

	SetAllDirty();
}
