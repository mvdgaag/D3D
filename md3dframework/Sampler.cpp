#include "Sampler.h"
#include "RenderContext.h"
#include <assert.h>
#include <d3d11_1.h>


void Sampler::Init(SamplerFilter inFilter, SamplerAddressMode inAddressU, SamplerAddressMode inAddressV, int inMinLod, int inMaxLod)
{
	CleanUp();

	D3D11_SAMPLER_DESC* samp_desc = new D3D11_SAMPLER_DESC();
	ZeroMemory(samp_desc, sizeof(D3D11_SAMPLER_DESC));
	samp_desc->Filter = (D3D11_FILTER)inFilter;
	mFilter = inFilter;
	samp_desc->AddressU = (D3D11_TEXTURE_ADDRESS_MODE)inAddressU;
	mAddressModeU = inAddressU;
	samp_desc->AddressV = (D3D11_TEXTURE_ADDRESS_MODE)inAddressV;
	mAddressModeV = inAddressV;
	samp_desc->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samp_desc->ComparisonFunc = D3D11_COMPARISON_NEVER;
	samp_desc->MinLOD = mMinLod = inMinLod;
	samp_desc->MaxLOD = mMaxLod = inMaxLod;
	
	D3DCall(theRenderContext.GetDevice()->CreateSamplerState(samp_desc, &mSamplerState));

	assert(mSamplerState != nullptr);
}


void Sampler::CleanUp()
{
	if (mSamplerState != nullptr)
	{
		mSamplerState->Release();
		mSamplerState = nullptr;
	}
}
