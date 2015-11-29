#pragma once
#include "GaagCommon.h"

PREDEFINE(Texture, pTexture);
PREDEFINE(Sampler, pSampler);
PREDEFINE(PixelShader, pPixelShader);
PREDEFINE(VertexShader, pVertexShader);
PREDEFINE(ConstantBuffer, pConstantBuffer);
PREDEFINE(Material, pMaterial);


class Material
{
public:
	enum MaterialFlags
	{
		DiffuseMap	= 1 << 0,
		NormalMap	= 1 << 1,
		SurfaceMap	= 1 << 2,
	};

	Material() {};
	~Material() {};
	Material(Material const& inOther);
	void operator=(Material const& inOther);

	void Init();
	void CleanUp();

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
};
