#pragma once
#include "BaseDrawable.h"
#include "LinearAlgebra.h"
#include <d3d11_1.h>


// predefiitions
class Mesh;
class Texture;
class Sampler;
class PixelShader;
class VertexShader;
class ConstantBuffer;
class Material;


class DrawableObject : public BaseDrawable
{
public:
	DrawableObject() : BaseDrawable() {}
	~DrawableObject() {};

	void Init(Mesh* inMesh, PixelShader* inPixelShader, VertexShader* inVertexShader,
		Texture* inDiffuseTexture, Sampler* inDiffuseSampler,
		Texture* inNormalTexture, Sampler* inNormalSampler,
		Texture* inMaterialTexture, Sampler* inMaterialSampler);
	void Init(Mesh* inMesh, Material* inMaterial);
	void Draw();

	PixelShader*	GetPixelShader()		{ return mPixelShader; }
	VertexShader*	GetVertexShader()		{ return mVertexShader; }
	
	Texture*		GetDiffuseTexture()		{ return mDiffuseTexture; }
	Texture*		GetNormalTexture()		{ return mNormalTexture; }
	Texture*		GetMaterialTexture()	{ return mMaterialTexture; }

	Sampler*		GetDiffuseSampler()		{ return mDiffuseSampler; }
	Sampler*		GetNormalSampler()		{ return mNormalSampler; }
	Sampler*		GetMaterialSampler()	{ return mMaterialSampler; }

private:
	struct ConstantBufferData
	{
		DirectX::XMMATRIX MVP;
		DirectX::XMMATRIX prevMVP;
		DirectX::XMFLOAT4 offsets;
	};
	ConstantBufferData mConstantBufferData;
	DirectX::XMMATRIX mPrevViewMatrix;
	DirectX::XMMATRIX mPrevProjectionMatrix;
	
	Mesh* mMesh = nullptr;

	Material* mMaterial;

	Texture* mDiffuseTexture = nullptr;
	Sampler* mDiffuseSampler = nullptr;
	Texture* mNormalTexture = nullptr;
	Sampler* mNormalSampler = nullptr;
	Texture* mMaterialTexture = nullptr;
	Sampler* mMaterialSampler = nullptr;

	PixelShader* mPixelShader = nullptr;
	VertexShader* mVertexShader = nullptr;

	ConstantBuffer* mConstantBuffer = nullptr;
};

