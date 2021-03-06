#include "ResourceFactory.h"
#include "GaagFramework.h"


void ResourceFactory::Init()
{
	mDefaultMaterial = MakeMaterial();
	mDefaultMaterial->SetPixelShader(LoadPixelShader(Gaag.GetDirectory("Shaders/BasicFragmentShader.hlsl")));
	mDefaultMaterial->SetVertexShader(LoadVertexShader(Gaag.GetDirectory("Shaders/BasicVertexShader.hlsl")));
	mDefaultMaterial->SetDiffuseValue(float4(0.5, 0.5, 0.5, 1.0));
	mDefaultMaterial->SetReflectivityValue(0.5);
	mDefaultMaterial->SetRoughnessValue(0.5);

	mCopyShader = LoadComputeShader(Gaag.GetDirectory("Shaders/CopyCompute.hlsl"));

	mDefaultLinearSampler = MakeSampler(FILTER_MIN_MAG_MIP_LINEAR, SAMPLER_ADDRESS_CLAMP, SAMPLER_ADDRESS_CLAMP);
	mDefaultPointSampler = MakeSampler(FILTER_MIN_MAG_MIP_POINT, SAMPLER_ADDRESS_CLAMP, SAMPLER_ADDRESS_CLAMP);

	mFullScreenTriangleMesh = MakeMesh();
	mFullScreenTriangleMesh->InitFullscreenTriangle();
}


void ResourceFactory::CleanUp()
{
	apBaseResource all_resources;
	for (auto it = mResources.begin(); it != mResources.end(); ++it)
		all_resources.push_back(it->second);
	for (int i = 0; i < all_resources.size(); i++)
		UnRegisterResource(all_resources[i]);
	assert(mResources.size() == 0);
}


void ResourceFactory::DestroyItem(pBaseResource inResource)
{
	assert(inResource != nullptr);
	UnRegisterResource(inResource);
	delete inResource;
}


pTexture ResourceFactory::LoadTexture(std::string inFilename, BindFlag inAdditionalBindFlags)
{
	pTexture texture = MAKE_NEW(Texture);
	texture->InitFromFile(inFilename, inAdditionalBindFlags);
	RegisterResource(texture);
	return texture;
};


pPixelShader ResourceFactory::LoadPixelShader(std::string inFilename)
{	
	pPixelShader shader = MAKE_NEW(PixelShader);
	shader->InitFromFile(inFilename);
	RegisterResource(shader);
	return shader;
};


pVertexShader ResourceFactory::LoadVertexShader(std::string inFilename)
{	
	pVertexShader shader = MAKE_NEW(VertexShader);
	shader->InitFromFile(inFilename);
	RegisterResource(shader);
	return shader;
};


pComputeShader ResourceFactory::LoadComputeShader(std::string inFilename)	
{
	pComputeShader shader = MAKE_NEW(ComputeShader);
	shader->InitFromFile(inFilename);
	RegisterResource(shader);
	return shader;
};


pMesh ResourceFactory::LoadMesh(std::string inFilename)			
{
	pMesh mesh = MAKE_NEW(Mesh);
	mesh->InitFromFile(inFilename);
	RegisterResource(mesh);
	return mesh;
};


pMesh ResourceFactory::MakeMesh()
{
	pMesh mesh = MAKE_NEW(Mesh);
	mesh->InitCube();
	RegisterResource(mesh);
	return mesh;
}


pTexture ResourceFactory::MakeTexture(int2 inSize, int inMipLevels, Format inFormat, BindFlag inBindFlags, MiscFlag inMiscFlags)
{
	pTexture texture = MAKE_NEW(Texture);
	texture->Init(inSize.x, inSize.y, inMipLevels, inFormat, 0, inBindFlags, CPU_ACCESS_DEFAULT, inMiscFlags);
	RegisterResource(texture);
	return texture;
};


pRenderTarget ResourceFactory::MakeRenderTarget(int2 inSize, int inMipLevels, Format inFormat)
{
	pRenderTarget target = MAKE_NEW(RenderTarget);
	target->Init(inSize.x, inSize.y, inMipLevels, inFormat);
	RegisterResource(target);
	return target;
}


pRenderTarget ResourceFactory::MakeRenderTarget(pTexture inTexture)
{
	pRenderTarget target = MAKE_NEW(RenderTarget);
	target->Init(inTexture);
	RegisterResource(target);
	return target;
}


pConstantBuffer	ResourceFactory::MakeConstantBuffer(int inSize)
{	
	pConstantBuffer buffer = MAKE_NEW(ConstantBuffer);
	buffer->Init(inSize);
	RegisterResource(buffer);
	return buffer;
}


pMaterial ResourceFactory::MakeMaterial()
{	
	pMaterial material = MAKE_NEW(Material);
	material->Init();
	RegisterResource(material);
	return material;
};


pSampler ResourceFactory::MakeSampler(SamplerFilter inFilter, SamplerAddressMode inAddressModeU, SamplerAddressMode inAddressModeV)
{	
	pSampler sampler = MAKE_NEW(Sampler);
	sampler->Init(inFilter, inAddressModeU, inAddressModeV);
	RegisterResource(sampler);
	return sampler;
};


pDepthStencilTarget ResourceFactory::MakeDepthStencilTarget(int2 inSize)
{
	pDepthStencilTarget target = MAKE_NEW(DepthStencilTarget);
	target->Init(inSize.x, inSize.y);
	RegisterResource(target);
	return target;
};


pPointLight ResourceFactory::MakePointLight(float3 inPosition, float inRadius, float4 inColor)
{
	pPointLight light = MAKE_NEW(PointLight);
	light->Init(inPosition, inRadius, inColor);
	RegisterResource(light);
	return light;
};


pSpotLight ResourceFactory::MakeSpotLight(float3 inPosition, float inRadius, float3 inDirection, float inConeCosine, float4 inColor)
{
	pSpotLight light = MAKE_NEW(SpotLight);
	light->Init(inPosition, inRadius, inDirection, inConeCosine, inColor);
	RegisterResource(light);
	return light;
}


pDirectionalLight ResourceFactory::MakeDirectionalLight(float3 inDirection, float4 inColor, int inShadowMapSize)
{
	pDirectionalLight light = MAKE_NEW(DirectionalLight);
	light->Init(inDirection, inColor, inShadowMapSize);
	RegisterResource(light);
	return light;
}


pMaterial ResourceFactory::CloneMaterial(pMaterial inOther)
{
	pMaterial result = MakeMaterial();
	*result = *inOther; // overloaded assignment operator
	return result;
};