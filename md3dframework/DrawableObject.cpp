#include "main.h"
#include "Framework.h"
#include "DrawableObject.h"
#include "Mesh.h"
#include "Texture.h"
#include "Sampler.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include <assert.h>


std::unordered_map<std::string, DrawableObject*> DrawableObject::sDrawableObjectMap;


DrawableObject::DrawableObject(std::string inName)
{
	mName = inName;
	assert(sDrawableObjectMap.find(inName) == sDrawableObjectMap.end());
	sDrawableObjectMap[inName] = this;
}


DrawableObject::~DrawableObject()
{
	assert(sDrawableObjectMap.find(mName) != sDrawableObjectMap.end());
	sDrawableObjectMap.erase(mName);
}


void DrawableObject::Init(Mesh* inMesh, PixelShader* inPixelShader, VertexShader* inVertexShader,
	Texture* inDiffuseTexture, Sampler* inDiffuseSampler,
	Texture* inNormalTexture, Sampler* inNormalSampler,
	Texture* inMaterialTexture, Sampler* inMaterialSampler)
{
	mMesh = inMesh;
	mPixelShader = inPixelShader;
	mVertexShader = inVertexShader;
	mDiffuseTexture = inDiffuseTexture;
	mDiffuseSampler = inDiffuseSampler;
	mNormalTexture = inNormalTexture;
	mNormalSampler = inNormalSampler;
	mMaterialTexture = inMaterialTexture;
	mMaterialSampler = inMaterialSampler;

	mConstantBuffer = new ConstantBuffer(mName + "ConstantBuffer");
	mConstantBuffer->Init(sizeof(ConstantBufferData));
	mTransform = DirectX::XMMatrixIdentity();
}


void DrawableObject::Draw()
{
	// check if everything is in place
	assert(mVertexShader != NULL);
	assert(mPixelShader != NULL);
	assert(mDiffuseTexture != NULL);
	assert(mNormalTexture != NULL);
	assert(mMaterialTexture != NULL);
	assert(mMesh != NULL);
	assert(mConstantBuffer != NULL);

	// set shaders
	theFramework.SetVertexShader(mVertexShader);
	theFramework.SetPixelShader(mPixelShader);
	
	// set textures
	theFramework.SetTextureAndSampler(mDiffuseTexture, mDiffuseSampler, 0);
	theFramework.SetTextureAndSampler(mNormalTexture, mNormalSampler, 1);
	theFramework.SetTextureAndSampler(mMaterialTexture, mMaterialSampler, 2);

	// todo: check if this leads to proper motion vectors
	const float halton23x[8] = {	1.0f/2.0f, 1.0f/4.0f, 3.0f/4.0f, 1.0f/8.0f,
									5.0f/8.0f, 3.0f/8.0f, 7.0f/8.0f, 1.0f/16.0f };
	const float halton23y[8] = {	1.0f/3.0f, 2.0f/3.0f, 1.0f/9.0f, 4.0f/9.0f,
									7.0f/9.0f, 2.0f/9.0f, 5.0f/9.0f, 8.0f/9.0f };
	int idx = theFramework.GetFrameID() % 8;
	mConstantBufferData.offsets.z = mConstantBufferData.offsets.x; // previous x
	mConstantBufferData.offsets.w = mConstantBufferData.offsets.y; // previous y
	mConstantBufferData.offsets.x = ((halton23x[idx] * 2.0 - 1.0) / theFramework.GetScreenWidth());
	mConstantBufferData.offsets.y = ((halton23y[idx] * 2.0 - 1.0) / theFramework.GetScreenHeight());
	mConstantBufferData.prevMVP = mConstantBufferData.MVP;
	mConstantBufferData.MVP = theFramework.GetCamera()->GetProjectionMatrix() * theFramework.GetCamera()->GetViewMatrix();

	// set constant buffers
	ID3D11DeviceContext* context;
	theFramework.GetDevice()->GetImmediateContext(&context);
	ID3D11Buffer* cbuf = mConstantBuffer->GetBuffer();
	context->UpdateSubresource(cbuf, 0, nullptr, &mConstantBufferData, 0, 0);
	
	//TODO: this seems to break, but should do the same as the 4 lines above
	//mConstantBuffer->SetData(&mConstantBufferData);

	theFramework.SetConstantBuffer(mConstantBuffer);
	theFramework.DrawMesh(mMesh);

	// reset state
	theFramework.SetTextureAndSampler(NULL, NULL, 0);
	theFramework.SetTextureAndSampler(NULL, NULL, 1);
	theFramework.SetTextureAndSampler(NULL, NULL, 2);
	theFramework.SetPixelShader(NULL);
	theFramework.SetVertexShader(NULL);
}