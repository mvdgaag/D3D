#pragma once
#include <unordered_map>
#include <assert.h>
#include <DirectXMath.h>



class BaseDrawable
{
public:
	explicit BaseDrawable(std::string inName) : mName(inName) {}
	virtual ~BaseDrawable() {}
	
	virtual void Draw() = 0;
	std::string	GetName() const { return mName; }

protected:
	std::string mName;
	DirectX::XMMATRIX mTransform;
};

