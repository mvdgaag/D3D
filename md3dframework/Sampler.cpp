#include "Sampler.h"
#include "RenderContext.h"
#include <assert.h>
#include <d3d11_1.h>


void Sampler::Init(int inFilter, int inAddressU, int inAddressV, float inMinLod, float inMaxLod)
{
	CleanUp();

	mSampDesc = new D3D11_SAMPLER_DESC();
	ZeroMemory(mSampDesc, sizeof(D3D11_SAMPLER_DESC));
	mSampDesc->Filter = (D3D11_FILTER)inFilter;
	mSampDesc->AddressU = (D3D11_TEXTURE_ADDRESS_MODE)inAddressU;
	mSampDesc->AddressV = (D3D11_TEXTURE_ADDRESS_MODE)inAddressV;
	mSampDesc->AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mSampDesc->ComparisonFunc = D3D11_COMPARISON_NEVER;
	mSampDesc->MinLOD = inMinLod;
	mSampDesc->MaxLOD = inMaxLod;
	D3DCall(theRenderContext.GetDevice()->CreateSamplerState(mSampDesc, &mSamplerState));

	assert(mSamplerState != nullptr);
}


void Sampler::CleanUp()
{
	if (mSamplerState != nullptr)
	{
		mSamplerState->Release();
		mSamplerState = nullptr;
	}
	if (mSampDesc != nullptr)
		delete mSampDesc;
}
