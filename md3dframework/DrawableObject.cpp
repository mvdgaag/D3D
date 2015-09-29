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


void DrawableObject::Draw(ID3D11DeviceContext* inContext)
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
	inContext->VSSetShader(mVertexShader->GetHandle(), NULL, 0);
	inContext->PSSetShader(mPixelShader->GetHandle(), NULL, 0);
	
	// set textures
	ID3D11ShaderResourceView* diffuse_tex = mDiffuseTexture->GetShaderResourceView();
	ID3D11SamplerState* diffuse_samp = mDiffuseSampler->GetSamplerState();
	inContext->PSSetShaderResources(0, 1, &diffuse_tex);
	inContext->PSSetSamplers(0, 1, &diffuse_samp);

	ID3D11ShaderResourceView* normal_tex = mNormalTexture->GetShaderResourceView();
	ID3D11SamplerState* normal_samp = mNormalSampler->GetSamplerState();
	inContext->PSSetShaderResources(1, 1, &normal_tex);
	inContext->PSSetSamplers(1, 1, &normal_samp);
	
	ID3D11ShaderResourceView* material_tex = mMaterialTexture->GetShaderResourceView();
	ID3D11SamplerState* material_samp = mMaterialSampler->GetSamplerState();
	inContext->PSSetShaderResources(2, 1, &material_tex);
	inContext->PSSetSamplers(2, 1, &material_samp);

	// set constant buffers
	ID3D11Buffer* cbuf = mConstantBuffer->GetBuffer();

	//DirectX::XMMATRIX WVP = mTransform * theFrameWork.GetCamera()->GetViewProjectionMatrix();
	DirectX::XMMATRIX WVP = theFrameWork.GetCamera()->GetViewProjectionMatrix();

	// todo: check if this leads to proper motion vectors
	DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(WVP);
	mConstantBufferData.MV = DirectX::XMMatrixInverse(&det, WVP) * mConstantBufferData.WVP;
	mConstantBufferData.WVP = WVP;

	static float a;
	a += 0.01;
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(5.0f * sin(a), 1.0f, 5.0f * cos(a), 0.0f);
	DirectX::XMVECTOR tar = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	
	mConstantBufferData.View = DirectX::XMMatrixLookAtLH(eye, tar, up);
	mConstantBufferData.Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 800.0f/600.0f, 0.01f, 100.0f);
	
	mConstantBufferData.View = DirectX::XMMatrixTranspose(mConstantBufferData.View);
	mConstantBufferData.Projection = DirectX::XMMatrixTranspose(mConstantBufferData.Projection);

	//mConstantBufferData.View = theFrameWork.GetCamera()->GetViewMatrix();
	//mConstantBufferData.Projection = theFrameWork.GetCamera()->GetViewProjectionMatrix();

	inContext->UpdateSubresource(cbuf, 0, nullptr, &mConstantBufferData, 0, 0);
	inContext->VSSetConstantBuffers(0, 1, &cbuf);
	
	// set vertex data
	ID3D11Buffer* mesh_verts = mMesh->GetVertexBuffer();
	UINT stride = mMesh->GetStride();
	UINT offset = mMesh->GetOffset();
	inContext->IASetVertexBuffers( 0, 1, &mesh_verts, &stride, &offset);
	inContext->IASetInputLayout(mVertexShader->GetLayout());

	ID3D11Buffer* mesh_indices = mMesh->GetIndexBuffer();
	inContext->IASetIndexBuffer(mesh_indices, DXGI_FORMAT_R16_UINT, 0);
	
	inContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw
	inContext->DrawIndexed(mMesh->GetNumIndices(), 0, 0);

	// reset shader resources
	ID3D11ShaderResourceView* none[] = { nullptr };
	inContext->PSSetShaderResources(0, 1, none);
	inContext->PSSetShaderResources(1, 1, none);
	inContext->PSSetShaderResources(2, 1, none);
}