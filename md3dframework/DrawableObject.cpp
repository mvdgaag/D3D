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

	ID3D11DeviceContext* context;
	theFramework.GetDevice()->GetImmediateContext(&context);

	// set shaders
	theFramework.SetVertexShader(mVertexShader);
	theFramework.SetPixelShader(mPixelShader);
	
	// set textures
	theFramework.SetTextureAndSampler(mDiffuseTexture, mDiffuseSampler, 0);
	theFramework.SetTextureAndSampler(mNormalTexture, mNormalSampler, 1);
	theFramework.SetTextureAndSampler(mMaterialTexture, mMaterialSampler, 2);

	// todo: check if this leads to proper motion vectors
	mConstantBufferData.View = theFramework.GetCamera()->GetViewMatrix();
	mConstantBufferData.Projection = theFramework.GetCamera()->GetProjectionMatrix();
	DirectX::XMMATRIX WVP = theFramework.GetCamera()->GetViewProjectionMatrix();
	DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(WVP);
	mConstantBufferData.MV = DirectX::XMMatrixInverse(&det, WVP) * mConstantBufferData.WVP;
	mConstantBufferData.WVP = WVP;

	// set constant buffers
	ID3D11Buffer* cbuf = mConstantBuffer->GetBuffer();
	context->UpdateSubresource(cbuf, 0, nullptr, &mConstantBufferData, 0, 0);
	
	//TODO: this seems to break, but should do the same as the two lines above
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