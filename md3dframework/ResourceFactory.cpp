#include "ResourceFactory.h"


void ResourceFactory::Init()
{
	mDefaultMaterial = MakeMaterial();
	mDefaultMaterial->SetPixelShader(LoadPixelShader("../md3dFramework/Shaders/BasicFragmentShader.hlsl"));
	mDefaultMaterial->SetVertexShader(LoadVertexShader("../md3dFramework/Shaders/BasicVertexShader.hlsl"));

	mCopyShader = LoadComputeShader("../md3dFramework/Shaders/CopyCompute.hlsl");

	mDefaultLinearSampler = MakeSampler(FILTER_MIN_MAG_MIP_LINEAR, SAMPLER_ADDRESS_CLAMP, SAMPLER_ADDRESS_CLAMP);
	mDefaultPointSampler = MakeSampler(FILTER_MIN_MAG_MIP_POINT, SAMPLER_ADDRESS_CLAMP, SAMPLER_ADDRESS_CLAMP);

	mFullScreenTriangleMesh = MakeMesh();
	mFullScreenTriangleMesh->InitFullscreenTriangle();
}


void ResourceFactory::CleanUp()
{
	for (auto it = mResources.begin(); it != mResources.end(); ++it)
		delete it->second;
	mResources.clear();
}


void ResourceFactory::DestroyItem(pBaseResource inResource)
{
	unsigned short hash = UuidHash(&inResource->mUUID, nullptr);
	assert(mResources.find(hash) != mResources.end());
	delete mResources[hash];
	mResources.erase(hash);
}


pTexture ResourceFactory::LoadTexture(std::string inFilename)
{
	pTexture texture = MAKE_NEW(Texture);
	texture->InitFromFile(inFilename);
	RPC_STATUS status;
	mResources[UuidHash(&texture->mUUID, &status)] = texture;
	return texture;
};


pPixelShader ResourceFactory::LoadPixelShader(std::string inFilename)
{	
	pPixelShader shader = MAKE_NEW(PixelShader);
	shader->InitFromFile(inFilename);
	RPC_STATUS status;
	mResources[UuidHash(&shader->mUUID, &status)] = shader;
	return shader;
};


pVertexShader ResourceFactory::LoadVertexShader(std::string inFilename)
{	
	pVertexShader shader = MAKE_NEW(VertexShader);
	shader->InitFromFile(inFilename);
	RPC_STATUS status;
	mResources[UuidHash(&shader->mUUID, &status)] = shader;
	return shader;
};


pComputeShader ResourceFactory::LoadComputeShader(std::string inFilename)	
{
	pComputeShader shader = MAKE_NEW(ComputeShader);
	shader->InitFromFile(inFilename);
	RPC_STATUS status;
	mResources[UuidHash(&shader->mUUID, &status)] = shader;
	return shader;
};


pMesh ResourceFactory::LoadMesh(std::string inFilename)			
{
	pMesh mesh = MAKE_NEW(Mesh);
	mesh->InitFromFile(inFilename);
	RPC_STATUS status;
	mResources[UuidHash(&mesh->mUUID, &status)] = mesh;
	return mesh;
};


pMesh ResourceFactory::MakeMesh()
{
	pMesh mesh = MAKE_NEW(Mesh);
	mesh->InitCube();
	RPC_STATUS status;
	mResources[UuidHash(&mesh->mUUID, &status)] = mesh;
	return mesh;
}


pTexture ResourceFactory::MakeTexture(int2 inSize, int inMipLevels, Format inFormat, BindFlag inBindFlags)
{
	pTexture texture = MAKE_NEW(Texture);
	texture->Init(inSize.x, inSize.y, inMipLevels, inFormat, 0, inBindFlags);
	RPC_STATUS status;
	mResources[UuidHash(&texture->mUUID, &status)] = texture;
	return texture;
};


pRenderTarget ResourceFactory::MakeRenderTarget(int2 inSize, int inMipLevels, Format inFormat)
{
	pRenderTarget target = MAKE_NEW(RenderTarget);
	target->Init(inSize.x, inSize.y, inMipLevels, inFormat);
	RPC_STATUS status;
	mResources[UuidHash(&target->mUUID, &status)] = target;
	return target;
}


pRenderTarget ResourceFactory::MakeRenderTarget(pTexture inTexture)
{
	pRenderTarget target = MAKE_NEW(RenderTarget);
	target->Init(inTexture);
	RPC_STATUS status;
	mResources[UuidHash(&target->mUUID, &status)] = target;
	return target;
}


pConstantBuffer	ResourceFactory::MakeConstantBuffer(int inSize)
{	
	pConstantBuffer buffer = MAKE_NEW(ConstantBuffer);
	buffer->Init(inSize);
	RPC_STATUS status; 
	mResources[UuidHash(&buffer->mUUID, &status)] = buffer;
	return buffer;
}


pMaterial ResourceFactory::MakeMaterial()
{	
	pMaterial material = MAKE_NEW(Material);
	material->Init();
	RPC_STATUS status;
	mResources[UuidHash(&material->mUUID, &status)] = material;
	return material;
};


pSampler ResourceFactory::MakeSampler(SamplerFilter inFilter, SamplerAddressMode inAddressModeU, SamplerAddressMode inAddressModeV)
{	
	pSampler sampler = MAKE_NEW(Sampler);
	sampler->Init(inFilter, inAddressModeU, inAddressModeV);
	RPC_STATUS status;
	mResources[UuidHash(&sampler->mUUID, &status)] = sampler;
	return sampler;
};


pDepthStencilTarget ResourceFactory::MakeDepthStencilTarget(int2 inSize)
{
	pDepthStencilTarget target = MAKE_NEW(DepthStencilTarget);
	target->Init(inSize.x, inSize.y);
	RPC_STATUS status;
	mResources[UuidHash(&target->mUUID, &status)] = target;
	return target;
};


pPointLight ResourceFactory::MakePointLight(float3 inPosition, float inRadius, float4 inColor)
{
	pPointLight light = MAKE_NEW(PointLight);
	light->Init(inPosition, inRadius, inColor);
	RPC_STATUS status;
	mResources[UuidHash(&light->mUUID, &status)] = light;
	return light;
};


pMaterial ResourceFactory::CloneMaterial(pMaterial inOther)
{
	pMaterial result = MakeMaterial();
	*result = *inOther; // overloaded assignment operator
	return result;
};