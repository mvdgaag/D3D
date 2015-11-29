#include "Material.h"
#include "RenderContext.h"
#include "ConstantBuffer.h"


Material::Material(Material const& inOther)
{
	Init();
	mConstantData = inOther.mConstantData;
	mDiffuseTexture = inOther.mDiffuseTexture;
	mDiffuseSampler = inOther.mDiffuseSampler;
	mEmissiveTexture = inOther.mEmissiveTexture;
	mEmissiveSampler = inOther.mEmissiveSampler;
	mNormalTexture = inOther.mNormalTexture;
	mNormalSampler = inOther.mNormalSampler;
	mSurfaceTexture = inOther.mSurfaceTexture;
	mSurfaceSampler = inOther.mSurfaceSampler;
	mPixelShader = inOther.mPixelShader;
	mVertexShader = inOther.mVertexShader;
}


void Material::operator=(Material const& inOther)
{
	Init();
	mConstantData = inOther.mConstantData;
	mDiffuseTexture = inOther.mDiffuseTexture;
	mDiffuseSampler = inOther.mDiffuseSampler;
	mEmissiveTexture = inOther.mEmissiveTexture;
	mEmissiveSampler = inOther.mEmissiveSampler;
	mNormalTexture = inOther.mNormalTexture;
	mNormalSampler = inOther.mNormalSampler;
	mSurfaceTexture = inOther.mSurfaceTexture;
	mSurfaceSampler = inOther.mSurfaceSampler;
	mPixelShader = inOther.mPixelShader;
	mVertexShader = inOther.mVertexShader;
}


void Material::Init()
{
	mConstantBuffer = MAKE_NEW(ConstantBuffer);
	mConstantBuffer->Init(sizeof(mConstantData));
}


void Material::CleanUp()
{
	mConstantBuffer = nullptr;
}


void Material::SetDiffuseTexture(pTexture inTexture)
{
	mDiffuseTexture = inTexture;
	if (mDiffuseTexture != nullptr)
		mConstantData.flags |= MaterialFlags::DiffuseMap;
	else
		mConstantData.flags &= ~MaterialFlags::DiffuseMap;
	UpdateConstantBuffer();
}


void Material::SetNormalTexture(pTexture inTexture)
{
	mNormalTexture = inTexture;
	if (mNormalTexture != nullptr)
		mConstantData.flags |= MaterialFlags::NormalMap;
	else
		mConstantData.flags &= ~MaterialFlags::NormalMap;
	UpdateConstantBuffer();
}


void Material::SetSurfaceTexture(pTexture inTexture)
{
	mSurfaceTexture = inTexture;
	if (mSurfaceTexture != nullptr)
		mConstantData.flags |= MaterialFlags::SurfaceMap;
	else
		mConstantData.flags &= ~MaterialFlags::SurfaceMap;
	UpdateConstantBuffer();
}


void Material::SetDiffuseValue(float4 inValue) 
{ 
	mConstantData.diffuseValue = inValue; 
	UpdateConstantBuffer();
}


void Material::SetSurfaceValues(float4 inValues) 
{ 
	mConstantData.surfaceValue = inValues; 
	UpdateConstantBuffer(); 
}


void Material::SetRoughnessValue(float inValue) 
{
	mConstantData.surfaceValue.x = inValue; 
	UpdateConstantBuffer(); 
}


void Material::SetReflectivityValue(float inValue) 
{
	mConstantData.surfaceValue.y = inValue; 
	UpdateConstantBuffer(); 
}


void Material::SetMetalicityValue(float inValue) 
{
	mConstantData.surfaceValue.z = inValue; 
	UpdateConstantBuffer(); 
}


void Material::SetEmissivenessValue(float inValue)
{
	mConstantData.surfaceValue.w = inValue;
	UpdateConstantBuffer();
}


void Material::SetFlags(MaterialFlags inFlags) 
{
	mConstantData.flags = inFlags; 
	UpdateConstantBuffer(); 
}


void Material::UpdateConstantBuffer()
{
	theRenderContext.UpdateSubResource(*mConstantBuffer, &mConstantData);
}