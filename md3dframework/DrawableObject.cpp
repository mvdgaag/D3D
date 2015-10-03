#include "main.h"
#include "FrameWork.h"
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

	ID3D11DeviceContext* context = theFramework.GetContext();

	// set shaders
	context->VSSetShader(mVertexShader->GetHandle(), NULL, 0);
	context->PSSetShader(mPixelShader->GetHandle(), NULL, 0);
	
	// set textures
	ID3D11ShaderResourceView* diffuse_tex = mDiffuseTexture->GetShaderResourceView();
	ID3D11SamplerState* diffuse_samp = mDiffuseSampler->GetSamplerState();
	context->PSSetShaderResources(0, 1, &diffuse_tex);
	context->PSSetSamplers(0, 1, &diffuse_samp);

	ID3D11ShaderResourceView* normal_tex = mNormalTexture->GetShaderResourceView();
	ID3D11SamplerState* normal_samp = mNormalSampler->GetSamplerState();
	context->PSSetShaderResources(1, 1, &normal_tex);
	context->PSSetSamplers(1, 1, &normal_samp);
	
	ID3D11ShaderResourceView* material_tex = mMaterialTexture->GetShaderResourceView();
	ID3D11SamplerState* material_samp = mMaterialSampler->GetSamplerState();
	context->PSSetShaderResources(2, 1, &material_tex);
	context->PSSetSamplers(2, 1, &material_samp);

	// set constant buffers
	ID3D11Buffer* cbuf = mConstantBuffer->GetBuffer();

	// todo: check if this leads to proper motion vectors
	mConstantBufferData.View = theFramework.GetCamera()->GetViewMatrix();
	mConstantBufferData.Projection = theFramework.GetCamera()->GetProjectionMatrix();
	DirectX::XMMATRIX WVP = theFramework.GetCamera()->GetViewProjectionMatrix();
	DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(WVP);
	mConstantBufferData.MV = DirectX::XMMatrixInverse(&det, WVP) * mConstantBufferData.WVP;
	mConstantBufferData.WVP = WVP;

	context->UpdateSubresource(cbuf, 0, nullptr, &mConstantBufferData, 0, 0);
	context->VSSetConstantBuffers(0, 1, &cbuf);
	
	// set vertex data
	ID3D11Buffer* mesh_verts = mMesh->GetVertexBuffer();
	UINT stride = mMesh->GetStride();
	UINT offset = mMesh->GetOffset();
	context->IASetVertexBuffers( 0, 1, &mesh_verts, &stride, &offset);
	context->IASetInputLayout(mVertexShader->GetLayout());

	ID3D11Buffer* mesh_indices = mMesh->GetIndexBuffer();
	context->IASetIndexBuffer(mesh_indices, DXGI_FORMAT_R16_UINT, 0);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw
	context->DrawIndexed(mMesh->GetNumIndices(), 0, 0);

	// reset state
	ID3D11ShaderResourceView* none[] = { NULL, NULL, NULL };
	context->PSSetShaderResources(0, 3, none);
	context->PSSetShader(NULL, NULL, 0);
	context->VSSetShader(NULL, NULL, 0);
}