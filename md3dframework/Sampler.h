#pragma once
#include "BaseResource.h"
#include <d3d11_1.h>

class Sampler : public BaseResource
{
public:
	Sampler(std::string inName) : BaseResource(inName) {};
	~Sampler() {};

	void Init(D3D11_FILTER inFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_MODE inAddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_MODE inAddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		float inMinLod = 0, float inMaxLod = D3D11_FLOAT32_MAX);
	
	ID3D11SamplerState* GetSamplerState() { return mSamplerState; }
	void CleanUp();
	
	ResourceType GetResourceType() const { return ResourceType::SAMPLER; };

private:
	ID3D11SamplerState* mSamplerState = nullptr;
	D3D11_SAMPLER_DESC mSampDesc;
};

