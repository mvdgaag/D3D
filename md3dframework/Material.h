#pragma once
#include "float4.h"

class Texture;
class Sampler;
class PixelShader;
class VertexShader;
class ConstantBuffer;


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

	void Init();
	void CleanUp();

	void SetPixelShader(PixelShader* inPixelShader)  { mPixelShader = inPixelShader; }
	PixelShader* GetPixelShader() { return mPixelShader; }

	void SetVertexShader(VertexShader* inVertexShader)  { mVertexShader = inVertexShader; }
	VertexShader* GetVertexShader() { return mVertexShader; }

	void SetDiffuseTexture(Texture* inTexture);
	Texture* GetDiffuseTexture() { return mDiffuseTexture; }
	
	void SetNormalTexture(Texture* inTexture);
	Texture* GetNormalTexture() { return mNormalTexture; }
	
	void SetSurfaceTexture(Texture* inTexture);
	Texture* GetSurfaceTexture() { return mSurfaceTexture; }

	void SetDiffuseValue(float3 inValue);
	float3 GetDiffuseValue() { return mConstantData.diffuseValue; }

	void SetSurfaceValues(float4 inValues);
	float4 GetSurfaceValues() { return mConstantData.surfaceValue; }

	void SetRoughnessValue(float inValue);
	float GetRoughnessValue() { return mConstantData.surfaceValue.x; }
	
	void SetReflectivityValue(float inValue);
	float GetReflectivityValue() { return mConstantData.surfaceValue.y; }
	
	void SetMetalicityValue(float inValue);
	float GetMetalicityValue() { return mConstantData.surfaceValue.z; }

	void SetFlags(MaterialFlags inFlags);
	MaterialFlags GetFlags() { return (MaterialFlags)mConstantData.flags; }

	ConstantBuffer* GetConstantBuffer() { return mConstantBuffer; }

private:
	void UpdateConstantBuffer();

	struct ConstantData
	{
		float3 diffuseValue;
		float4 surfaceValue;
		unsigned int flags;
	};
	ConstantData mConstantData;

	Texture* mDiffuseTexture = nullptr;
	Sampler* mDiffuseSampler = nullptr;
	Texture* mEmissiveTexture = nullptr;
	Sampler* mEmissiveSampler = nullptr;
	Texture* mNormalTexture = nullptr;
	Sampler* mNormalSampler = nullptr;
	Texture* mSurfaceTexture = nullptr; // roughness, reflectivity, metalicity, emissiveness
	Sampler* mSurfaceSampler = nullptr;

	PixelShader* mPixelShader = nullptr;
	VertexShader* mVertexShader = nullptr;

	ConstantBuffer* mConstantBuffer = nullptr;
};

