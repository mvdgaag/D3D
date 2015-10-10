#pragma once
#include <unordered_map>
#include <assert.h>
#include <DirectXMath.h>



class BaseDrawable
{
public:
	explicit BaseDrawable() {}
	virtual ~BaseDrawable() {}
	
	virtual void Draw() = 0;

protected:
	DirectX::XMMATRIX mTransform;
};

