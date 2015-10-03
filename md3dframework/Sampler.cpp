#include "Sampler.h"
#include "main.h"
#include "FrameWork.h"
#include <assert.h>


void Sampler::Init(D3D11_FILTER inFilter, D3D11_TEXTURE_ADDRESS_MODE inAddressU,
	D3D11_TEXTURE_ADDRESS_MODE inAddressV, float inMinLod, float inMaxLod)
{
	CleanUp();

	ZeroMemory(&mSampDesc, sizeof(mSampDesc));
	mSampDesc.Filter = inFilter;
	mSampDesc.AddressU = inAddressU;
	mSampDesc.AddressV = inAddressV;
	mSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mSampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	mSampDesc.MinLOD = inMinLod;
	mSampDesc.MaxLOD = inMaxLod;
	D3DCall(theFramework.GetDevice()->CreateSamplerState(&mSampDesc, &mSamplerState));

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
