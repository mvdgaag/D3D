#pragma once
#include "BaseResource.h"
#include "GaagCommon.h"


REGISTERCLASS(Sampler);
struct ID3D11SamplerState;

typedef enum SamplerFilter
{
	FILTER_MIN_MAG_MIP_POINT = 0,
	FILTER_MIN_MAG_POINT_MIP_LINEAR = 0x1,
	FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x4,
	FILTER_MIN_POINT_MAG_MIP_LINEAR = 0x5,
	FILTER_MIN_LINEAR_MAG_MIP_POINT = 0x10,
	FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x11,
	FILTER_MIN_MAG_LINEAR_MIP_POINT = 0x14,
	FILTER_MIN_MAG_MIP_LINEAR = 0x15,
	FILTER_ANISOTROPIC = 0x55,
	FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
	FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 0x81,
	FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x84,
	FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 0x85,
	FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 0x90,
	FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x91,
	FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 0x94,
	FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 0x95,
	FILTER_COMPARISON_ANISOTROPIC = 0xd5,
	FILTER_MINIMUM_MIN_MAG_MIP_POINT = 0x100,
	FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x101,
	FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x104,
	FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x105,
	FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x110,
	FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x111,
	FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x114,
	FILTER_MINIMUM_MIN_MAG_MIP_LINEAR = 0x115,
	FILTER_MINIMUM_ANISOTROPIC = 0x155,
	FILTER_MAXIMUM_MIN_MAG_MIP_POINT = 0x180,
	FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR = 0x181,
	FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT = 0x184,
	FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR = 0x185,
	FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT = 0x190,
	FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 0x191,
	FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT = 0x194,
	FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR = 0x195,
	FILTER_MAXIMUM_ANISOTROPIC = 0x1d5
};
inline SamplerFilter operator|(SamplerFilter a, SamplerFilter b) { return static_cast<SamplerFilter>(static_cast<int>(a) | static_cast<int>(b)); }


typedef enum SamplerAddressMode
{
	SAMPLER_ADDRESS_WRAP = 1,
	SAMPLER_ADDRESS_MIRROR = 2,
	SAMPLER_ADDRESS_CLAMP = 3,
	SAMPLER_ADDRESS_BORDER = 4,
	SAMPLER_ADDRESS_MIRROR_ONCE = 5
};
inline SamplerAddressMode operator|(SamplerAddressMode a, SamplerAddressMode b) { return static_cast<SamplerAddressMode>(static_cast<int>(a) | static_cast<int>(b)); }


class Sampler : public BaseResource
{
	friend class ResourceFactory;
	friend class RenderContext;

public:

	void Init(SamplerFilter inFilter = FILTER_MIN_MAG_MIP_LINEAR, 
		SamplerAddressMode inAddressU = SAMPLER_ADDRESS_CLAMP, 
		SamplerAddressMode inAddressV = SAMPLER_ADDRESS_CLAMP,
		int inMinLod = 0, 
		int inMaxLod = 1024);

	void CleanUp() override;
	
	ResourceType GetResourceType() const override { return ResourceType::SAMPLER; }

protected:
	SamplerFilter mFilter;
	SamplerAddressMode mAddressModeU;
	SamplerAddressMode mAddressModeV;
	int mMinLod;
	int mMaxLod;
	ID3D11SamplerState* mSamplerState;

private:
	Sampler() : BaseResource() {};
	~Sampler() { CleanUp(); }

	Sampler(Sampler const&) = delete;
	void operator=(Sampler const&) = delete;
};
