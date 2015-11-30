#pragma once
#include "GaagCommon.h"
#include "BaseResource.h"

REGISTERCLASS(Texture);
REGISTERCLASS(Sampler);
REGISTERCLASS(PixelShader);
REGISTERCLASS(VertexShader);
REGISTERCLASS(ConstantBuffer);
REGISTERCLASS(Material);


class Material : public BaseResource
{
	friend class ResourceFactory;
	friend class RenderContext;

public:
	enum MaterialFlags
	{
		DiffuseMap	= 1 << 0,
		NormalMap	= 1 << 1,
		SurfaceMap	= 1 << 2,
	};

	void Init();
	void CleanUp() override;

	void SetPixelShader(pPixelShader inPixelShader)  { mPixelShader = inPixelShader; }
	pPixelShader GetPixelShader() { return mPixelShader; }

	void SetVertexShader(pVertexShader inVertexShader)  { mVertexShader = inVertexShader; }
	pVertexShader GetVertexShader() { return mVertexShader; }

	void SetDiffuseTexture(pTexture inTexture);
	pTexture GetDiffuseTexture() { return mDiffuseTexture; }
	
	void SetNormalTexture(pTexture inTexture);
	pTexture GetNormalTexture() { return mNormalTexture; }
	
	void SetSurfaceTexture(pTexture inTexture);
	pTexture GetSurfaceTexture() { return mSurfaceTexture; }

	void SetDiffuseValue(float4 inValue);
	float4 GetDiffuseValue() { return mConstantData.diffuseValue; }

	void SetSurfaceValues(float4 inValues);
	float4 GetSurfaceValues() { return mConstantData.surfaceValue; }

	void SetRoughnessValue(float inValue);
	float GetRoughnessValue() { return mConstantData.surfaceValue.x; }
	
	void SetReflectivityValue(float inValue);
	float GetReflectivityValue() { return mConstantData.surfaceValue.y; }
	
	void SetMetalicityValue(float inValue);
	float GetMetalicityValue() { return mConstantData.surfaceValue.z; }

	void SetEmissivenessValue(float inValue);
	float GetEmissivenessValue() { return mConstantData.surfaceValue.w; }

	void SetFlags(MaterialFlags inFlags);
	MaterialFlags GetFlags() { return (MaterialFlags)mConstantData.flags; }

	pConstantBuffer GetConstantBuffer() { return mConstantBuffer; }

	ResourceType GetResourceType() const override { return ResourceType::MATERIAL; }

protected:
	void UpdateConstantBuffer();

	struct ConstantData
	{
		float4 diffuseValue;
		float4 surfaceValue;
		unsigned int flags;
		unsigned int padding1;
		unsigned int padding2;
		unsigned int padding3;
	};
	ConstantData mConstantData;

	pTexture mDiffuseTexture = nullptr;
	pSampler mDiffuseSampler = nullptr;
	pTexture mEmissiveTexture = nullptr;
	pSampler mEmissiveSampler = nullptr;
	pTexture mNormalTexture = nullptr;
	pSampler mNormalSampler = nullptr;
	pTexture mSurfaceTexture = nullptr; // roughness, reflectivity, metalicity, emissiveness
	pSampler mSurfaceSampler = nullptr;

	pPixelShader mPixelShader = nullptr;
	pVertexShader mVertexShader = nullptr;

	pConstantBuffer mConstantBuffer = nullptr;

private:
	Material() : BaseResource() {}
	~Material() { CleanUp(); }
	Material(Material const& inOther);
	void operator=(Material const& inOther);

};
