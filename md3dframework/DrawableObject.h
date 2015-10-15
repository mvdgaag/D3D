#pragma once
#include "BaseDrawable.h"
#include "LinearAlgebra.h"


// predefinitions
class Mesh;
class Material;
class ConstantBuffer;


class DrawableObject : public BaseDrawable
{
public:
	DrawableObject() : BaseDrawable() {}
	~DrawableObject() {};

	void Init(Mesh* inMesh, Material* inMaterial);
	void Draw();

	Mesh* GetMesh() { return mMesh; }
	Material* GetMaterial() { return mMaterial; }

private:
	struct ConstantBufferData
	{
		DirectX::XMMATRIX MVP;
		DirectX::XMMATRIX prevMVP;
		DirectX::XMFLOAT4 offsets;
	};
	ConstantBufferData mConstantBufferData;
	
	Mesh* mMesh = nullptr;
	Material* mMaterial = nullptr;
	ConstantBuffer* mConstantBuffer = nullptr;
};

