#pragma once
#include "Gaag.h"
#include "Layer.h"

REGISTERCLASS(LayerStack);


enum BlendMode
{
	BlendCopy,
	BlendAdd,
	BlendSub,
	BlendMul,
	BlendDiv,
	BlendMin,
	BlendMax
};


// note that this uses opacity only when mask is not set
class LayerStack : public Layer
{
public:
	LayerStack()	{};
	~LayerStack()	{};

	LayerType	GetType() override						{ return LayerType::LAYER_STACK; }
	void		Update() override;
	bool		GetDirty(int2 inTileIndex) override;

	void		AddLayer(pLayer inLayer, pLayer inMask = nullptr, BlendMode inBlendMode = BlendCopy, float inOpacity = 1.0f);
	void		RemoveLayer(pLayer inLayer);
	void		MoveLayerUp(pLayer inLayer);
	void		MoveLayerDown(pLayer inLayer);

private:
	apLayer			mLayers;
	apLayer			mMasks;
	aint			mBlendModes;
	afloat			mOpacities;
};

