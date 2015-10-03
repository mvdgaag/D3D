#pragma once
#include "LinearAlgebra.h"
#include <unordered_map>
#include <d3d11_1.h>


// predefiitions
class Mesh;
class Texture;
class Sampler;
class PixelShader;
class VertexShader;
class ConstantBuffer;


class DrawableObject
{
public:
	DrawableObject(std::string inName);
	~DrawableObject();

	void Init(Mesh* inMesh, PixelShader* inPixelShader, VertexShader* inVertexShader,
		Texture* inDiffuseTexture, Sampler* inDiffuseSampler,
		Texture* inNormalTexture, Sampler* inNormalSampler,
		Texture* inMaterialTexture, Sampler* inMaterialSampler);
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
	std::string mName;
	DirectX::XMMATRIX mTransform;

	struct ConstantBufferData
	{
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMMATRIX WVP;
		DirectX::XMMATRIX MV;
	};
	ConstantBufferData mConstantBufferData;
	
	Mesh* mMesh = nullptr;

	Texture* mDiffuseTexture = nullptr;
	Sampler* mDiffuseSampler = nullptr;
	Texture* mNormalTexture = nullptr;
	Sampler* mNormalSampler = nullptr;
	Texture* mMaterialTexture = nullptr;
	Sampler* mMaterialSampler = nullptr;

	PixelShader* mPixelShader = nullptr;
	VertexShader* mVertexShader = nullptr;

	ConstantBuffer* mConstantBuffer = nullptr;

	static std::unordered_map<std::string, DrawableObject*> sDrawableObjectMap;
};

