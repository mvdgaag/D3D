#pragma once
#include "BaseResource.h"
#include "GAAGCommon.h"

struct ID3D11SamplerState;
struct D3D11_SAMPLER_DESC;

PREDEFINE(Sampler, pSampler);

class Sampler : public BaseResource
{
	friend class RenderContext;

public:
	Sampler() : BaseResource() {};
	~Sampler() {};

	void Init(int inFilter = 21, int inAddressU = 1, int inAddressV = 1, float inMinLod = 0, float inMaxLod = 1024);
	// D3D11_FILTER_MIN_MAG_MIP_LINEAR = 21
	// D3D11_TEXTURE_ADDRESS_WRAP = 1

	void CleanUp() override;
	
	ResourceType GetResourceType() const override { return ResourceType::SAMPLER; };

protected:
	ID3D11SamplerState* mSamplerState = nullptr;
	D3D11_SAMPLER_DESC* mSampDesc = nullptr;

private:
	Sampler(Sampler const&) = delete;
	void operator=(Sampler const&) = delete;
};
