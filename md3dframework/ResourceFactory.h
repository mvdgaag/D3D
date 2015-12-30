#pragma once
#include "GaagCommon.h"
#include "BaseResource.h"
#include "Texture.h"
#include "Sampler.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "ComputeShader.h"
#include "Mesh.h"
#include "Material.h"
#include "ConstantBuffer.h"
#include "RenderTarget.h"
#include "DepthStencilTarget.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"


#define theResourceFactory ResourceFactory::GetInstance()


class ResourceFactory
{
public:
	static ResourceFactory& GetInstance()
	{
		static ResourceFactory instance;
		return instance;
	}

	void Init();
	void CleanUp();

	void				DestroyItem(pBaseResource inResource);

	pTexture			LoadTexture(std::string inFilename);
	pPixelShader		LoadPixelShader(std::string inFilename);
	pVertexShader		LoadVertexShader(std::string inFilename);
	pComputeShader		LoadComputeShader(std::string inFilename);
	pMesh				LoadMesh(std::string inFilename);

	pMesh				MakeMesh();
	pTexture			MakeTexture(int2 inSize, int inMipLevels, Format inFormat, BindFlag inBindFlags);
	pRenderTarget		MakeRenderTarget(int2 inSize, int inMipLevels, Format inFormat);
	pRenderTarget		MakeRenderTarget(pTexture inTexture);
	pConstantBuffer		MakeConstantBuffer(int inSize);
	pMaterial			MakeMaterial();
	pSampler			MakeSampler(SamplerFilter inFilter, SamplerAddressMode inAddressModeU, SamplerAddressMode inAddressModeV);
	pDepthStencilTarget MakeDepthStencilTarget(int2 inSize);
	pPointLight			MakePointLight(float3 inPosition, float inRadius, float4 inColor);
	pSpotLight			MakeSpotLight(float3 inPosition, float inRadius, float3 inDirection, float inConeCosine, float4 inColor);
	pDirectionalLight	MakeDirectionalLight(float3 inDirection, float4 inColor);

	pMaterial			CloneMaterial(pMaterial inOther);

	pMaterial			GetDefaultMaterial()		{ return mDefaultMaterial; }
	pComputeShader		GetCopyShader()				{ return mCopyShader; }
	pSampler			GetDefaultLinearSampler()	{ return mDefaultLinearSampler; }
	pSampler			GetDefaultPointSampler()	{ return mDefaultPointSampler; }
	pMesh				GetFullScreenTriangleMesh()	{ return mFullScreenTriangleMesh; }

private:
	ResourceFactory()	{}
	~ResourceFactory()	{ CleanUp(); }
	
	void RegisterResource(pBaseResource inResource)		
	{ 
		assert(inResource != nullptr); 
		assert(mResources.find(inResource->mUUID) == mResources.end());
		mResources[inResource->mUUID] = inResource;
	}

	void UnRegisterResource(pBaseResource inResource)	
	{ 
		assert(inResource != nullptr); 
		assert(mResources.find(inResource->mUUID) != mResources.end());
		mResources.erase(inResource->mUUID);
	}

	ResourceFactory(ResourceFactory const&) = delete;
	void operator=(ResourceFactory const&) = delete;

	pMaterial		mDefaultMaterial;
	pComputeShader	mCopyShader;
	pSampler		mDefaultLinearSampler;
	pSampler		mDefaultPointSampler;
	pMesh			mFullScreenTriangleMesh;

	std::unordered_map<long, pBaseResource> mResources;
};
