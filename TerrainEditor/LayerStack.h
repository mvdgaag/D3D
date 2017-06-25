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


class LayerStack : public Layer
{
public:
	LayerStack()	{};
	~LayerStack()	{};

	void UpdateComposite();

private:
	apLayer			mLayers;
	apLayer			mMasks;
	aint			mBlendModes;
};

